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
#include "graphics/shaders.h"
#include "graphics/utilities.h"
#include "deferredRendering.h"
#include "forwardRendering.h"

#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   RenderData renderList[65535];
   U32 renderCount = 0;

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

      renderCount++;
      return item;
   }

   void render()
   {
      deferredPreRender();
      forwardPreRender();

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
               bgfx::setTexture(i, item->textures->at(i).uniform, item->textures->at(i).handle);
         }

         // Setup Uniforms
         if ( item->uniforms )
         {
            for (S32 i = 0; i < item->uniforms->size(); ++i)
               bgfx::setUniform(item->uniforms->at(i).uniform, item->uniforms->at(i).data, item->uniforms->at(i).count);
         }

	      // Set render states.
	      bgfx::setState(BGFX_STATE_DEFAULT);

	      // Submit primitive
	      bgfx::submit(item->view);
      }

      deferredPostRender();
      forwardPostRender();
   }

   // Debug Function
   void dumpRenderData()
   {
      
      Con::printf("Begin Forward Render of %d Items", renderCount);
      for (U32 n = 0; n < renderCount; ++n)
      {
         RenderData* item = &renderList[n];

         // Transform Table.
         Con::printf("Transforms Count: %d Shader: %d Vertex Buffer: %d Index Buffer %d", item->transformCount, item->shader.idx, item->vertexBuffer.idx, item->indexBuffer.idx);
         
         // Setup Textures
         if ( item->textures )
         {
            //for (S32 i = 0; i < item->textures->size(); ++i)
            //   Con::printf("Texture%d: %d", i, item->textures->at(i).handle.idx);
         }

         // Setup Uniforms
         if ( item->uniforms )
         {
            //for (S32 i = 0; i < item->uniforms->size(); ++i)
            //   Con::printf("Uniform%d: %d", i, item->uniforms->at(i).uniform.idx);
         }
      }
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