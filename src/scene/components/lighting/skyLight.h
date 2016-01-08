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


#ifndef _SKY_LIGHT_FEATURE_H_
#define _SKY_LIGHT_FEATURE_H_

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

namespace Scene
{
   // Sky Light

   class SkyLight : public BaseComponent, public Rendering::RenderHook
   {
      private:
         typedef BaseComponent Parent;

      protected:
         Graphics::ViewTableEntry*  mDeferredAmbientView;
         Graphics::Shader*          mShader;

         // Input Sky Cubemap
         StringTableEntry           mSkyCubePath;
         bgfx::UniformHandle        mSkyCubeUniform;
         bgfx::TextureHandle        mSkyCubemap;

         // Shared between Radiance/Irradiance
         bgfx::UniformHandle        mGenerateParamsUniform;

         // Radiance (Specular)
         U32                        mRadianceSize;
         bgfx::UniformHandle        mRadianceCubeUniform;
         bgfx::TextureHandle        mRadianceCubemap;
         bool                       mGenerateRadiance;
         bool                       mRadianceReady;
         Graphics::Shader*          mGenerateRadianceShader;

         // Irradiance (Diffuse)
         bgfx::UniformHandle        mIrradianceCubeUniform;
         bgfx::TextureHandle        mIrradianceCubemap;
         bool                       mGenerateIrradiance;
         bool                       mIrradianceReady;
         Graphics::Shader*          mGenerateIrradianceShader;

         void initBuffers();
         void destroyBuffers();

      public:
         SkyLight();
         ~SkyLight();

         virtual void preRender();
         virtual void render();
         virtual void postRender();
         virtual void resize();

         virtual void onAddToScene();
         virtual void onRemoveFromScene();

         virtual void onAddToCamera();
         virtual void onRemoveFromCamera();

         void loadSkyCubeTexture(StringTableEntry path);
         void generateRadianceCubeTexture();
         void generateIrradianceCubeTexture();
         void refresh();

         static bool setSkyCube(void* obj, const char* data) { static_cast<SkyLight*>(obj)->loadSkyCubeTexture(StringTable->insert(data)); return false; }

         static void initPersistFields();

         DECLARE_CONOBJECT(SkyLight);
   };
}

#endif