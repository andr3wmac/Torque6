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

#include "renderCamera.h"
#include "console/consoleInternal.h"
#include "graphics/shaders.h"
#include "graphics/dgl.h"
#include "scene/scene.h"

#include <bgfx/bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   RenderCamera::RenderCamera()
   {
      mInitialized = false;
      mBeginEnabled = true;
      mFinishEnabled = true;
      mPostBuffers[0].idx = bgfx::invalidHandle;
      mPostBuffers[1].idx = bgfx::invalidHandle;
      mPostBufferIdx = 0;

      // Shaders
      mBeginShader = NULL;
      mFinishShader = NULL;

      // Views
      mBeginView = NULL;
      mFinishView = NULL;

      nearPlane         = 0.1f;
      farPlane          = 200.0f;
      projectionWidth   = 0.0f;
      projectionHeight  = 0.0f;

      mDeferredShading  = new DeferredShading(this);
      mTransparency     = new Transparency(this);
      mCameraPosUniform = Graphics::Shader::getUniformVec4("u_camPos");
   }

   RenderCamera::~RenderCamera()
   {
      if (mDeferredShading != NULL)
         SAFE_DELETE(mDeferredShading);
   }

   void RenderCamera::render()
   {
      Vector<Rendering::RenderHook*> renderHookList = *Rendering::getRenderHookList();
      bgfx::setUniform(mCameraPosUniform, Point4F(position.x, position.y, position.z, 0.0f));

      // Filter

      // PreRender
      for (S32 n = 0; n < renderHookList.size(); ++n)
         renderHookList[n]->preRender(this);
      mDeferredShading->preRender();

      // Render
      for (S32 n = 0; n < renderHookList.size(); ++n)
         renderHookList[n]->render(this);

      // Render Opaque Surfaces
      mDeferredShading->render();
      
      // Render Transparency Surfaces
      mTransparency->render(getPostSource());

      // Post Render
      for (S32 n = 0; n < renderHookList.size(); ++n)
         renderHookList[n]->postRender(this);
      mDeferredShading->postRender();

      // Post Processing
      postProcess();

      // Present
   }

   // ----------------------------------------
   //   Render Post Process
   // ----------------------------------------

   int QSORT_CALLBACK compareRenderPostProcessPriority(const void * a, const void * b)
   {
      return ((*(Rendering::RenderPostProcess**)a)->mPriority - (*(Rendering::RenderPostProcess**)b)->mPriority);
   }

   void RenderCamera::addRenderPostProcess(RenderPostProcess* postProcess)
   {
      if (!mInitialized)
         initBuffers();

      renderPostProcessList.push_back(postProcess);
      qsort((void *)renderPostProcessList.address(), renderPostProcessList.size(), sizeof(Rendering::RenderPostProcess*), compareRenderPostProcessPriority);

      postProcess->mCamera = this;
      postProcess->onAddToCamera();
   }

   bool RenderCamera::removeRenderPostProcess(RenderPostProcess* postProcess)
   {
      for (Vector< RenderPostProcess* >::iterator itr = renderPostProcessList.begin(); itr != renderPostProcessList.end(); ++itr)
      {
         if ((*itr) == postProcess)
         {
            renderPostProcessList.erase(itr);
            break;
         }
      }

      postProcess->onRemoveFromCamera();
      postProcess->mCamera = NULL;
      return false;
   }

   Vector<RenderPostProcess*>* RenderCamera::getRenderPostProcessList()
   {
      return &renderPostProcessList;
   }

   // ----------------------------------------
   //   Post Processing
   // ----------------------------------------

   void RenderCamera::initBuffers()
   {
      destroyBuffers();

      // Shaders
      mBeginShader = Graphics::getDefaultShader("rendering/begin_vs.tsh", "rendering/begin_fs.tsh");
      mFinishShader = Graphics::getDefaultShader("rendering/finish_vs.tsh", "rendering/finish_fs.tsh");

      // Views
      mBeginView = Graphics::getView("Post_Begin", 4000);
      mFinishView = Graphics::getView("Post_Finish", 5000);

      // Create two buffers for flip-flopping.
      const U32 samplerFlags = 0
         | BGFX_TEXTURE_RT
         | BGFX_TEXTURE_MIN_POINT
         | BGFX_TEXTURE_MAG_POINT
         | BGFX_TEXTURE_MIP_POINT
         | BGFX_TEXTURE_U_CLAMP
         | BGFX_TEXTURE_V_CLAMP;

      mPostBuffers[0] = bgfx::createFrameBuffer(bgfx::BackbufferRatio::Equal, bgfx::TextureFormat::BGRA8, samplerFlags);
      mPostBuffers[1] = bgfx::createFrameBuffer(bgfx::BackbufferRatio::Equal, bgfx::TextureFormat::BGRA8, samplerFlags);
      mPostBufferIdx = 0;
      mInitialized = true;
   }

   void RenderCamera::destroyBuffers()
   {
      if (bgfx::isValid(mPostBuffers[0]))
         bgfx::destroyFrameBuffer(mPostBuffers[0]);
      if (bgfx::isValid(mPostBuffers[1]))
         bgfx::destroyFrameBuffer(mPostBuffers[1]);
   }

   Graphics::ViewTableEntry* RenderCamera::overrideBegin()
   {
      mBeginEnabled = false;
      return mBeginView;
   }

   Graphics::ViewTableEntry* RenderCamera::overrideFinish()
   {
      mFinishEnabled = false;
      return mFinishView;
   }

   void RenderCamera::freeBegin()
   {
      mBeginEnabled = true;
   }

   void RenderCamera::freeFinish()
   {
      mFinishEnabled = true;
   }

   bgfx::FrameBufferHandle RenderCamera::getPostSource()
   {
      return mPostBuffers[mPostBufferIdx];
   }

   bgfx::FrameBufferHandle RenderCamera::getPostTarget()
   {
      U32 targetIdx = mPostBufferIdx == 0 ? 1 : 0;
      return mPostBuffers[targetIdx];
   }

   void RenderCamera::flipPostBuffers()
   {
      mPostBufferIdx = mPostBufferIdx == 0 ? 1 : 0;
   }

   void RenderCamera::postProcess()
   {
      if (!mInitialized)
         initBuffers();

      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);

      // Begin
      if (mBeginEnabled)
      {
         bgfx::setViewFrameBuffer(mBeginView->id, getPostTarget());
         bgfx::setViewTransform(mBeginView->id, NULL, proj);
         bgfx::setViewRect(mBeginView->id, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);
         bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), getPostSource());
         bgfx::setState(0
            | BGFX_STATE_RGB_WRITE
            | BGFX_STATE_ALPHA_WRITE
            );
         fullScreenQuad((F32)canvasWidth, (F32)canvasHeight);
         bgfx::submit(mBeginView->id, mBeginShader->mProgram);
         flipPostBuffers();
      }

      // Process each RenderPostProcess
      for (S32 n = 0; n < renderPostProcessList.size(); ++n)
      {
         renderPostProcessList[n]->process();
         flipPostBuffers();
      }

      // Finish
      if (mFinishEnabled)
      {
         bgfx::setViewTransform(mFinishView->id, NULL, proj);
         bgfx::setViewRect(mFinishView->id, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);
         bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), getPostSource());
         bgfx::setState(0
            | BGFX_STATE_RGB_WRITE
            | BGFX_STATE_ALPHA_WRITE
            );
         fullScreenQuad((F32)canvasWidth, (F32)canvasHeight);
         bgfx::submit(mFinishView->id, mFinishShader->mProgram);
      }
   }

   bgfx::FrameBufferHandle RenderCamera::getBackBuffer()
   {
      return mDeferredShading->getBackBuffer();
   }

   bgfx::TextureHandle RenderCamera::getColorTexture()
   {
      return mDeferredShading->getColorTexture();
   }

   bgfx::TextureHandle RenderCamera::getDepthTexture()
   {
      return mDeferredShading->getDepthTexture();
   }

   bgfx::TextureHandle RenderCamera::getDepthTextureRead()
   {
      return mDeferredShading->getDepthTextureRead();
   }

   bgfx::TextureHandle RenderCamera::getNormalTexture()
   {
      return mDeferredShading->getNormalTexture();
   }

   bgfx::TextureHandle RenderCamera::getMatInfoTexture()
   {
      return mDeferredShading->getMatInfoTexture();
   }
}
