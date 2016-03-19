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


#ifndef _CUBEMAP_PROCESSOR_H_
#define _CUBEMAP_PROCESSOR_H_

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

namespace Lighting
{
   class CubemapProcessor
   {
      protected:
         bgfx::TextureHandle mSourceCubemap;
         U32 mSourceSize;
         bgfx::TextureHandle mBRDFTexture;
         bgfx::TextureHandle mRadianceCubemap;
         U32 mRadianceSize;
         bgfx::TextureHandle mIrradianceCubemap;
         U32 mIrradianceSize;

      public:
         virtual void init(bgfx::TextureHandle sourceCubemap, U32 sourceSize,
            bgfx::TextureHandle radianceCubemap, U32 radianceSize,
            bgfx::TextureHandle irradianceCubemap, U32 irradianceSize,
            bgfx::TextureHandle brdfTexture)
         {
            mSourceCubemap       = sourceCubemap;
            mSourceSize          = sourceSize;
            mBRDFTexture         = brdfTexture;
            mRadianceCubemap     = radianceCubemap;
            mRadianceSize        = radianceSize;
            mIrradianceCubemap   = irradianceCubemap;
            mIrradianceSize      = irradianceSize;
         }

         virtual void process() { }
         virtual bool isFinished() { return false; }
   };

   class GPUCubemapProcessor : public CubemapProcessor
   {
      protected:
         U32 mStage;

         // Shared between Radiance/Irradiance
         bgfx::TextureHandle  mHammersleyLUT;
         bgfx::UniformHandle  mSourceCubemapUniform;
         bgfx::UniformHandle  mGenerateParamsUniform;

         // Shaders
         Graphics::Shader*    mGenerateBRDFShader;
         Graphics::Shader*    mGenerateRadianceShader;
         Graphics::Shader*    mGenerateIrradianceShader;

         U32                  mGenerateRadianceMip;

         void generateBRDFTexture();
         void generateRadianceCubeTexture();
         void generateIrradianceCubeTexture();

      public:
         GPUCubemapProcessor();
         ~GPUCubemapProcessor();

         virtual void init(bgfx::TextureHandle sourceCubemap, U32 sourceSize,
            bgfx::TextureHandle radianceCubemap, U32 radianceSize,
            bgfx::TextureHandle irradianceCubemap, U32 irradianceSize,
            bgfx::TextureHandle brdfTexture);
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
            bgfx::TextureHandle irradianceCubemap, U32 irradianceSize,
            bgfx::TextureHandle brdfTexture);

         virtual void process();
         virtual bool isFinished();
   };
}

#endif // _CUBEMAP_PROCESSOR_H_