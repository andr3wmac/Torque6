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

#include "hdrComponent.h"
#include "console/consoleInternal.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "scene/scene.h"
#include "rendering/rendering.h"
#include "scene/components/cameraComponent.h"

#include <bgfx/bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Scene
{
   IMPLEMENT_CONOBJECT(HDRComponent);

   HDRComponent::HDRComponent()
   {
      mPriority = 4000;

      // Settings
      mMiddleGray    = 0.18f;
      mWhite         = 1.1f;
      mThreshold     = 2.5f;
      mAutoExposure  = false;
      mExposure      = 1.0f;

      // Views
      mLuminanceView             = NULL;
      mDownscale_Luminance0View  = NULL;
      mDownscale_Luminance1View  = NULL;
      mDownscale_Luminance2View  = NULL;
      mDownscale_Luminance3View  = NULL;
      mBrightnessView            = NULL;
      mBlurYView                 = NULL;
      mBlurX_TonemapView         = NULL;

      // Shaders
      mLumShader           = Graphics::getDefaultShader("components/hdr/vs_hdr_lum.tsh",     "components/hdr/fs_hdr_lum.tsh");
      mLumAvgShader        = Graphics::getDefaultShader("components/hdr/vs_hdr_lumavg.tsh",  "components/hdr/fs_hdr_lumavg.tsh");
      mBlurShader          = Graphics::getDefaultShader("components/hdr/vs_hdr_blur.tsh",    "components/hdr/fs_hdr_blur.tsh");
      mBrightShader        = Graphics::getDefaultShader("components/hdr/vs_hdr_bright.tsh",  "components/hdr/fs_hdr_bright.tsh");
      mTonemapShader       = Graphics::getDefaultShader("components/hdr/vs_hdr_tonemap.tsh", "components/hdr/fs_hdr_tonemap.tsh");
      mTonemapAutoShader   = Graphics::getDefaultShader("components/hdr/vs_hdr_tonemap.tsh", "components/hdr/fs_hdr_tonemap_auto.tsh");

      // Framebuffers
      mLuminanceBuffer[0]  = bgfx::createFrameBuffer(128, 128, bgfx::TextureFormat::BGRA8);
      mLuminanceBuffer[1]  = bgfx::createFrameBuffer( 64,  64, bgfx::TextureFormat::BGRA8);
      mLuminanceBuffer[2]  = bgfx::createFrameBuffer( 16,  16, bgfx::TextureFormat::BGRA8);
      mLuminanceBuffer[3]  = bgfx::createFrameBuffer(  4,   4, bgfx::TextureFormat::BGRA8);
      mLuminanceBuffer[4]  = bgfx::createFrameBuffer(  1,   1, bgfx::TextureFormat::BGRA8);
      mBrightBuffer        = bgfx::createFrameBuffer(bgfx::BackbufferRatio::Half, bgfx::TextureFormat::BGRA8);
      mBlurBuffer          = bgfx::createFrameBuffer(bgfx::BackbufferRatio::Eighth, bgfx::TextureFormat::BGRA8);

      // Uniforms
      mTonemapUniform   = bgfx::createUniform("u_tonemap",  bgfx::UniformType::Vec4);
      mOffsetUniform    = bgfx::createUniform("u_offset",   bgfx::UniformType::Vec4, 16);

      const bgfx::RendererType::Enum renderer = bgfx::getRendererType();
      mTexelHalf = bgfx::RendererType::Direct3D9 == renderer ? 0.5f : 0.0f;
   }

   HDRComponent::~HDRComponent()
   {
      for(U8 i = 0; i < 5; ++i )
         bgfx::destroyFrameBuffer(mLuminanceBuffer[i]);

      bgfx::destroyFrameBuffer(mBrightBuffer);
      bgfx::destroyFrameBuffer(mBlurBuffer);
      bgfx::destroyUniform(mTonemapUniform);
      bgfx::destroyUniform(mOffsetUniform);
   }

   void HDRComponent::onAddToScene()
   {
      CameraComponent* camera = mOwnerObject->findComponentByType<CameraComponent>();
      if (!camera)
         return;

      mCamera = camera->getRenderCamera();
      if (mCamera)
         mCamera->addRenderPostProcess(this);
   }

   void HDRComponent::onRemoveFromScene()
   {
      if (mCamera)
         mCamera->removeRenderPostProcess(this);
   }

   void HDRComponent::onAddToCamera()
   {
      // Views
      mLuminanceView             = mCamera->overrideBegin();
      mDownscale_Luminance0View  = Graphics::getView("HDR_Downscale_Luminance0", 4000, mCamera);
      mDownscale_Luminance1View  = Graphics::getView("HDR_Downscale_Luminance1");
      mDownscale_Luminance2View  = Graphics::getView("HDR_Downscale_Luminance2");
      mDownscale_Luminance3View  = Graphics::getView("HDR_Downscale_Luminance3");
      mBrightnessView            = Graphics::getView("HDR_Brightness");
      mBlurYView                 = Graphics::getView("HDR_BlurY");
      mBlurX_TonemapView         = Graphics::getView("HDR_BlurX_Tonemap");
   }

   void HDRComponent::onRemoveFromCamera()
   {
      mCamera->freeBegin();
      Graphics::deleteView(mDownscale_Luminance0View);
      Graphics::deleteView(mDownscale_Luminance1View);
      Graphics::deleteView(mDownscale_Luminance2View);
      Graphics::deleteView(mDownscale_Luminance3View);
      Graphics::deleteView(mBrightnessView);
      Graphics::deleteView(mBlurYView);
      Graphics::deleteView(mBlurX_TonemapView);
   }

   void HDRComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addGroup("HDR");

         addField("MiddleGray", TypeF32, Offset(mMiddleGray, HDRComponent), "");
         addField("WhitePoint", TypeF32, Offset(mWhite, HDRComponent), "");
         addField("Threshold", TypeF32, Offset(mThreshold, HDRComponent), "");
         addField("AutoExposure", TypeBool, Offset(mAutoExposure, HDRComponent), "");
         addField("Exposure", TypeF32, Offset(mExposure, HDRComponent), "");

      endGroup("HDR");
   }

   void HDRComponent::setOffsets2x2Lum(bgfx::UniformHandle _handle, U32 _width, U32 _height)
   {
	   float offsets[16][4];

	   float du = 1.0f/_width;
	   float dv = 1.0f/_height;

	   uint32_t num = 0;
	   for (uint32_t yy = 0; yy < 3; ++yy)
	   {
		   for (uint32_t xx = 0; xx < 3; ++xx)
		   {
			   offsets[num][0] = (xx - mTexelHalf) * du;
			   offsets[num][1] = (yy - mTexelHalf) * dv;
			   ++num;
		   }
	   }

	   bgfx::setUniform(_handle, offsets, num);
   }

   void HDRComponent::setOffsets4x4Lum(bgfx::UniformHandle _handle, U32 _width, U32 _height)
   {
	   float offsets[16][4];

	   float du = 1.0f/_width;
	   float dv = 1.0f/_height;

	   uint32_t num = 0;
	   for (uint32_t yy = 0; yy < 4; ++yy)
	   {
		   for (uint32_t xx = 0; xx < 4; ++xx)
		   {
			   offsets[num][0] = (xx - 1.0f - mTexelHalf) * du;
			   offsets[num][1] = (yy - 1.0f - mTexelHalf) * dv;
			   ++num;
		   }
	   }

	   bgfx::setUniform(_handle, offsets, num);
   }

   void HDRComponent::process()
   {
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);

      bgfx::setViewTransform(mLuminanceView->id, NULL, proj);
      bgfx::setViewRect(mLuminanceView->id, 0, 0, 128, 128);
      bgfx::setViewFrameBuffer(mLuminanceView->id, mLuminanceBuffer[0]);

      bgfx::setViewTransform(mDownscale_Luminance0View->id, NULL, proj);
      bgfx::setViewRect(mDownscale_Luminance0View->id, 0, 0, 64, 64);
      bgfx::setViewFrameBuffer(mDownscale_Luminance0View->id, mLuminanceBuffer[1]);

      bgfx::setViewTransform(mDownscale_Luminance1View->id, NULL, proj);
      bgfx::setViewRect(mDownscale_Luminance1View->id, 0, 0, 16, 16);
      bgfx::setViewFrameBuffer(mDownscale_Luminance1View->id, mLuminanceBuffer[2]);

      bgfx::setViewTransform(mDownscale_Luminance2View->id, NULL, proj);
      bgfx::setViewRect(mDownscale_Luminance2View->id, 0, 0, 4, 4);
      bgfx::setViewFrameBuffer(mDownscale_Luminance2View->id, mLuminanceBuffer[3]);

      bgfx::setViewTransform(mDownscale_Luminance3View->id, NULL, proj);
      bgfx::setViewRect(mDownscale_Luminance3View->id, 0, 0, 1, 1);
      bgfx::setViewFrameBuffer(mDownscale_Luminance3View->id, mLuminanceBuffer[4]);

      bgfx::setViewTransform(mBrightnessView->id, NULL, proj);
      bgfx::setViewRect(mBrightnessView->id, 0, 0, mCamera->width/2, mCamera->height/2);
      bgfx::setViewFrameBuffer(mBrightnessView->id, mBrightBuffer);

      bgfx::setViewTransform(mBlurYView->id, NULL, proj);
      bgfx::setViewRect(mBlurYView->id, 0, 0, mCamera->width /8, mCamera->height /8);
      bgfx::setViewFrameBuffer(mBlurYView->id, mBlurBuffer);

      bgfx::setViewTransform(mBlurX_TonemapView->id, NULL, proj);
      bgfx::setViewRect(mBlurX_TonemapView->id, 0, 0, mCamera->width, mCamera->height);
      bgfx::setViewFrameBuffer(mBlurX_TonemapView->id, mCamera->getPostTarget());

      // Calculate luminance.
      setOffsets2x2Lum(mOffsetUniform, 128, 128);
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), mCamera->getPostSource());
      bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad(128.0f, 128.0f);
      bgfx::submit(mLuminanceView->id, mLumShader->mProgram);

      // Downscale luminance 0.
      setOffsets4x4Lum(mOffsetUniform, 128, 128);
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), mLuminanceBuffer[0]);
      bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad(64.0f, 64.0f);
      bgfx::submit(mDownscale_Luminance0View->id, mLumAvgShader->mProgram);

      // Downscale luminance 1.
      setOffsets4x4Lum(mOffsetUniform, 64, 64);
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), mLuminanceBuffer[1]);
      bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad(16.0f, 16.0f);
      bgfx::submit(mDownscale_Luminance1View->id, mLumAvgShader->mProgram);

      // Downscale luminance 2.
      setOffsets4x4Lum(mOffsetUniform, 16, 16);
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), mLuminanceBuffer[2]);
      bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad(4.0f, 4.0f);
      bgfx::submit(mDownscale_Luminance2View->id, mLumAvgShader->mProgram);

      // Downscale luminance 3.
      setOffsets4x4Lum(mOffsetUniform, 4, 4);
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), mLuminanceBuffer[3]);
      bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad(1.0f, 1.0f);
      bgfx::submit(mDownscale_Luminance3View->id, mLumAvgShader->mProgram);

      float tonemap[4] = { mMiddleGray, mSquared(mWhite), mThreshold, mExposure };
      bgfx::setUniform(mTonemapUniform, tonemap);

      // Bright pass threshold is tonemap[3].
      setOffsets4x4Lum(mOffsetUniform, mCamera->width/2, mCamera->height/2);
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), mCamera->getPostSource());
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), mLuminanceBuffer[4]);
      bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad((float)mCamera->width /2.0f, (float)mCamera->height /2.0f);
      bgfx::submit(mBrightnessView->id, mBrightShader->mProgram);

      // Blur bright pass vertically.
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), mBrightBuffer);
      bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad((float)mCamera->width /8.0f, (float)mCamera->height /8.0f);
      bgfx::submit(mBlurYView->id, mBlurShader->mProgram);

      // Blur bright pass horizontally, do tonemaping and combine.
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), mCamera->getPostSource());
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), mLuminanceBuffer[4]);
      bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), mBlurBuffer);
      bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad((float)mCamera->width, (float)mCamera->height);
      if ( mAutoExposure )
         bgfx::submit(mBlurX_TonemapView->id, mTonemapAutoShader->mProgram);
      else
         bgfx::submit(mBlurX_TonemapView->id, mTonemapShader->mProgram);
   }
}
