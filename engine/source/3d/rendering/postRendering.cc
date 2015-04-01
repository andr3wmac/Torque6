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

   PostRendering::PostRendering()
   {
      finalShader = Graphics::getShader("final_vs.sc", "final_fs.sc");
      setRendering(true);
   }

   PostRendering::~PostRendering()
   {

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
      // This projection matrix is used because its a full screen quad.
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
      bgfx::setViewTransform(Graphics::ViewTable::Final, NULL, proj);
      bgfx::setViewRect(Graphics::ViewTable::Final, 0, 0, canvasWidth, canvasHeight);

      // Flip to screen.
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getFinalTexture(), 0);
      bgfx::setProgram(finalShader->mProgram);

      bgfx::setState(0
         | BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
         | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
         );

      fullScreenQuad(canvasWidth, canvasHeight);
      bgfx::submit(Graphics::ViewTable::Final);
   }
}