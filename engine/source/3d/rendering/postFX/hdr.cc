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

#include "hdr.h"
#include "console/consoleInternal.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/utilities.h"
#include "3d/scene/core.h"

#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   HDRPostFX::HDRPostFX()
   {
      middleGray = 0.18f;
	   white      = 1.1f;
	   threshold  = 1.5f;

      lumShader      = Graphics::getShader("post/hdr/vs_hdr_lum.sc",     "post/hdr/fs_hdr_lum.sc");
	   lumAvgShader   = Graphics::getShader("post/hdr/vs_hdr_lumavg.sc",  "post/hdr/fs_hdr_lumavg.sc");
	   blurShader     = Graphics::getShader("post/hdr/vs_hdr_blur.sc",    "post/hdr/fs_hdr_blur.sc");
	   brightShader   = Graphics::getShader("post/hdr/vs_hdr_bright.sc",  "post/hdr/fs_hdr_bright.sc");
	   tonemapShader  = Graphics::getShader("post/hdr/vs_hdr_tonemap.sc", "post/hdr/fs_hdr_tonemap.sc");

      lum[0] = bgfx::createFrameBuffer(128, 128, bgfx::TextureFormat::BGRA8);
	   lum[1] = bgfx::createFrameBuffer( 64,  64, bgfx::TextureFormat::BGRA8);
	   lum[2] = bgfx::createFrameBuffer( 16,  16, bgfx::TextureFormat::BGRA8);
	   lum[3] = bgfx::createFrameBuffer(  4,   4, bgfx::TextureFormat::BGRA8);
	   lum[4] = bgfx::createFrameBuffer(  1,   1, bgfx::TextureFormat::BGRA8);
	   bright = bgfx::createFrameBuffer(bgfx::BackbufferRatio::Half, bgfx::TextureFormat::BGRA8);
	   blur   = bgfx::createFrameBuffer(bgfx::BackbufferRatio::Eighth, bgfx::TextureFormat::BGRA8);

      u_tonemap = bgfx::createUniform("u_tonemap",  bgfx::UniformType::Uniform4fv);
	   u_offset  = bgfx::createUniform("u_offset",   bgfx::UniformType::Uniform4fv, 16);

      const bgfx::RendererType::Enum renderer = bgfx::getRendererType();
	   s_texelHalf = bgfx::RendererType::Direct3D9 == renderer ? 0.5f : 0.0f;
   }

   HDRPostFX::~HDRPostFX()
   {

   }

   void HDRPostFX::setOffsets2x2Lum(bgfx::UniformHandle _handle, U32 _width, U32 _height)
   {
	   float offsets[16][4];

	   float du = 1.0f/_width;
	   float dv = 1.0f/_height;

	   uint32_t num = 0;
	   for (uint32_t yy = 0; yy < 3; ++yy)
	   {
		   for (uint32_t xx = 0; xx < 3; ++xx)
		   {
			   offsets[num][0] = (xx - s_texelHalf) * du;
			   offsets[num][1] = (yy - s_texelHalf) * dv;
			   ++num;
		   }
	   }

	   bgfx::setUniform(_handle, offsets, num);
   }

   void HDRPostFX::setOffsets4x4Lum(bgfx::UniformHandle _handle, U32 _width, U32 _height)
   {
	   float offsets[16][4];

	   float du = 1.0f/_width;
	   float dv = 1.0f/_height;

	   uint32_t num = 0;
	   for (uint32_t yy = 0; yy < 4; ++yy)
	   {
		   for (uint32_t xx = 0; xx < 4; ++xx)
		   {
			   offsets[num][0] = (xx - 1.0f - s_texelHalf) * du;
			   offsets[num][1] = (yy - 1.0f - s_texelHalf) * dv;
			   ++num;
		   }
	   }

	   bgfx::setUniform(_handle, offsets, num);
   }

   void HDRPostFX::render()
   {
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);

      bgfx::setViewTransform(Graphics::HDR_Luminance, NULL, proj);
      bgfx::setViewRect(Graphics::HDR_Luminance, 0, 0, 128, 128);
		bgfx::setViewFrameBuffer(Graphics::HDR_Luminance, lum[0]);

      bgfx::setViewTransform(Graphics::HDR_Downscale_Luminance0, NULL, proj);
      bgfx::setViewRect(Graphics::HDR_Downscale_Luminance0, 0, 0, 64, 64);
		bgfx::setViewFrameBuffer(Graphics::HDR_Downscale_Luminance0, lum[1]);

      bgfx::setViewTransform(Graphics::HDR_Downscale_Luminance1, NULL, proj);
		bgfx::setViewRect(Graphics::HDR_Downscale_Luminance1, 0, 0, 16, 16);
		bgfx::setViewFrameBuffer(Graphics::HDR_Downscale_Luminance1, lum[2]);

      bgfx::setViewTransform(Graphics::HDR_Downscale_Luminance2, NULL, proj);
		bgfx::setViewRect(Graphics::HDR_Downscale_Luminance2, 0, 0, 4, 4);
		bgfx::setViewFrameBuffer(Graphics::HDR_Downscale_Luminance2, lum[3]);

      bgfx::setViewTransform(Graphics::HDR_Downscale_Luminance3, NULL, proj);
		bgfx::setViewRect(Graphics::HDR_Downscale_Luminance3, 0, 0, 1, 1);
		bgfx::setViewFrameBuffer(Graphics::HDR_Downscale_Luminance3, lum[4]);

      bgfx::setViewTransform(Graphics::HDR_Brightness, NULL, proj);
      bgfx::setViewRect(Graphics::HDR_Brightness, 0, 0, Rendering::canvasWidth/2, Rendering::canvasHeight/2);
		bgfx::setViewFrameBuffer(Graphics::HDR_Brightness, bright);

      bgfx::setViewTransform(Graphics::HDR_BlurY, NULL, proj);
      bgfx::setViewRect(Graphics::HDR_BlurY, 0, 0, Rendering::canvasWidth/8, Rendering::canvasHeight/8);
		bgfx::setViewFrameBuffer(Graphics::HDR_BlurY, blur);

      bgfx::setViewTransform(Graphics::HDR_BlurX_Tonemap, NULL, proj);
		bgfx::setViewRect(Graphics::HDR_BlurX_Tonemap, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);
      bgfx::setViewFrameBuffer(Graphics::HDR_BlurX_Tonemap, Rendering::getPostTarget());

      // Calculate luminance.
		setOffsets2x2Lum(u_offset, 128, 128);
		bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getPostSource());
      bgfx::setProgram(lumShader->mProgram);
		bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
		fullScreenQuad(128.0f, 128.0f);
		bgfx::submit(Graphics::HDR_Luminance);

		// Downscale luminance 0.
		setOffsets4x4Lum(u_offset, 128, 128);
		bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), lum[0]);
		bgfx::setProgram(lumAvgShader->mProgram);
		bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
		fullScreenQuad(64.0f, 64.0f);
		bgfx::submit(Graphics::HDR_Downscale_Luminance0);

		// Downscale luminance 1.
		setOffsets4x4Lum(u_offset, 64, 64);
		bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), lum[1]);
		bgfx::setProgram(lumAvgShader->mProgram);
		bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
		fullScreenQuad(16.0f, 16.0f);
		bgfx::submit(Graphics::HDR_Downscale_Luminance1);

		// Downscale luminance 2.
		setOffsets4x4Lum(u_offset, 16, 16);
		bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), lum[2]);
		bgfx::setProgram(lumAvgShader->mProgram);
		bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
		fullScreenQuad(4.0f, 4.0f);
		bgfx::submit(Graphics::HDR_Downscale_Luminance2);

		// Downscale luminance 3.
		setOffsets4x4Lum(u_offset, 4, 4);
		bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), lum[3]);
		bgfx::setProgram(lumAvgShader->mProgram);
		bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
		fullScreenQuad(1.0f, 1.0f);
		bgfx::submit(Graphics::HDR_Downscale_Luminance3);

      float tonemap[4] = { middleGray, mSquared(white), threshold, 0.0f };
		bgfx::setUniform(u_tonemap, tonemap);

		// Bright pass threshold is tonemap[3].
		setOffsets4x4Lum(u_offset, Rendering::canvasWidth/2, Rendering::canvasHeight/2);
		bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getPostSource());
		bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), lum[4]);
      bgfx::setProgram(brightShader->mProgram);
		bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
		fullScreenQuad((float)Rendering::canvasWidth/2.0f, (float)Rendering::canvasHeight/2.0f);
		bgfx::submit(Graphics::HDR_Brightness);

		// Blur bright pass vertically.
		bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), bright);
		bgfx::setProgram(blurShader->mProgram);
		bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad((float)Rendering::canvasWidth/8.0f, (float)Rendering::canvasHeight/8.0f);
		bgfx::submit(Graphics::HDR_BlurY);

		// Blur bright pass horizontally, do tonemaping and combine.
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getPostSource());
		bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), lum[4]);
		bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), blur);
		bgfx::setProgram(tonemapShader->mProgram);
		bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
		fullScreenQuad((float)Rendering::canvasWidth, (float)Rendering::canvasHeight);
		bgfx::submit(Graphics::HDR_BlurX_Tonemap);
   }
}
