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

#include "ssao.h"
#include "console/consoleInternal.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "3d/scene/core.h"

#include <bgfx/bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Scene
{
   IMPLEMENT_CONOBJECT(SSAO);

   SSAO::SSAO()
   {
      mName       = "SSAO";
      mPriority   = 3500;
      mBias       = 0.005f;
      mIntensity  = 3.0f;
      mScale      = 1.0f;
      mRadius     = 0.03f;

      // Buffers
      mOcclusionBuffer.idx     = bgfx::invalidHandle;
      mOcclusionBlurBuffer.idx = bgfx::invalidHandle;

      // Uniforms
      mSSAOParamsUniform = bgfx::createUniform("u_SSAOParams", bgfx::UniformType::Vec4);

      // Views
      mAccumulateView = Graphics::getView("SSAO_Accumulate", 3500);
      mBlurXView      = Graphics::getView("SSAO_BlurX");
      mBlurYView      = Graphics::getView("SSAO_BlurY");
      mApplyView      = Graphics::getView("SSAO_Apply");

      // Shaders
      mAccumulateShader = Graphics::getDefaultShader("features/ssao/ssao_vs.sc", "features/ssao/ssao_accumulate_fs.sc");
      mBlurXShader      = Graphics::getDefaultShader("features/ssao/ssao_vs.sc", "features/ssao/ssao_blurx_fs.sc");
      mBlurYShader      = Graphics::getDefaultShader("features/ssao/ssao_vs.sc", "features/ssao/ssao_blury_fs.sc");
      mApplyShader      = Graphics::getDefaultShader("features/ssao/ssao_vs.sc", "features/ssao/ssao_apply_fs.sc");

      initBuffers();
   }

   SSAO::~SSAO()
   {
      destroyBuffers();
   }

   void SSAO::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addGroup("SSAO");

         addField("Bias", TypeF32, Offset(mBias, SSAO), "");
         addField("Intensity", TypeF32, Offset(mIntensity, SSAO), "");
         addField("Scale", TypeF32, Offset(mScale, SSAO), "");
         addField("Radius", TypeF32, Offset(mRadius, SSAO), "");

      endGroup("SSAO");
   }

   void SSAO::initBuffers()
   {
      destroyBuffers();

      // Framebuffers
      mOcclusionBuffer = bgfx::createFrameBuffer(bgfx::BackbufferRatio::Equal, bgfx::TextureFormat::RGBA8);
      mOcclusionBlurBuffer = bgfx::createFrameBuffer(bgfx::BackbufferRatio::Equal, bgfx::TextureFormat::RGBA8);
   }

   void SSAO::destroyBuffers()
   {
      if (isValid(mOcclusionBuffer))
         bgfx::destroyFrameBuffer(mOcclusionBuffer);
      if (isValid(mOcclusionBlurBuffer))
         bgfx::destroyFrameBuffer(mOcclusionBlurBuffer);
   }

   void SSAO::render()
   {
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);

      // Params
      float params[4] = { mBias, mIntensity, mScale, mRadius };
      bgfx::setUniform(mSSAOParamsUniform, params);

      // Accumulate
      bgfx::setViewTransform(mAccumulateView->id, NULL, proj);
      bgfx::setViewRect(mAccumulateView->id, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);
      bgfx::setViewFrameBuffer(mAccumulateView->id, mOcclusionBuffer);
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getDepthTexture());
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), Rendering::getNormalTexture());
      bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), Graphics::noiseTexture);
      bgfx::setState(BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad((float)Rendering::canvasWidth, (float)Rendering::canvasHeight);
      bgfx::submit(mAccumulateView->id, mAccumulateShader->mProgram);

      // Blur X
      bgfx::setViewTransform(mBlurXView->id, NULL, proj);
      bgfx::setViewRect(mBlurXView->id, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);
      bgfx::setViewFrameBuffer(mBlurXView->id, mOcclusionBlurBuffer);
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), mOcclusionBuffer);
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), Rendering::getNormalTexture());
      bgfx::setState(BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad((float)Rendering::canvasWidth, (float)Rendering::canvasHeight);
      bgfx::submit(mBlurXView->id, mBlurXShader->mProgram);

      // Blur Y
      bgfx::setViewTransform(mBlurYView->id, NULL, proj);
      bgfx::setViewRect(mBlurYView->id, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);
      bgfx::setViewFrameBuffer(mBlurYView->id, mOcclusionBuffer);
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), mOcclusionBlurBuffer);
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), Rendering::getNormalTexture());
      bgfx::setState(BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad((float)Rendering::canvasWidth, (float)Rendering::canvasHeight);
      bgfx::submit(mBlurYView->id, mBlurYShader->mProgram);

      // Apply
      bgfx::setViewTransform(mApplyView->id, NULL, proj);
      bgfx::setViewRect(mApplyView->id, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);
      bgfx::setViewFrameBuffer(mApplyView->id, Rendering::getPostTarget());
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getPostSource());
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), mOcclusionBuffer);
      bgfx::setState(BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad((float)Rendering::canvasWidth, (float)Rendering::canvasHeight);
      bgfx::submit(mApplyView->id, mApplyShader->mProgram);
   }

   void SSAO::resize()
   {

   }
}
