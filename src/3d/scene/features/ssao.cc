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

#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Scene
{
   IMPLEMENT_CONOBJECT(SSAO);

   SSAO::SSAO()
   {
      mPriority = 3500;

      // Views
      v_SSAO_Accumulate = Graphics::getView("SSAO_Accumulate", 3500);
      v_SSAO_BlurX      = Graphics::getView("SSAO_BlurX");
      v_SSAO_BlurY      = Graphics::getView("SSAO_BlurY");
      v_SSAO_Apply      = Graphics::getView("SSAO_Apply");

      // Shaders
      ssaoAccumulateShader = Graphics::getShader("features/ssao/ssao_vs.sc", "features/ssao/ssao_accumulate_fs.sc");
      ssaoBlurXShader      = Graphics::getShader("features/ssao/ssao_vs.sc", "features/ssao/ssao_blurx_fs.sc");
      ssaoBlurYShader      = Graphics::getShader("features/ssao/ssao_vs.sc", "features/ssao/ssao_blury_fs.sc");
      ssaoApplyShader      = Graphics::getShader("features/ssao/ssao_vs.sc", "features/ssao/ssao_apply_fs.sc");

      // Framebuffers
      occlusionBuffer     = bgfx::createFrameBuffer(Rendering::canvasWidth, Rendering::canvasHeight, bgfx::TextureFormat::RGBA8);
      occlusionBlurBuffer = bgfx::createFrameBuffer(Rendering::canvasWidth, Rendering::canvasHeight, bgfx::TextureFormat::RGBA8);
   }

   SSAO::~SSAO()
   {

   }

   void SSAO::render()
   {
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);

      // Accumulate
      bgfx::setViewTransform(v_SSAO_Accumulate->id, NULL, proj);
      bgfx::setViewRect(v_SSAO_Accumulate->id, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);
      bgfx::setViewFrameBuffer(v_SSAO_Accumulate->id, occlusionBuffer);
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getDepthTexture());
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), Rendering::getNormalTexture());
      bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), Graphics::noiseTexture);
      bgfx::setState(BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad((float)Rendering::canvasWidth, (float)Rendering::canvasHeight);
      bgfx::submit(v_SSAO_Accumulate->id, ssaoAccumulateShader->mProgram);

      // Blur X
      bgfx::setViewTransform(v_SSAO_BlurX->id, NULL, proj);
      bgfx::setViewRect(v_SSAO_BlurX->id, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);
      bgfx::setViewFrameBuffer(v_SSAO_BlurX->id, occlusionBlurBuffer);
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), occlusionBuffer);
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), Rendering::getNormalTexture());
      bgfx::setState(BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad((float)Rendering::canvasWidth, (float)Rendering::canvasHeight);
      bgfx::submit(v_SSAO_BlurX->id, ssaoBlurXShader->mProgram);

      // Blur Y
      bgfx::setViewTransform(v_SSAO_BlurY->id, NULL, proj);
      bgfx::setViewRect(v_SSAO_BlurY->id, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);
      bgfx::setViewFrameBuffer(v_SSAO_BlurY->id, occlusionBuffer);
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), occlusionBlurBuffer);
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), Rendering::getNormalTexture());
      bgfx::setState(BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad((float)Rendering::canvasWidth, (float)Rendering::canvasHeight);
      bgfx::submit(v_SSAO_BlurY->id, ssaoBlurYShader->mProgram);

      // Apply
      bgfx::setViewTransform(v_SSAO_Apply->id, NULL, proj);
      bgfx::setViewRect(v_SSAO_Apply->id, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);
      bgfx::setViewFrameBuffer(v_SSAO_Apply->id, Rendering::getPostTarget());
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getPostSource());
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), occlusionBuffer);
      bgfx::setState(BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad((float)Rendering::canvasWidth, (float)Rendering::canvasHeight);
      bgfx::submit(v_SSAO_Apply->id, ssaoApplyShader->mProgram);
   }
}
