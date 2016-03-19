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


#ifndef _LIGHTING_H_
#define _LIGHTING_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _RENDERING_H_
#include "rendering/rendering.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx/bgfx.h>
#endif

#ifndef _BASE_COMPONENT_H_
#include "scene/components/baseComponent.h"
#endif

#ifndef _RENDER_CAMERA_H
#include "rendering/renderCamera.h"
#endif

#ifndef _DEBUG_MODE_H_
#include "debug/debugMode.h"
#endif

namespace Lighting
{
   struct DLL_PUBLIC LightData
   {
      enum Enum
      {
         Deleted = BIT(0),
         Hidden = BIT(1)
      };

      U32      flags;
      Point3F  position;
      F32      color[3];
      F32      attenuation;
      F32      intensity;
   };

   LightData* createLightData();
   Vector<LightData*> getLightList();
   Vector<LightData*> getNearestLights(Point3F position);

   // Directional Light
   struct DLL_PUBLIC DirectionalLight
   {
      Point3F              direction;
      ColorF               color;
      bgfx::TextureHandle  shadowMap;
      bgfx::UniformHandle  shadowMapUniform;
   };

   extern DirectionalLight directionalLight;
   void setDirectionalLight(Point3F direction, ColorF color, bgfx::TextureHandle shadowMap);

   // Environment Light
   struct DLL_PUBLIC EnvironmentLight
   {
      bgfx::TextureHandle  radianceCubemap;
      bgfx::UniformHandle  radianceCubemapUniform;
      bgfx::TextureHandle  irradianceCubemap;
      bgfx::UniformHandle  irradianceCubemapUniform;
      bgfx::TextureHandle  brdfTexture;
      bgfx::UniformHandle  brdfTextureUniform;
   };

   extern EnvironmentLight environmentLight;
   void setEnvironmentLight(bgfx::TextureHandle radianceCubemap, bgfx::TextureHandle irradianceCubemap, bgfx::TextureHandle brdfTexture);

   void init();
   void destroy();
}

#endif