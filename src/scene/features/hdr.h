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


#ifndef _HDR_FEATURE_H_
#define _HDR_FEATURE_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _RENDERINGCOMMON_H_
#include "common.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx/bgfx.h>
#endif

#ifndef _POST_RENDERING_H_
#include "rendering/postRendering.h"
#endif

namespace Scene
{
   // HDR: High Dynamic Range
   // Based On: https://github.com/bkaradzic/bgfx/tree/master/examples/09-hdr

   class HDR : public Rendering::PostRenderFeature
   {
      private:
         typedef Rendering::PostRenderFeature Parent;

      protected:
         F32 mTexelHalf;
         F32 mMiddleGray;
         F32 mWhite;
         F32 mThreshold;

         Graphics::ViewTableEntry* mLuminanceView;
         Graphics::ViewTableEntry* mDownscale_Luminance0View;
         Graphics::ViewTableEntry* mDownscale_Luminance1View;
         Graphics::ViewTableEntry* mDownscale_Luminance2View;
         Graphics::ViewTableEntry* mDownscale_Luminance3View;
         Graphics::ViewTableEntry* mBrightnessView;
         Graphics::ViewTableEntry* mBlurYView;
         Graphics::ViewTableEntry* mBlurX_TonemapView;

         bgfx::UniformHandle mTonemapUniform;
         bgfx::UniformHandle mOffsetUniform;

         bgfx::FrameBufferHandle mLuminanceBuffer[5];
         bgfx::FrameBufferHandle mBrightBuffer;
         bgfx::FrameBufferHandle mBlurBuffer;

         Graphics::Shader* mLumShader;
         Graphics::Shader* mLumAvgShader;
         Graphics::Shader* mBlurShader;
         Graphics::Shader* mBrightShader;
         Graphics::Shader* mTonemapShader;

         void setOffsets2x2Lum(bgfx::UniformHandle _handle, U32 _width, U32 _height);
         void setOffsets4x4Lum(bgfx::UniformHandle _handle, U32 _width, U32 _height);

      public:
         HDR();
         ~HDR();

         virtual void onActivate();
         virtual void onDeactivate();
         virtual void render();

         static void initPersistFields();

         DECLARE_CONOBJECT(HDR);
   };
}

#endif