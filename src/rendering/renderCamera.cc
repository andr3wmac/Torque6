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
#include "deferredShading/deferredShading.h"
#include "forwardShading/forwardShading.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/utilities.h"
#include "lighting/lighting.h"
#include "scene/scene.h"

#include <bgfx/bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   RenderCamera::RenderCamera(const char* renderPathType)
   {
      refCount  = 0;
      mName     = StringTable->EmptyString;
      mPriority = 0;

      mInitialized         = false;
      mBeginEnabled        = true;
      mFinishEnabled       = true;
      mFinishBuffer.idx    = bgfx::invalidHandle;
      mPostBuffers[0].idx  = bgfx::invalidHandle;
      mPostBuffers[1].idx  = bgfx::invalidHandle;
      mPostTextures[0].idx = bgfx::invalidHandle;
      mPostTextures[1].idx = bgfx::invalidHandle;
      mPostBufferIdx       = 0;
      mRenderTextureName   = NULL;

      // Shaders
      mBeginShader   = NULL;
      mFinishShader  = NULL;

      // Views
      mBeginView  = NULL;
      mFinishView = NULL;
      mDebugView  = NULL;

      matchWindowSize   = true;
      width             = Rendering::windowWidth;
      height            = Rendering::windowHeight;
      nearPlane         = 0.1f;
      farPlane          = 200.0f;
      projectionWidth   = 0.0f;
      projectionHeight  = 0.0f;
      fov               = 60.f;

      mRenderPath    = Rendering::getRenderPathInstance(renderPathType, this);
      mTransparency  = new Transparency(this);

      // Common Uniforms
      mCommonUniforms.camPos                 = Graphics::Shader::getUniformVec4("u_camPos");
      mCommonUniforms.sceneViewMat           = Graphics::Shader::getUniformMat4("u_sceneViewMat", 1);
      mCommonUniforms.sceneInvViewMat        = Graphics::Shader::getUniformMat4("u_sceneInvViewMat", 1);
      mCommonUniforms.sceneProjMat           = Graphics::Shader::getUniformMat4("u_sceneProjMat", 1);
      mCommonUniforms.sceneInvProjMat        = Graphics::Shader::getUniformMat4("u_sceneInvProjMat", 1);
      mCommonUniforms.sceneViewProjMat       = Graphics::Shader::getUniformMat4("u_sceneViewProjMat", 1);
      mCommonUniforms.sceneInvViewProjMat    = Graphics::Shader::getUniformMat4("u_sceneInvViewProjMat", 1);
      mCommonUniforms.sceneDirLightDirection = Graphics::Shader::getUniformVec4("u_sceneDirLightDirection", 1);
      mCommonUniforms.sceneDirLightColor     = Graphics::Shader::getUniformVec4("u_sceneDirLightColor", 1);
      mCommonUniforms.time                   = Graphics::Shader::getUniform("u_time", bgfx::UniformType::Vec4);
   }

   RenderCamera::~RenderCamera()
   {
      destroyBuffers();

      if (mRenderPath != NULL)
         SAFE_DELETE(mRenderPath);

      if (mTransparency != NULL)
         SAFE_DELETE(mTransparency);
   }

   StringTableEntry RenderCamera::getName()
   {
      return mName;
   }

   void RenderCamera::setName(StringTableEntry name)
   {
      mName = name;
   }

   StringTableEntry RenderCamera::getRenderTextureName()
   {
      return mRenderTextureName;
   }

   void RenderCamera::setRenderTextureName(StringTableEntry name)
   {
      if (mRenderTextureName == name)
         return;

      mRenderTextureName = name;
      if (mRenderTextureName != NULL && mRenderTextureName != StringTable->EmptyString)
      {
         RenderTexture* rt = Rendering::createRenderTexture(mRenderTextureName, width, height);
         mFinishBuffer = bgfx::createFrameBuffer(1, &rt->handle, false);
      }
      else {
         mFinishBuffer.idx = bgfx::invalidHandle;
      }
   }

   void RenderCamera::setRenderFrameBuffer(bgfx::FrameBufferHandle fbh)
   {
      mFinishBuffer = fbh;
   }

   S16 RenderCamera::getRenderPriority()
   {
      return mPriority;
   }

   void RenderCamera::setRenderPriority(S16 priority)
   {
      mPriority = priority;
   }

   void RenderCamera::setCommonUniforms()
   {
      bgfx::setUniform(mCommonUniforms.camPos, Point4F(position.x, position.y, position.z, 0.0f));

      F32 time = (F32)Sim::getCurrentTime();
      bgfx::setUniform(mCommonUniforms.time, Point4F(time, 0.0f, 0.0f, 0.0f));

      bgfx::setUniform(mCommonUniforms.sceneViewMat, viewMatrix, 1);
      bgfx::setUniform(mCommonUniforms.sceneProjMat, projectionMatrix, 1);

      float invViewMat[16];
      bx::mtxInverse(invViewMat, viewMatrix);
      bgfx::setUniform(mCommonUniforms.sceneInvViewMat, invViewMat, 1);

      float invProjMat[16];
      bx::mtxInverse(invProjMat, projectionMatrix);
      bgfx::setUniform(mCommonUniforms.sceneInvProjMat, invProjMat, 1);

      float viewProjMtx[16];
      bx::mtxMul(viewProjMtx, viewMatrix, projectionMatrix);
      bgfx::setUniform(mCommonUniforms.sceneViewProjMat, viewProjMtx, 1);

      float invViewProjMtx[16];
      bx::mtxInverse(invViewProjMtx, viewProjMtx);
      bgfx::setUniform(mCommonUniforms.sceneInvViewProjMat, invViewProjMtx, 1);

      // Directional Light
      bgfx::setUniform(mCommonUniforms.sceneDirLightDirection,
         Point4F(Lighting::directionalLight.direction.x, Lighting::directionalLight.direction.y, Lighting::directionalLight.direction.z, 0.0f));
      bgfx::setUniform(mCommonUniforms.sceneDirLightColor, &Lighting::directionalLight.color.red);

      // Touch bottom view to make sure all uniforms are set for the whole camera render.
      Graphics::ViewTableEntry* bottomView = Graphics::getCameraStart(this);
      if (bottomView != NULL)
         bgfx::touch(bottomView->id);
      else
         bgfx::touch(0);
   }

   void RenderCamera::render()
   {
      // We need the render hook list to trigger them at multiple steps.
      Vector<Rendering::RenderHook*> renderHookList = *Rendering::getRenderHookList();

      bool windowSizeChanged = false;
      if (matchWindowSize)
      {
         windowSizeChanged = (width != Rendering::windowWidth || height != Rendering::windowHeight || Rendering::windowSizeChanged);
         if (windowSizeChanged)
         {
            width    = Rendering::windowWidth;
            height   = Rendering::windowHeight;
         }
      }

      // Projection Matrix Setup (NOTE: This doesn't need to be per-frame)
      F32 camAspect = F32(width) / F32(height);
      projectionHeight = 1.0f / mTan(bx::toRad(fov) * 0.5f);
      projectionWidth = projectionHeight * camAspect;
      bx::mtxProj(projectionMatrix, fov, camAspect, nearPlane, farPlane);

      if (matchWindowSize && windowSizeChanged)
      {
         initBuffers();
         mRenderPath->resize();
         mTransparency->resize();
      }

      // Filter
      {
         Rendering::RenderData* renderData = Rendering::getRenderDataList();

         for (U32 n = 0; n < Rendering::getRenderDataCount(); ++n, ++renderData)
            renderData->flags &= ~Rendering::RenderData::Filtered;

         for (S32 n = 0; n < mRenderFilterList.size(); ++n)
            mRenderFilterList[n]->execute();
      }

      // PreRender
      {
         for (S32 n = 0; n < renderHookList.size(); ++n)
            renderHookList[n]->preRender(this);

         mRenderPath->preRender();
      }

      // Render
      {
         for (S32 n = 0; n < renderHookList.size(); ++n)
            renderHookList[n]->render(this);

         // Render Opaque Surfaces
         mRenderPath->render();

         // Render Transparency Surfaces
         mTransparency->render(getPostSource());
      }

      // Post Render
      {
         for (S32 n = 0; n < renderHookList.size(); ++n)
            renderHookList[n]->postRender(this);

         mRenderPath->postRender();

         // Post Processing
         postProcess();
      }

      // Present
      setCommonUniforms();
   }

   // ----------------------------------------
   //   Render Filters
   // ----------------------------------------

   int QSORT_CALLBACK compareRenderFilterPriority(const void * a, const void * b)
   {
      return ((*(Rendering::RenderFilter**)a)->mPriority - (*(Rendering::RenderFilter**)b)->mPriority);
   }

   void RenderCamera::addRenderFilter(RenderFilter* filter)
   {
      mRenderFilterList.push_back(filter);
      qsort((void *)mRenderFilterList.address(), mRenderFilterList.size(), sizeof(Rendering::RenderFilter*), compareRenderFilterPriority);

      filter->mCamera = this;
      filter->onAddToCamera();
   }

   bool RenderCamera::removeRenderFilter(RenderFilter* filter)
   {
      for (Vector< RenderFilter* >::iterator itr = mRenderFilterList.begin(); itr != mRenderFilterList.end(); ++itr)
      {
         if ((*itr) == filter)
         {
            mRenderFilterList.erase(itr);
            break;
         }
      }

      filter->onRemoveFromCamera();
      filter->mCamera = NULL;
      return false;
   }

   Vector<RenderFilter*>* RenderCamera::getRenderFilterList()
   {
      return &mRenderFilterList;
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

      mRenderPostProcessList.push_back(postProcess);
      qsort((void *)mRenderPostProcessList.address(), mRenderPostProcessList.size(), sizeof(Rendering::RenderPostProcess*), compareRenderPostProcessPriority);

      postProcess->mCamera = this;
      postProcess->onAddToCamera();
   }

   bool RenderCamera::removeRenderPostProcess(RenderPostProcess* postProcess)
   {
      for (Vector< RenderPostProcess* >::iterator itr = mRenderPostProcessList.begin(); itr != mRenderPostProcessList.end(); ++itr)
      {
         if ((*itr) == postProcess)
         {
            mRenderPostProcessList.erase(itr);
            break;
         }
      }

      postProcess->onRemoveFromCamera();
      postProcess->mCamera = NULL;
      return false;
   }

   Vector<RenderPostProcess*>* RenderCamera::getRenderPostProcessList()
   {
      return &mRenderPostProcessList;
   }

   // ----------------------------------------
   //   Post Processing
   // ----------------------------------------

   void RenderCamera::initBuffers()
   {
      destroyBuffers();

      // Shaders
      mBeginShader  = Graphics::getDefaultShader("rendering/begin_vs.tsh", "rendering/begin_fs.tsh");
      mFinishShader = Graphics::getDefaultShader("rendering/finish_vs.tsh", "rendering/finish_fs.tsh");

      // Views
      mBeginView  = Graphics::getView("Post_Begin", 4000, this);
      mDebugView  = Graphics::getView("Post_Debug", 4900, this);
      mFinishView = Graphics::getView("Post_Finish", 5000, this);

      // Create two buffers for flip-flopping.
      const U32 samplerFlags = 0
         | BGFX_TEXTURE_RT
         | BGFX_TEXTURE_MIN_POINT
         | BGFX_TEXTURE_MAG_POINT
         | BGFX_TEXTURE_MIP_POINT
         | BGFX_TEXTURE_U_CLAMP
         | BGFX_TEXTURE_V_CLAMP;

      mPostTextures[0]  = bgfx::createTexture2D(width, height, 1, bgfx::TextureFormat::BGRA8, samplerFlags);
      mPostBuffers[0]   = bgfx::createFrameBuffer(1, &mPostTextures[0]);
      mPostTextures[1]  = bgfx::createTexture2D(width, height, 1, bgfx::TextureFormat::BGRA8, samplerFlags);
      mPostBuffers[1]   = bgfx::createFrameBuffer(1, &mPostTextures[1]);
      mPostBufferIdx    = 0;

      mInitialized    = true;
   }

   void RenderCamera::destroyBuffers()
   {
      if (bgfx::isValid(mPostBuffers[0]))
         bgfx::destroyFrameBuffer(mPostBuffers[0]);
      if (bgfx::isValid(mPostBuffers[1]))
         bgfx::destroyFrameBuffer(mPostBuffers[1]);
      if (bgfx::isValid(mPostTextures[0]))
         bgfx::destroyTexture(mPostTextures[0]);
      if (bgfx::isValid(mPostTextures[1]))
         bgfx::destroyTexture(mPostTextures[1]);
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
         bgfx::setViewRect(mBeginView->id, 0, 0, width, height);
         bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), getPostSource());
         bgfx::setState(0
            | BGFX_STATE_RGB_WRITE
            | BGFX_STATE_ALPHA_WRITE
            );
         fullScreenQuad((F32)width, (F32)height);
         bgfx::submit(mBeginView->id, mBeginShader->mProgram);
         flipPostBuffers();
      }

      // Process each RenderPostProcess
      for (S32 n = 0; n < mRenderPostProcessList.size(); ++n)
      {
         mRenderPostProcessList[n]->process();
         flipPostBuffers();
      }

      // Render debug drawings
      if ( Debug::isDebugEnabled() )
      {
         /*bgfx::TextureHandle debugBufferTextures[2] = { mPostTextures[mPostBufferIdx], mRenderPath->getDepthTexture() };
         bgfx::FrameBufferHandle debugBuffer = bgfx::createFrameBuffer(BX_COUNTOF(debugBufferTextures), debugBufferTextures);

         bgfx::setViewRect(mDebugView->id, 0, 0, width, height);
         bgfx::setViewTransform(mDebugView->id, viewMatrix, projectionMatrix);
         bgfx::setViewFrameBuffer(mDebugView->id, debugBuffer);
         Debug::renderDebug(this, mDebugView->id);

         bgfx::destroyFrameBuffer(debugBuffer);*/
      }

      // Render To Texture
      if (bgfx::isValid(mFinishBuffer))
      {
         bgfx::setViewFrameBuffer(mFinishView->id, mFinishBuffer);
      }

      // Finish
      if (mFinishEnabled)
      {
         bgfx::setViewTransform(mFinishView->id, NULL, proj);
         bgfx::setViewRect(mFinishView->id, 0, 0, width, height);
         bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), getPostSource());
         bgfx::setState(0
            | BGFX_STATE_RGB_WRITE
            | BGFX_STATE_ALPHA_WRITE
            );
         fullScreenQuad((F32)width, (F32)height);
         bgfx::submit(mFinishView->id, mFinishShader->mProgram);
      }
   }

   // ----------------------------------------
   //  Render Path Management
   // ----------------------------------------
   static HashMap < const char*, _CreateRenderPathFunc_ > sRenderPathCreateFuncs;

   void registerRenderPath(const char* renderPathName, _CreateRenderPathFunc_ createFuncPtr)
   {
      sRenderPathCreateFuncs.insert(renderPathName, createFuncPtr);
   }

   Rendering::RenderPath* getRenderPathInstance(const char* renderPathName, Rendering::RenderCamera* camera)
   {
      Rendering::_CreateRenderPathFunc_ createFuncPtr = sRenderPathCreateFuncs[renderPathName];

      if (createFuncPtr != NULL)
         return createFuncPtr(camera);

      return NULL;
   }
}
