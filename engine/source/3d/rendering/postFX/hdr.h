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


#ifndef _HDR_POSTFX_H_
#define _HDR_POSTFX_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _RENDERINGCOMMON_H_
#include "common.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx.h>
#endif

#ifndef _POST_RENDERING_H_
#include "../postRendering.h"
#endif

namespace Rendering
{
   // HDR Post Processing
   // Based On: https://github.com/bkaradzic/bgfx/tree/master/examples/09-hdr

   class HDRPostFX : public PostFX
   {
      protected:
         F32 s_texelHalf;
         F32 middleGray;
	      F32 white;
	      F32 threshold;

         Graphics::ViewTableEntry* v_HDR_Luminance;
         Graphics::ViewTableEntry* v_HDR_Downscale_Luminance0;
         Graphics::ViewTableEntry* v_HDR_Downscale_Luminance1;
         Graphics::ViewTableEntry* v_HDR_Downscale_Luminance2;
         Graphics::ViewTableEntry* v_HDR_Downscale_Luminance3;
         Graphics::ViewTableEntry* v_HDR_Brightness;
         Graphics::ViewTableEntry* v_HDR_BlurY;
         Graphics::ViewTableEntry* v_HDR_BlurX_Tonemap; 

         bgfx::UniformHandle u_tonemap;
	      bgfx::UniformHandle u_offset;

         bgfx::FrameBufferHandle lum[5];
         bgfx::FrameBufferHandle bright;
         bgfx::FrameBufferHandle blur;

         Graphics::Shader* lumShader;
	      Graphics::Shader* lumAvgShader;
	      Graphics::Shader* blurShader;
	      Graphics::Shader* brightShader;
	      Graphics::Shader* tonemapShader;

         void setOffsets2x2Lum(bgfx::UniformHandle _handle, U32 _width, U32 _height);
         void setOffsets4x4Lum(bgfx::UniformHandle _handle, U32 _width, U32 _height);

      public:
         HDRPostFX();
         ~HDRPostFX();
         void render();
   };
}

#endif