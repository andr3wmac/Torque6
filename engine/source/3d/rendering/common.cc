//-----------------------------------------------------------------------------
// Copyright (c) 2014 Andrew Mac
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


#include "common.h"
#include "console/consoleInternal.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/utilities.h"
#include "deferredRendering.h"
#include "forwardRendering.h"
#include "3d/scene/core.h"

#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   RenderData renderList[65535];
   U32 renderCount = 0;
   Graphics::Shader* finalShader = NULL;
   bgfx::TextureHandle finalTexture = BGFX_INVALID_HANDLE;
   bgfx::TextureHandle depthTexture = BGFX_INVALID_HANDLE;

   bgfx::TextureHandle getDepthTexture()
   {
      if ( !bgfx::isValid(depthTexture) )
      {
         const U32 samplerFlags = 0
				| BGFX_TEXTURE_RT
				| BGFX_TEXTURE_MIN_POINT
				| BGFX_TEXTURE_MAG_POINT
				| BGFX_TEXTURE_MIP_POINT
				| BGFX_TEXTURE_U_CLAMP
				| BGFX_TEXTURE_V_CLAMP;

         depthTexture = bgfx::createTexture2D(Scene::canvasWidth, Scene::canvasHeight, 1, bgfx::TextureFormat::D24, samplerFlags);
      }
      return depthTexture;
   }

   void init()
   {
      finalShader = new Graphics::Shader("shaders/final_vs.sc", "shaders/final_fs.sc");

      initBuffers();
      deferredInit();
      forwardInit();
   }

   void initBuffers()
   {
      const U32 samplerFlags = 0
		   | BGFX_TEXTURE_RT
		   | BGFX_TEXTURE_MIN_POINT
		   | BGFX_TEXTURE_MAG_POINT
		   | BGFX_TEXTURE_MIP_POINT
		   | BGFX_TEXTURE_U_CLAMP
		   | BGFX_TEXTURE_V_CLAMP;

      if ( bgfx::isValid(finalTexture) )
         bgfx::destroyTexture(finalTexture);

      finalTexture = bgfx::createTexture2D(Scene::canvasWidth, Scene::canvasHeight, 1, bgfx::TextureFormat::BGRA8, samplerFlags);

      if ( !bgfx::isValid(depthTexture) )
      {
         const U32 samplerFlags = 0
				| BGFX_TEXTURE_RT
				| BGFX_TEXTURE_MIN_POINT
				| BGFX_TEXTURE_MAG_POINT
				| BGFX_TEXTURE_MIP_POINT
				| BGFX_TEXTURE_U_CLAMP
				| BGFX_TEXTURE_V_CLAMP;

         depthTexture = bgfx::createTexture2D(Scene::canvasWidth, Scene::canvasHeight, 1, bgfx::TextureFormat::D24, samplerFlags);
      }
   }

   void destroy()
   {
      forwardDestroy();
      deferredDestroy();
      destroyBuffers();

      SAFE_DELETE(finalShader);
   }

   void destroyBuffers()
   {
      if ( bgfx::isValid(finalTexture) )
         bgfx::destroyTexture(finalTexture);

      if ( bgfx::isValid(depthTexture) )
      {
         bgfx::destroyTexture(depthTexture);
         depthTexture.idx = bgfx::invalidHandle;
      }
   }

   void resize()
   {
      // Due to relying on the shared depth and final textures
      // these need to be tore down in this order:
      deferredDestroyBuffers();
      forwardDestroyBuffers();
      destroyBuffers();

      // And built back up in this order:
      initBuffers();
      deferredInitBuffers();
      forwardInitBuffers();
   }

   RenderData* createRenderData()
   {
      RenderData* item = &renderList[renderCount];

      // Reset Values
      item->indexBuffer.idx = bgfx::invalidHandle;
      item->vertexBuffer.idx = bgfx::invalidHandle;
      item->shader.idx = bgfx::invalidHandle;
      item->transformCount = 0;
      item->transformTable = NULL;
      item->uniforms = NULL;
      item->textures = NULL;
      item->view = 0;
      item->state = BGFX_STATE_DEFAULT;

      renderCount++;
      return item;
   }

   void preRender()
   {
      if ( Scene::canvasSizeChanged )
         resize();

      bgfx::setViewClear(Graphics::ViewTable::Final
		   , BGFX_CLEAR_COLOR_BIT|BGFX_CLEAR_DEPTH_BIT
		   , 1.0f
		   , 0
		   , 1
		);
      bgfx::setViewRect(Graphics::ViewTable::Final, 0, 0, Scene::canvasWidth, Scene::canvasHeight);
      bgfx::setViewTransform(Graphics::ViewTable::Final, Scene::viewMatrix, Scene::projectionMatrix);

      deferredPreRender();
      forwardPreRender();
   }

   void render()
   {
      for (U32 n = 0; n < renderCount; ++n)
      {
         RenderData* item = &renderList[n];

         // Transform Table.
         bgfx::setTransform(item->transformTable, item->transformCount);

         // Shader and Buffers
         bgfx::setProgram(item->shader);
	      bgfx::setVertexBuffer(item->vertexBuffer);
	      bgfx::setIndexBuffer(item->indexBuffer);
         
         // Setup Textures
         if ( item->textures )
         {
            for (S32 i = 0; i < item->textures->size(); ++i)
            {
               if ( item->textures->at(i).isDepthTexture )
                  bgfx::setTexture(i, item->textures->at(i).uniform, Rendering::deferredGBuffer, 2);
               else
                  bgfx::setTexture(i, item->textures->at(i).uniform, item->textures->at(i).handle);
            }
         }

         // Setup Uniforms
         if ( item->uniforms )
         {
            for (S32 i = 0; i < item->uniforms->size(); ++i)
               bgfx::setUniform(item->uniforms->at(i).uniform, item->uniforms->at(i).data, item->uniforms->at(i).count);
         }

	      // Set render states.
	      bgfx::setState(item->state);

	      // Submit primitive
	      bgfx::submit(item->view);
      }
   }

   void postRender()
   {
      deferredPostRender();
      forwardPostRender();

      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, (F32)Scene::canvasWidth, (float)Scene::canvasHeight, 0.0f, 0.0f, 1000.0f);
      bgfx::setViewTransform(Graphics::ViewTable::Final, NULL, proj);
      bgfx::setViewRect(Graphics::ViewTable::Final, 0, 0, Scene::canvasWidth, Scene::canvasHeight);

      // Flip to screen.
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::finalTexture, 0);
		bgfx::setProgram(finalShader->mProgram);

		bgfx::setState(0
			| BGFX_STATE_RGB_WRITE
			| BGFX_STATE_ALPHA_WRITE
			);

		dglScreenQuad(0, 0, Scene::canvasWidth, Scene::canvasHeight);
      bgfx::submit(Graphics::ViewTable::Final);
   }

   Vector<LightData> lightList;
   Vector<LightData*> getNearestLights(Point3F position)
   {
      //U64 hpFreq = bx::getHPFrequency() / 1000000.0; // micro-seconds.
      //U64 startTime = bx::getHPCounter();

      Vector<LightData*> results;
      F32 lightDistance[4];

      U32 n;
      S32 i;
      U32 lightCount = lightList.size();
      for ( n = 0; n < lightCount; ++n )
      {
         // Get distance.
         Point3F lightVector = position - lightList[n].position;
         F32 distToLight = lightVector.len();

         // We need a max of 4 lights
         if ( results.size() < 4 )
         {
            results.push_back(&lightList[n]);
            lightDistance[results.size() - 1] = distToLight;
            continue;
         }

         // Check for shortest distance.
         for( i = 0; i < 4; ++i )
         {
            if ( lightDistance[i] < distToLight )
               continue;

            lightDistance[i] = distToLight;
            results[i] = &lightList[n];
            break;
         }
      }

      //U64 endTime = bx::getHPCounter();
      //Con::printf("getNearestLights took: %d microseconds. (1 microsecond = 0.001 milliseconds)", (U32)((endTime - startTime) / hpFreq));

      return results;
   }

   // Debug Function
   void testGetNearestLights()
   {
      for( U32 n = 0; n < 10000; ++n )
      {
         Rendering::LightData lData;
         lData.position = Point3F(mRandF(-10000, 10000), mRandF(-10000, 10000), mRandF(-10000, 10000));
         lightList.push_back(lData);
      }
      Vector<LightData*> nearestLights = getNearestLights(Point3F(0, 0, 0));
      for( S32 n = 0; n < nearestLights.size(); ++n )
      {
         Con::printf("Nearest Light: %f %f %f", nearestLights[n]->position.x, nearestLights[n]->position.y, nearestLights[n]->position.z);
      }
      lightList.clear();
   }

}