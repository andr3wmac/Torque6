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
#include "combinedRendering.h"
#include "3d/scene/core.h"

#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   F32 viewMatrix[16];
   F32 projectionMatrix[16];

   bool canvasSizeChanged = false;
   U32 canvasWidth = 0;
   U32 canvasHeight = 0;
   U32 canvasClearColor = 0;

   RenderData renderList[65535];
   U32 renderCount = 0;
   
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

         depthTexture = bgfx::createTexture2D(canvasWidth, canvasHeight, 1, bgfx::TextureFormat::D24, samplerFlags);
      }
      return depthTexture;
   }

   bgfx::TextureHandle getFinalTexture()
   {
      if ( !bgfx::isValid(finalTexture) )
      {
         const U32 samplerFlags = 0
		      | BGFX_TEXTURE_RT
		      | BGFX_TEXTURE_MIN_POINT
		      | BGFX_TEXTURE_MAG_POINT
		      | BGFX_TEXTURE_MIP_POINT
		      | BGFX_TEXTURE_U_CLAMP
		      | BGFX_TEXTURE_V_CLAMP;

         finalTexture = bgfx::createTexture2D(canvasWidth, canvasHeight, 1, bgfx::TextureFormat::BGRA8, samplerFlags);
      }
      return finalTexture;
   }

   void init()
   {
      combinedInit();
      deferredInit();
      forwardInit();
   }

   void destroy()
   {
      forwardDestroy();
      deferredDestroy();
      combinedDestroy();

      if ( bgfx::isValid(finalTexture) )
      {
         bgfx::destroyTexture(finalTexture);
         finalTexture.idx = bgfx::invalidHandle;
      }

      if ( bgfx::isValid(depthTexture) )
      {
         bgfx::destroyTexture(depthTexture);
         depthTexture.idx = bgfx::invalidHandle;
      }
   }

   // Process Frame
   void render(U32 width, U32 height, U32 clearColor)
   {
      canvasSizeChanged = ( canvasWidth != width || canvasHeight != height );
      canvasWidth = width;
      canvasHeight = height;
      canvasClearColor = clearColor;

      if ( canvasSizeChanged )
         resize();

      // TODO: Cache this?
      bx::mtxProj(projectionMatrix, 60.0f, float(canvasWidth)/float(canvasHeight), 0.1f, 10000.0f, true);

      // Render Layer 0 is the bottom, we want it canvas clear color.
      bgfx::setViewClear(Graphics::ViewTable::RenderLayer0
		   , BGFX_CLEAR_COLOR_BIT|BGFX_CLEAR_DEPTH_BIT
         , canvasClearColor
         , 1.0f
		   , 0
		);
      bgfx::setViewRect(Graphics::ViewTable::RenderLayer0, 0, 0, canvasWidth, canvasHeight);
      bgfx::setViewTransform(Graphics::ViewTable::RenderLayer0, viewMatrix, projectionMatrix);
      bgfx::submit(Graphics::ViewTable::RenderLayer0);

      // Render All Renderable Items ;)
      Renderable::renderAll();
   }

   void resize()
   {
      destroy();
      init();
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