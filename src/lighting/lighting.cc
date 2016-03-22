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

#include "lighting.h"
#include "console/consoleInternal.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "scene/scene.h"
#include "rendering/rendering.h"
#include "rendering/renderCamera.h"
#include "scene/components/cameraComponent.h"

#include <bx/fpumath.h>

namespace Lighting
{
   // Lighting
   LightData            lightList[2048];
   U32                  lightCount = 0;
   DirectionalLight     directionalLight;
   bool                 usingDefaultSkyLight = true;
   EnvironmentLight     skyLight;

   void init()
   {
      // Directional Light
      directionalLight.color = ColorF(0.0f, 0.0f, 0.0f, 0.0f);
      directionalLight.direction = Point3F(0.0f, 0.0f, 0.0f);
      directionalLight.shadowMap.idx = bgfx::invalidHandle;
      directionalLight.shadowMapUniform = bgfx::createUniform("ShadowMap", bgfx::UniformType::Int1);

      // Sky Light
      skyLight.radianceCubemap = bgfx::createTextureCube(1, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT);
      skyLight.radianceCubemapUniform = bgfx::createUniform("RadianceCubemap", bgfx::UniformType::Int1);
      skyLight.irradianceCubemap = bgfx::createTextureCube(1, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT);
      skyLight.irradianceCubemapUniform = bgfx::createUniform("IrradianceCubemap", bgfx::UniformType::Int1);
      skyLight.brdfTexture = bgfx::createTexture2D(1, 1, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT);
      skyLight.brdfTextureUniform = bgfx::createUniform("BRDFTexture", bgfx::UniformType::Int1);
   }

   void destroy()
   {
      bgfx::destroyUniform(directionalLight.shadowMapUniform);
      bgfx::destroyUniform(skyLight.radianceCubemapUniform);
      bgfx::destroyUniform(skyLight.irradianceCubemapUniform);
      bgfx::destroyUniform(skyLight.brdfTextureUniform);
   }

   // ----------------------------------------
   //   Lights
   // ----------------------------------------
   LightData* createLightData()
   {
      LightData* light = NULL;
      for (U32 n = 0; n < lightCount; ++n)
      {
         if (lightList[n].flags & LightData::Deleted)
         {
            light = &lightList[n];
            break;
         }
      }

      if (light == NULL)
      {
         light = &lightList[lightCount];
         lightCount++;
      }

      // Reset Values
      light->flags = 0;
      light->color[0] = 0.0f;
      light->color[1] = 0.0f;
      light->color[2] = 0.0f;
      light->attenuation = 0.0f;
      light->intensity = 0.0f;
      light->position.set(0.0f, 0.0f, 0.0f);

      return light;
   }

   Vector<LightData*> getLightList()
   {
      Vector<LightData*> results;
      for (U32 n = 0; n < lightCount; ++n)
      {
         if (lightList[n].flags & LightData::Deleted)
            continue;
         results.push_back(&lightList[n]);
      }
      return results;
   }

   Vector<LightData*> getNearestLights(Point3F position)
   {
      //U64 hpFreq = bx::getHPFrequency() / 1000000.0; // micro-seconds.
      //U64 startTime = bx::getHPCounter();

      Vector<LightData*> results;
      F32 lightDistance[4];

      U32 n;
      S32 i;
      for (n = 0; n < lightCount; ++n)
      {
         if (lightList[n].flags & LightData::Deleted)
            continue;

         // Get distance.
         Point3F lightVector = position - lightList[n].position;
         F32 distToLight = lightVector.len();

         // We need a max of 4 lights
         if (results.size() < 4)
         {
            results.push_back(&lightList[n]);
            lightDistance[results.size() - 1] = distToLight;
            continue;
         }

         // Check for shortest distance.
         for (i = 0; i < 4; ++i)
         {
            if (lightDistance[i] < distToLight)
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

   // Directional Light
   void setDirectionalLight(Point3F direction, ColorF color, bgfx::TextureHandle shadowMap)
   {
      directionalLight.color     = color;
      directionalLight.direction = direction;
      directionalLight.shadowMap = shadowMap;
   }

   // Sky Light
   void setSkyLight(bgfx::TextureHandle radianceCubemap, bgfx::TextureHandle irradianceCubemap, bgfx::TextureHandle brdfTexture)
   {
      usingDefaultSkyLight       = false;
      skyLight.radianceCubemap   = radianceCubemap;
      skyLight.irradianceCubemap = irradianceCubemap;
      skyLight.brdfTexture       = brdfTexture;
   }

   // Debug Function
   void testGetNearestLights()
   {
      /*for( U32 n = 0; n < 10000; ++n )
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
      */
   }
}
