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

#include "ssr.h"
#include "console/consoleInternal.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "3d/scene/core.h"

#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   SSRPostFX::SSRPostFX()
   {
      // Views
      v_SSR = Graphics::getView("SSR", "TransparencyFinal");

      // Shaders
      ssrShader = Graphics::getShader("post/ssr/ssr_vs.sc", "post/ssr/ssr_fs.sc");
   }

   SSRPostFX::~SSRPostFX()
   {

   }

   void SSRPostFX::render()
   {
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);

      bgfx::setViewTransform(v_SSR->id, NULL, proj);
		bgfx::setViewRect(v_SSR->id, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);
      bgfx::setViewFrameBuffer(v_SSR->id, Rendering::getPostTarget());

      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getColorTexture());
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), Rendering::getNormalTexture());
      bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), Rendering::getMatInfoTexture());
      bgfx::setTexture(3, Graphics::Shader::getTextureUniform(3), Rendering::getDepthTexture());
      bgfx::setTexture(4, Graphics::Shader::getTextureUniform(4), Rendering::getPostSource());

		bgfx::setProgram(ssrShader->mProgram);
		bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
		fullScreenQuad((float)Rendering::canvasWidth, (float)Rendering::canvasHeight);
		bgfx::submit(v_SSR->id);
   }
}
