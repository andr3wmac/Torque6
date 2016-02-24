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
#include "rendering/rendering.h"
#include "scene/scene.h"
#include "scene/components/cameraComponent.h"

#include <bgfx/bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Scene
{
   IMPLEMENT_CONOBJECT(DLAA);

   DLAA::DLAA()
   {
      mPriority      = 5000;
      mEdgeShader    = NULL;
      mFinalShader   = NULL;
      mEdgeView      = NULL;
      mFinalView     = NULL;
   }

   DLAA::~DLAA()
   {

   }

   void DLAA::onAddToScene()
   {
      CameraComponent* camera = mOwnerObject->findComponentByType<CameraComponent>();
      if (!camera)
         return;

      mCamera = camera->getRenderCamera();
      if (mCamera)
         mCamera->addRenderPostProcess(this);
   }

   void DLAA::onRemoveFromScene()
   {
      if (mCamera)
         mCamera->removeRenderPostProcess(this);
   }

   void DLAA::onAddToCamera()
   {
      // Shaders
      mEdgeShader = Graphics::getDefaultShader("components/dlaa/dlaa_edge_vs.tsh", "components/dlaa/dlaa_edge_fs.tsh");
      mFinalShader = Graphics::getDefaultShader("components/dlaa/dlaa_final_vs.tsh", "components/dlaa/dlaa_final_fs.tsh");

      // Views
      mEdgeView = Graphics::getView("DLAA_Edge", 4999, mCamera);
      mFinalView = mCamera->overrideFinish();
   }

   void DLAA::onRemoveFromCamera()
   {
      // Delete Views
      Graphics::deleteView(mEdgeView);
      mCamera->freeFinish();
   }

   void DLAA::process()
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
      bgfx::setViewFrameBuffer(mEdgeView->id, mCamera->getPostTarget());
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), mCamera->getPostSource());
      bgfx::setState(0
         | BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
         );
      fullScreenQuad((F32)Rendering::canvasWidth, (F32)Rendering::canvasHeight);
      bgfx::submit(mEdgeView->id, mEdgeShader->mProgram);

      mCamera->flipPostBuffers();

      // DLAA Final:
      bgfx::setViewTransform(mFinalView->id, NULL, proj);
      bgfx::setViewRect(mFinalView->id, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), mCamera->getPostSource());
      bgfx::setState(0
         | BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
         );
      fullScreenQuad((F32)Rendering::canvasWidth, (F32)Rendering::canvasHeight);
      bgfx::submit(mFinalView->id, mFinalShader->mProgram);
   }
}
