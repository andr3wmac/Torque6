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


#include "forwardRendering.h"
#include "console/consoleInternal.h"
#include "graphics/shaders.h"

#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Scene
{
   Vector<ForwardRenderData> forwardRenderList;
   Vector<LightData> lightList;

   void renderForward()
   {
      for (S32 n = 0; n < forwardRenderList.size(); ++n)
      {
         ForwardRenderData* item = &forwardRenderList[n];

         // Transform Table.
         bgfx::setTransform(item->transformTable, item->transformCount);

         // Shader and Buffers
         bgfx::setProgram(item->shader);
	      bgfx::setVertexBuffer(item->vertexBuffer);
	      bgfx::setIndexBuffer(item->indexBuffer);
         
         // Setup Textures
         for (S32 i = 0; i < item->textures.size(); ++i)
            bgfx::setTexture(i, item->textures[i].uniform, item->textures[i].handle);

         // Setup Uniforms
         for (S32 i = 0; i < item->uniforms.size(); ++i)
            bgfx::setUniform(item->uniforms[i].uniform, item->uniforms[i].data, item->uniforms[i].count);

	      // Set render states.
	      bgfx::setState(BGFX_STATE_DEFAULT);

	      // Submit primitive for rendering to view 0.
	      bgfx::submit(0);
      }
   }

   void testGetNearestLights()
   {
      for( U32 n = 0; n < 10000; ++n )
      {
         Scene::LightData lData;
         lData.position = Point3F(mRandF(-10000, 10000), mRandF(-10000, 10000), mRandF(-10000, 10000));
         lightList.push_back(lData);
      }
      Vector<LightData*> nearestLights = getNearestLights(Point3F(0, 0, 0));
      for( U32 n = 0; n < nearestLights.size(); ++n )
      {
         Con::printf("Nearest Light: %f %f %f", nearestLights[n]->position.x, nearestLights[n]->position.y, nearestLights[n]->position.z);
      }
      lightList.clear();
   }

   Vector<LightData*> getNearestLights(Point3F position)
   {

      U64 hpFreq = bx::getHPFrequency() / 1000000.0; // micro-seconds.
      U64 startTime = bx::getHPCounter();

      Vector<LightData*> results;
      F32 lightDistance[4];

      // Con::printf("Finding shortest distance from: %f %f %f", position.x, position.y, position.z);

      U32 n, i;
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
      U64 endTime = bx::getHPCounter();

      //Con::printf("getNearestLights took: %d microseconds. (1 microsecond = 0.001 milliseconds)", (U32)((endTime - startTime) / hpFreq));
      //for( i = 0; i < results.size(); ++i )
      //{
      //   Con::printf("Distance to light: %f Light Position: %f %f %f", lightDistance[i], results[i]->position.x, results[i]->position.y, results[i]->position.z); 
      //   Con::printf("Light Color: %f %f %f", results[i]->color[0], results[i]->color[1], results[i]->color[2]);
      //}

      return results;
   }
}