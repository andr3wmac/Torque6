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
#include <graphics/core.h>
#include <bx/fpumath.h>

#include "TerrainCell.h"

// Link to Editor Plugin
#include "TerrainEditor.h"

using namespace Plugins;

bool                             enabled = false;
U32                              megaTextureSize = 4096;
bgfx::TextureHandle              megaTexture = BGFX_INVALID_HANDLE;
bgfx::FrameBufferHandle          megaTextureBuffer = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle              megaShader = BGFX_INVALID_HANDLE;
Graphics::ViewTableEntry*        v_TerrainMegaTexture = NULL;
bgfx::TextureHandle              textures[3] = {BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE};
Vector<Rendering::TextureData>   textureData;
bool                             redrawMegaTexture = true;
Rendering::UniformSet            uniformSet;
Point2F                          lastFocusPoint;

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
   Graphics::ShaderAsset* megaShaderAsset = Link.Graphics.getShaderAsset("Terrain:megaShader");
   if ( megaShaderAsset )
      megaShader = megaShaderAsset->getProgram();

   const U32 samplerFlags = 0
      | BGFX_TEXTURE_RT
      | BGFX_TEXTURE_MIN_POINT
      | BGFX_TEXTURE_MAG_POINT
      | BGFX_TEXTURE_MIP_POINT
      | BGFX_TEXTURE_U_CLAMP
      | BGFX_TEXTURE_V_CLAMP;

   // G-Buffer
   megaTexture = Link.bgfx.createTexture2D(megaTextureSize, megaTextureSize, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT | BGFX_TEXTURE_U_CLAMP, NULL);
   megaTextureBuffer = Link.bgfx.createFrameBuffer(1, &megaTexture, false);
   Link.requestPluginAPI("Editor", loadEditorAPI);

   lastFocusPoint.set(0, 0);
   uniformSet.uniforms = new Vector<Rendering::UniformData>;

   // View
   v_TerrainMegaTexture = Link.Graphics.getView("TerrainMegaTexture", "DeferredGeometry", true);
}

void preRender()
{
   if ( terrainGrid.size() < 1 )
      return;

   // Calculate focus point.
   Scene::SceneCamera* cam = Link.Scene.getActiveCamera();
   Point3F camPos = cam->getPosition();

   Point2F focusPoint;
   focusPoint.set((camPos.x / terrainGrid[0].width) - terrainGrid[0].gridX, (camPos.z / terrainGrid[0].height) - terrainGrid[0].gridY);
   Point2F diff = lastFocusPoint - focusPoint;
   if ( diff.len() > 0.025f )
   {
      lastFocusPoint = focusPoint;
      refresh();
   }
}

void render()
{
   if ( terrainGrid.size() < 1 )
      return;

   // Check for dirty cells.
   for ( U32 n = 0; n < terrainGrid.size(); ++n )
   {
      if ( terrainGrid[n].dirty )
      {
         redrawMegaTexture = true;
         terrainGrid[n].dirty = false;
      }
   }

   if ( redrawMegaTexture )
   {
      redrawMegaTexture = false;

      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
      Link.bgfx.setViewFrameBuffer(v_TerrainMegaTexture->id, megaTextureBuffer);
      Link.bgfx.setViewTransform(v_TerrainMegaTexture->id, NULL, proj, BGFX_VIEW_STEREO, NULL);
      Link.bgfx.setViewRect(v_TerrainMegaTexture->id, 0, 0, megaTextureSize, megaTextureSize);

      U8 tex_offset = 0;
      if ( terrainGrid[0].mBlendTexture.idx != bgfx::invalidHandle )
      {
         tex_offset++;
         Link.bgfx.setTexture(0, Link.Graphics.getTextureUniform(0), terrainGrid[0].mBlendTexture, UINT32_MAX);
      }

      for ( U32 n = 0; n < 3; ++n )
      {
         if ( textures[n].idx != bgfx::invalidHandle )
         {
            Link.bgfx.setTexture(n + tex_offset, Link.Graphics.getTextureUniform(n + tex_offset), textures[n], UINT32_MAX);
         }
      }

      // Setup Uniforms
      if ( !uniformSet.isEmpty() )
      {
         for (S32 i = 0; i < uniformSet.uniforms->size(); ++i)
         {
            Rendering::UniformData* uniform = &uniformSet.uniforms->at(i);
            Link.bgfx.setUniform(uniform->uniform, uniform->_dataPtr, uniform->count);
         }
      }

      Link.bgfx.setProgram(megaShader);
      Link.bgfx.setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE, 0);
      Link.Graphics.fullScreenQuad(megaTextureSize, megaTextureSize, 0.0f);
      Link.bgfx.submit(v_TerrainMegaTexture->id, 0);
   }
}

void destroy()
{
   //
   Link.bgfx.destroyFrameBuffer(megaTextureBuffer);
}

// Console Functions
void enableTerrain(SimObject *obj, S32 argc, const char *argv[])
{
   enabled = true;
}

void disableTerrain(SimObject *obj, S32 argc, const char *argv[])
{
   enabled = false;
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
   TerrainCell cell(&megaTexture, uniformSet.uniforms, gridX, gridY);
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
   TerrainCell cell(&megaTexture, uniformSet.uniforms, gridX, gridY);
   terrainGrid.push_back(cell);
   terrainGrid.back().loadHeightMap(argv[3]);

   refresh();
}

void loadTexture(SimObject *obj, S32 argc, const char *argv[])
{
   TextureObject* texture_obj = Link.Graphics.loadTexture(argv[2], TextureHandle::BitmapKeepTexture, false, false, false);
   if ( texture_obj )
      textures[dAtoi(argv[1])] = texture_obj->getBGFXTexture();

   refresh();
}

void refresh()
{
   if ( terrainGrid.size() < 1 )
      return;
   
   uniformSet.clear();

   Rendering::UniformData* u_focusPoint = uniformSet.addUniform();
   u_focusPoint->count = 1;
   u_focusPoint->uniform = Link.Graphics.getUniformVec4("focusPoint", 1);
   u_focusPoint->setValue(Point4F(lastFocusPoint.x, lastFocusPoint.y, 0.0f, 0.0f));

   Rendering::UniformData* u_cascadeSize = uniformSet.addUniform();
   u_cascadeSize->count = 1;
   u_cascadeSize->uniform = Link.Graphics.getUniformVec4("cascadeSize", 1);
   u_cascadeSize->setValue(Point4F(0.1f, 0.25f, 0.5f, 0.0f));

   Rendering::UniformData* u_layerScale = uniformSet.addUniform();
   u_layerScale->count = 1;
   u_layerScale->uniform = Link.Graphics.getUniformVec4("layerScale", 1);
   u_layerScale->setValue(Point4F(16.0f, 1.0f, 1.0f, 1.0f));

   redrawMegaTexture = true;
}
