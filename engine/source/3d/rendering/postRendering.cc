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

#include "postRendering.h"
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
   PostRendering* _postRenderingInst = NULL;

   void postInit()
   {
      if ( _postRenderingInst != NULL ) return;
      _postRenderingInst = new PostRendering();
   }

   void postDestroy()
   {
      SAFE_DELETE(_postRenderingInst);
   }

   void addPostFX(PostFX* fx)
   {
      _postRenderingInst->postFXList.push_back(fx);
   }

   PostRendering::PostRendering()
   {
      finalShader = Graphics::getShader("post/final_vs.sc", "post/final_fs.sc");
      ssrShader = Graphics::getShader("post/ssr_vs.sc", "post/ssr_fs.sc");
      setRendering(true);
   }

   PostRendering::~PostRendering()
   {
      for( S32 n = 0; n < postFXList.size(); ++n)
      {
         PostFX* fx = postFXList[n];
         SAFE_DELETE(fx);
      }
      postFXList.clear();
   }

   void PostRendering::preRender()
   {
      // Unused.
   }

   void PostRendering::render()
   {
      // Unused.
   }

   void PostRendering::postRender()
   {
      for( S32 n = 0; n < postFXList.size(); ++n)
      {
         PostFX* fx = postFXList[n];
         fx->render();
      }

      // This projection matrix is used because its a full screen quad.
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
      bgfx::setViewTransform(Graphics::Final, NULL, proj);
      bgfx::setViewRect(Graphics::Final, 0, 0, canvasWidth, canvasHeight);

      // Flip to screen.
      /*bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getBackBufferTexture(), 0);
      bgfx::setProgram(finalShader->mProgram);

      bgfx::setState(0
         | BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
         | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
         );

      fullScreenQuad((F32)canvasWidth, (F32)canvasHeight);
      bgfx::submit(Graphics::Final);*/

      // SSR Test
      /*bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getDepthTexture(), 0);
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), Rendering::getBackBufferTexture(), 0);
      bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), Rendering::getNormalTexture(), 0);
      bgfx::setProgram(ssrShader->mProgram);

      bgfx::setState(0
         | BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
         | BGFX_STATE_BLEND_ADD
         );

      fullScreenQuad((F32)canvasWidth, (F32)canvasHeight);
      bgfx::submit(Graphics::Final);
      */
   }
}
