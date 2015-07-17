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
#include "graphics/core.h"
#include "3d/scene/core.h"

#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   PostRendering* _postRenderingInst = NULL;

   U32 _postBufferIdx = 0;
   bgfx::FrameBufferHandle _postBuffers[2] = { BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE };

   void postInit()
   {
      if ( _postRenderingInst != NULL ) return;
      _postRenderingInst = new PostRendering();

      // Create two buffers for flip-flopping.
      const U32 samplerFlags = 0
            | BGFX_TEXTURE_RT
            | BGFX_TEXTURE_MIN_POINT
            | BGFX_TEXTURE_MAG_POINT
            | BGFX_TEXTURE_MIP_POINT
            | BGFX_TEXTURE_U_CLAMP
            | BGFX_TEXTURE_V_CLAMP;

      _postBuffers[0] = bgfx::createFrameBuffer(canvasWidth, canvasHeight, bgfx::TextureFormat::BGRA8, samplerFlags);
      _postBuffers[1] = bgfx::createFrameBuffer(canvasWidth, canvasHeight, bgfx::TextureFormat::BGRA8, samplerFlags);
   }

   void postDestroy()
   {
      SAFE_DELETE(_postRenderingInst);

      if ( bgfx::isValid(_postBuffers[0]) )
         bgfx::destroyFrameBuffer(_postBuffers[0]);
      if ( bgfx::isValid(_postBuffers[1]) )
         bgfx::destroyFrameBuffer(_postBuffers[1]);
   }

   void addPostFX(PostFX* fx)
   {
      _postRenderingInst->postFXList.push_back(fx);
   }

   bgfx::FrameBufferHandle getPostSource()
   {
      return _postBuffers[_postBufferIdx];
   }

   bgfx::FrameBufferHandle getPostTarget()
   {
      U32 targetIdx = _postBufferIdx == 0 ? 1 : 0;
      return _postBuffers[targetIdx];
   }

   void flipPostBuffers()
   {
      _postBufferIdx = _postBufferIdx == 0 ? 1 : 0;
   }

   PostRendering::PostRendering()
   {
      finalShader = Graphics::getShader("post/final_vs.sc", "post/final_fs.sc");
      finalFXAAShader = Graphics::getShader("post/final_vs.sc", "post/final_fxaa_fs.sc");

      // Get Views
      v_Final = Graphics::getView("Final", "TorqueGUITop", true);

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
         flipPostBuffers();
      }

      // This projection matrix is used because its a full screen quad.
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
      bgfx::setViewTransform(v_Final->id, NULL, proj);
      bgfx::setViewRect(v_Final->id, 0, 0, canvasWidth, canvasHeight);

      // Copy the last Post target into the actual final buffer.
      // Note: we use getPostSource() because the buffers are flipped after every postFX,
      //       also if no PostFX are loaded the previous stage will be in source.
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), getPostSource());
      bgfx::setProgram(finalFXAAShader->mProgram);

      bgfx::setState(0
         | BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
         | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
         );

      fullScreenQuad((F32)canvasWidth, (F32)canvasHeight);
      bgfx::submit(v_Final->id);
   }
}
