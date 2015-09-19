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

#include "dlaa.h"
#include "console/consoleInternal.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "3d/scene/core.h"
#include "3d/rendering/common.h"

#include <bgfx/bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Scene
{
   IMPLEMENT_CONOBJECT(DLAA);

   DLAA::DLAA()
   {
      mName = "DLAA";
      mPriority = 5000;

      // Shaders
      mEdgeShader    = Graphics::getShader("features/dlaa/dlaa_edge_vs.sc", "features/dlaa/dlaa_edge_fs.sc");
      mFinalShader   = Graphics::getShader("features/dlaa/dlaa_final_vs.sc", "features/dlaa/dlaa_final_fs.sc");

      // Views
      mEdgeView   = Graphics::getView("DLAA_Edge", 4999);
      mFinalView  = Rendering::overridePostFinish();
   }

   DLAA::~DLAA()
   {

   }

   void DLAA::render()
   {
      // This projection matrix is used because its a full screen quad.
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);

      // DLAA Edge:
      bgfx::setViewClear(mEdgeView->id
         , BGFX_CLEAR_COLOR
         , 0x00000000
         , 1.0f
         , 0
         );
      bgfx::setViewTransform(mEdgeView->id, NULL, proj);
      bgfx::setViewRect(mEdgeView->id, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);
      bgfx::setViewFrameBuffer(mEdgeView->id, Rendering::getPostTarget());
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getPostSource());
      bgfx::setState(0
         | BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
         );
      fullScreenQuad((F32)Rendering::canvasWidth, (F32)Rendering::canvasHeight);
      bgfx::submit(mEdgeView->id, mEdgeShader->mProgram);

      Rendering::flipPostBuffers();

      // DLAA Final:
      bgfx::setViewTransform(mFinalView->id, NULL, proj);
      bgfx::setViewRect(mFinalView->id, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getPostSource());
      bgfx::setState(0
         | BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
         //| BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
         );
      fullScreenQuad((F32)Rendering::canvasWidth, (F32)Rendering::canvasHeight);
      bgfx::submit(mFinalView->id, mFinalShader->mProgram);
   }
}
