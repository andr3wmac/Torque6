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

#include "console/consoleTypes.h"
#include "megaTerrain.h"
#include "graphics/core.h"
#include "rendering/rendering.h"
#include "rendering/renderCamera.h"
#include "TerrainCell.h"

// Script bindings.
#include "megaTerrain_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

// bgfx/bx
#include <bgfx/bgfx.h>
#include <bx/fpumath.h>

// Assimp - Asset Import Library
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

namespace Scene
{
   IMPLEMENT_PLUGIN_CONOBJECT(MegaTerrain);

   MegaTerrain::MegaTerrain()
   {
      megaTextureSize         = 4096;
      megaTexture.idx         = bgfx::invalidHandle;
      megaTextureBuffer.idx   = bgfx::invalidHandle;
      megaShader.idx          = bgfx::invalidHandle;
      textures[0].idx         = bgfx::invalidHandle;
      textures[1].idx         = bgfx::invalidHandle;
      textures[2].idx         = bgfx::invalidHandle;
      v_TerrainMegaTexture    = NULL;
      redrawMegaTexture       = true;

      // Load Shader
      Graphics::ShaderAsset* megaShaderAsset = Torque::Graphics.getShaderAsset("Terrain:megaShader");
      if (megaShaderAsset)
         megaShader = megaShaderAsset->getProgram();

      const U32 samplerFlags = 0
         | BGFX_TEXTURE_RT
         | BGFX_TEXTURE_MIN_POINT
         | BGFX_TEXTURE_MAG_POINT
         | BGFX_TEXTURE_MIP_POINT
         | BGFX_TEXTURE_U_CLAMP
         | BGFX_TEXTURE_V_CLAMP;

      // G-Buffer
      megaTexture       = Torque::bgfx.createTexture2D((U16)megaTextureSize, (U16)megaTextureSize, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT | BGFX_TEXTURE_U_CLAMP, NULL);
      megaTextureBuffer = Torque::bgfx.createFrameBuffer(1, &megaTexture, false);

      lastFocusPoint.set(0, 0);
      uniformSet.uniforms = new Vector<Rendering::UniformData>;

      // View
      v_TerrainMegaTexture = Torque::Graphics.getView("TerrainMegaTexture", 900, NULL);
   }

   MegaTerrain::~MegaTerrain()
   {
      Torque::bgfx.destroyFrameBuffer(megaTextureBuffer);
   }

   void MegaTerrain::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();
   }

   void MegaTerrain::onAddToScene()
   {
      refresh();
   }

   void MegaTerrain::preRender(Rendering::RenderCamera* camera)
   {
      if (terrainGrid.size() < 1)
         return;

      // Calculate focus point.
      Point3F camPos = camera->position;
      Point2F focusPoint;
      focusPoint.set((camPos.x / terrainGrid[0].width) - terrainGrid[0].gridX, (camPos.y / terrainGrid[0].height) - terrainGrid[0].gridY);
      Point2F diff = lastFocusPoint - focusPoint;
      if (diff.len() > 0.025f)
      {
         lastFocusPoint = focusPoint;
         refresh();
      }
   }

   void MegaTerrain::render(Rendering::RenderCamera* camera)
   {
      if (terrainGrid.size() < 1)
         return;

      // Check for dirty cells.
      for (S32 n = 0; n < terrainGrid.size(); ++n)
      {
         if (terrainGrid[n].dirty)
         {
            redrawMegaTexture = true;
            terrainGrid[n].dirty = false;
         }
      }

      if (redrawMegaTexture)
      {
         redrawMegaTexture = false;

         F32 proj[16];
         bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
         Torque::bgfx.setViewFrameBuffer(v_TerrainMegaTexture->id, megaTextureBuffer);
         Torque::bgfx.setViewTransform(v_TerrainMegaTexture->id, NULL, proj, BGFX_VIEW_STEREO, NULL);
         Torque::bgfx.setViewRect(v_TerrainMegaTexture->id, 0, 0, (U16)megaTextureSize, (U16)megaTextureSize);

         U8 tex_offset = 0;
         if (terrainGrid[0].mBlendTexture.idx != bgfx::invalidHandle)
         {
            tex_offset++;
            Torque::bgfx.setTexture(0, Torque::Graphics.getTextureUniform(0), terrainGrid[0].mBlendTexture, UINT32_MAX);
         }

         for (U8 n = 0; n < 3; ++n)
         {
            if (textures[n].idx != bgfx::invalidHandle)
            {
               Torque::bgfx.setTexture(n + tex_offset, Torque::Graphics.getTextureUniform(n + tex_offset), textures[n], UINT32_MAX);
            }
         }

         // Setup Uniforms
         if (!uniformSet.isEmpty())
         {
            for (S32 i = 0; i < uniformSet.uniforms->size(); ++i)
            {
               Rendering::UniformData* uniform = &uniformSet.uniforms->at(i);
               Torque::bgfx.setUniform(uniform->uniform, uniform->_dataPtr, (U16)uniform->count);
            }
         }

         Torque::bgfx.setState(BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE, 0);
         Torque::Graphics.fullScreenQuad((F32)megaTextureSize, (F32)megaTextureSize, 0.0f);
         Torque::bgfx.submit(v_TerrainMegaTexture->id, megaShader, 0, false);
      }
   }

   void MegaTerrain::postRender(Rendering::RenderCamera* camera)
   {

   }

   void MegaTerrain::loadEmptyTerrain(S32 gridX, S32 gridY, S32 width, S32 height)
   {
      for (S32 n = 0; n < terrainGrid.size(); ++n)
      {
         if (terrainGrid[n].gridX != gridX || terrainGrid[n].gridY != gridY)
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

   void MegaTerrain::loadHeightMap(S32 gridX, S32 gridY, const char* heightMap)
   {
      for (S32 n = 0; n < terrainGrid.size(); ++n)
      {
         if (terrainGrid[n].gridX != gridX || terrainGrid[n].gridY != gridY)
            continue;

         terrainGrid[n].loadHeightMap(heightMap);
         return;
      }

      // Create new cell
      TerrainCell cell(&megaTexture, uniformSet.uniforms, gridX, gridY);
      terrainGrid.push_back(cell);
      terrainGrid.back().loadHeightMap(heightMap);

      refresh();
   }

   void MegaTerrain::loadTexture(S32 slot, const char* texture)
   {
      TextureObject* texture_obj = Torque::Graphics.loadTexture(texture, TextureHandle::BitmapKeepTexture, BGFX_TEXTURE_NONE, false, false);
      if (texture_obj)
         textures[slot] = texture_obj->getBGFXTexture();

      refresh();
   }

   void MegaTerrain::refresh()
   {
      Parent::refresh();

      if (terrainGrid.size() < 1)
         return;

      uniformSet.clear();

      Rendering::UniformData* u_focusPoint = uniformSet.addUniform();
      u_focusPoint->count = 1;
      u_focusPoint->uniform = Torque::Graphics.getUniformVec4("focusPoint", 1);
      u_focusPoint->setValue(Point4F(lastFocusPoint.x, lastFocusPoint.y, 0.0f, 0.0f));

      Rendering::UniformData* u_cascadeSize = uniformSet.addUniform();
      u_cascadeSize->count = 1;
      u_cascadeSize->uniform = Torque::Graphics.getUniformVec4("cascadeSize", 1);
      u_cascadeSize->setValue(Point4F(0.1f, 0.25f, 0.5f, 0.0f));

      Rendering::UniformData* u_layerScale = uniformSet.addUniform();
      u_layerScale->count = 1;
      u_layerScale->uniform = Torque::Graphics.getUniformVec4("layerScale", 1);
      u_layerScale->setValue(Point4F(16.0f, 1.0f, 1.0f, 1.0f));

      redrawMegaTexture = true;
   }
}
