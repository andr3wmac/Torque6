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
   class CubemapProcessor;

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
         bgfx::TextureHandle        mSkyCubemap;
         bgfx::TextureInfo          mSkyCubemapInfo;

         CubemapProcessor*          mCubemapProcessor;
         bgfx::UniformHandle        mIrradianceCubeUniform;
         bgfx::TextureHandle        mIrradianceCubemap;
         bgfx::UniformHandle        mRadianceCubeUniform;
         bgfx::TextureHandle        mRadianceCubemap;

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

         void refresh();

         static bool setSkyCube(void* obj, const char* data) { static_cast<SkyLight*>(obj)->loadSkyCubeTexture(StringTable->insert(data)); return false; }

         static void initPersistFields();

         DECLARE_CONOBJECT(SkyLight);
   };

   class CubemapProcessor
   {
      protected:
         bgfx::TextureHandle mSourceCubemap;
         U32 mSourceSize;
         bgfx::TextureHandle mRadianceCubemap;
         U32 mRadianceSize;
         bgfx::TextureHandle mIrradianceCubemap;
         U32 mIrradianceSize;

      public:
         virtual void init(bgfx::TextureHandle sourceCubemap, U32 sourceSize,
            bgfx::TextureHandle radianceCubemap, U32 radianceSize,
            bgfx::TextureHandle irradianceCubemap, U32 irradianceSize)
         {
            mSourceCubemap = sourceCubemap;
            mSourceSize = sourceSize;
            mRadianceCubemap = radianceCubemap;
            mRadianceSize = radianceSize;
            mIrradianceCubemap = irradianceCubemap;
            mIrradianceSize = irradianceSize;
         }

         virtual void process() { }
         virtual bool isFinished() { return false; }
   };

   class GPUCubemapProcessor : public CubemapProcessor
   {
      protected:
         // Shared between Radiance/Irradiance
         bgfx::UniformHandle        mSourceCubemapUniform;
         bgfx::UniformHandle        mGenerateParamsUniform;

         // Radiance (Specular)
         bool                       mGenerateRadiance;
         bool                       mRadianceReady;
         Graphics::Shader*          mGenerateRadianceShader;

         // Irradiance (Diffuse)
         bool                       mGenerateIrradiance;
         bool                       mIrradianceReady;
         Graphics::Shader*          mGenerateIrradianceShader;

         void generateRadianceCubeTexture();
         void generateIrradianceCubeTexture();

      public:
         GPUCubemapProcessor();
         ~GPUCubemapProcessor();

         virtual void init(bgfx::TextureHandle sourceCubemap, U32 sourceSize,
            bgfx::TextureHandle radianceCubemap, U32 radianceSize,
            bgfx::TextureHandle irradianceCubemap, U32 irradianceSize);
         virtual void process();
         virtual bool isFinished();
   };

   class CPUCubemapProcessor : public CubemapProcessor
   {
      protected:
         U32* mSourceBuffer;
         U32* mRadianceBuffer;
         U32* mIrradianceBuffer;
         U32  mStage;

      public:
         CPUCubemapProcessor();
         ~CPUCubemapProcessor();

         virtual void init(bgfx::TextureHandle sourceCubemap, U32 sourceSize,
            bgfx::TextureHandle radianceCubemap, U32 radianceSize,
            bgfx::TextureHandle irradianceCubemap, U32 irradianceSize);

         virtual void process();
         virtual bool isFinished();
   };
}

#endif