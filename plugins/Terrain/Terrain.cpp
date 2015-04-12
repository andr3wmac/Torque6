//-----------------------------------------------------------------------------
// Copyright (c) 2015 Andrew Mac
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "Terrain.h"
#include <plugins/plugins_shared.h>

#include <sim/simObject.h>
#include <3d/rendering/common.h>
#include <graphics/utilities.h>
#include <bx/fpumath.h>

#include "TerrainCell.h"

// Link to Editor Plugin
#include "TerrainEditor.h"

using namespace Plugins;

bool terrainEnabled = false;
bgfx::TextureHandle terrainTextures[1] = {BGFX_INVALID_HANDLE};
bgfx::FrameBufferHandle terrainTextureBuffer = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle terrainMegaShader = BGFX_INVALID_HANDLE;
bgfx::TextureHandle mTextures[3] = {BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE};
Vector<Rendering::TextureData> mTextureData;
bool redrawMegatexture = true;
Rendering::UniformData* u_focusPoint = NULL;
Rendering::UniformSet mUniformSet;
Point2F lastFocusPoint;

// Called when the plugin is loaded.
void create()
{
   // Register Console Functions
   Link.Con.addCommand("Terrain", "loadEmptyTerrain", loadEmptyTerrain, "", 5, 5);
   Link.Con.addCommand("Terrain", "loadHeightMap", loadHeightMap, "", 4, 4);
   Link.Con.addCommand("Terrain", "loadTexture", loadTexture, "", 3, 3);
   Link.Con.addCommand("Terrain", "enable", enableTerrain, "", 1, 1);
   Link.Con.addCommand("Terrain", "disable", disableTerrain, "", 1, 1);
   Link.Con.addCommand("Terrain", "stitchEdges", stitchEdges, "", 1, 1);

   // Load Shader
   Graphics::ShaderAsset* terrainMegaShaderAsset = Plugins::Link.Graphics.getShaderAsset("Terrain:megaShader");
   if ( terrainMegaShaderAsset )
      terrainMegaShader = terrainMegaShaderAsset->getProgram();

   const U32 samplerFlags = 0
      | BGFX_TEXTURE_RT
      | BGFX_TEXTURE_MIN_POINT
      | BGFX_TEXTURE_MAG_POINT
      | BGFX_TEXTURE_MIP_POINT
      | BGFX_TEXTURE_U_CLAMP
      | BGFX_TEXTURE_V_CLAMP;

   // G-Buffer
   terrainTextures[0] = Link.bgfx.createTexture2D(4096, 4096, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT | BGFX_TEXTURE_U_CLAMP, NULL);
   terrainTextureBuffer = Link.bgfx.createFrameBuffer(1, terrainTextures, false);
   Link.requestPluginAPI("Editor", loadEditorAPI);

   lastFocusPoint.set(0, 0);
}

void render()
{
   if ( terrainGrid.size() < 1 )
      return;

   // Calculate focus point.
   Scene::SceneCamera* cam = Plugins::Link.Scene.getActiveCamera();
   Point3F camPos = cam->getPosition();

   Point2F focusPoint;
   focusPoint.set((camPos.x / terrainGrid[0].width) - terrainGrid[0].gridX, (camPos.z / terrainGrid[0].height) - terrainGrid[0].gridY);
   Point2F diff = lastFocusPoint - focusPoint;
   if ( diff.len() > 0.025f )
   {
      lastFocusPoint = focusPoint;
      refresh();
   }

   // Check for dirty cells.
   for ( U32 n = 0; n < terrainGrid.size(); ++n )
   {
      if ( terrainGrid[n].dirty )
      {
         redrawMegatexture = true;
         terrainGrid[n].dirty = false;
      }
   }

   if ( redrawMegatexture )
   {
      redrawMegatexture = false;

      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
      Link.bgfx.setViewFrameBuffer(Graphics::ViewTable::TerrainTexture, terrainTextureBuffer);
      Link.bgfx.setViewTransform(Graphics::ViewTable::TerrainTexture, NULL, proj, BGFX_VIEW_STEREO, NULL);
      Link.bgfx.setViewRect(Graphics::ViewTable::TerrainTexture, 0, 0, 4096, 4096);

      U8 tex_offset = 0;
      if ( terrainGrid[0].mBlendTexture.idx != bgfx::invalidHandle )
      {
         tex_offset++;
         Link.bgfx.setTexture(0, Plugins::Link.Graphics.getTextureUniform(0), terrainGrid[0].mBlendTexture, UINT32_MAX);
      }

      for ( U32 n = 0; n < 3; ++n )
      {
         if ( mTextures[n].idx != bgfx::invalidHandle )
         {
            Link.bgfx.setTexture(n + tex_offset, Plugins::Link.Graphics.getTextureUniform(n + tex_offset), mTextures[n], UINT32_MAX);
         }
      }

      // Setup Uniforms
      if ( !mUniformSet.isEmpty() )
      {
         for (S32 i = 0; i < mUniformSet.uniforms->size(); ++i)
         {
            Rendering::UniformData* uniform = &mUniformSet.uniforms->at(i);
            Link.bgfx.setUniform(uniform->uniform, uniform->data, uniform->count);
         }
      }

      Link.bgfx.setProgram(terrainMegaShader);
      Link.bgfx.setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE, 0);
      //Link.Graphics.fullScreenQuad(4096, 4096);
      Link.Graphics.screenSpaceQuad(0, 0, 4096, 4096, 4096, 4096);
      Link.bgfx.submit(Graphics::ViewTable::TerrainTexture, 0);
   }
}

void destroy()
{
   //
   Link.bgfx.destroyFrameBuffer(terrainTextureBuffer);
}

// Console Functions
void enableTerrain(SimObject *obj, S32 argc, const char *argv[])
{
   terrainEnabled = true;
}

void disableTerrain(SimObject *obj, S32 argc, const char *argv[])
{
   terrainEnabled = false;
}

void loadEmptyTerrain(SimObject *obj, S32 argc, const char *argv[])
{
   S32 gridX = dAtoi(argv[1]);
   S32 gridY = dAtoi(argv[2]);
   S32 width = dAtoi(argv[3]);
   S32 height = dAtoi(argv[4]);
   for(U32 n = 0; n < terrainGrid.size(); ++n)
   {
      if ( terrainGrid[n].gridX != gridX || terrainGrid[n].gridY != gridY )
         continue;

      terrainGrid[n].loadEmptyTerrain(width, height);
      return;
   }

   // Create new cell
   TerrainCell cell(&terrainTextures[0], mUniformSet.uniforms, gridX, gridY);
   terrainGrid.push_back(cell);
   terrainGrid.back().loadEmptyTerrain(width, height);

   refresh();
}

void loadHeightMap(SimObject *obj, S32 argc, const char *argv[])
{
   S32 gridX = dAtoi(argv[1]);
   S32 gridY = dAtoi(argv[2]);
   for(U32 n = 0; n < terrainGrid.size(); ++n)
   {
      if ( terrainGrid[n].gridX != gridX || terrainGrid[n].gridY != gridY )
         continue;

      terrainGrid[n].loadHeightMap(argv[3]);
      return;
   }

   // Create new cell
   TerrainCell cell(&terrainTextures[0], mUniformSet.uniforms, gridX, gridY);
   terrainGrid.push_back(cell);
   terrainGrid.back().loadHeightMap(argv[3]);

   refresh();
}

void loadTexture(SimObject *obj, S32 argc, const char *argv[])
{
   TextureObject* texture_obj = Plugins::Link.Graphics.loadTexture(argv[2], TextureHandle::TextureHandleType::BitmapKeepTexture, false, false, false);
   if ( texture_obj )
      mTextures[dAtoi(argv[1])] = texture_obj->getBGFXTexture();

   refresh();
}

void refresh()
{
   if ( terrainGrid.size() < 1 )
      return;
   
   mUniformSet.clear();

   Rendering::UniformData* u_focusPoint = mUniformSet.addUniform();
   u_focusPoint->count = 1;
   u_focusPoint->uniform = Plugins::Link.Graphics.getUniformVec2("focusPoint", 1);
   u_focusPoint->data = new Point2F(lastFocusPoint.x, lastFocusPoint.y);

   Rendering::UniformData* u_cascadeSize = mUniformSet.addUniform();
   u_cascadeSize->count = 1;
   u_cascadeSize->uniform = Plugins::Link.Graphics.getUniformVec3("cascadeSize", 1);
   u_cascadeSize->data = new Point3F(0.1f, 0.25f, 0.5f);

   Rendering::UniformData* u_layerScale = mUniformSet.addUniform();
   u_layerScale->count = 1;
   u_layerScale->uniform = Plugins::Link.Graphics.getUniformVec4("layerScale", 1);
   u_layerScale->data = new Point4F(16.0f, 1.0f, 1.0f, 1.0f);

   redrawMegatexture = true;
}