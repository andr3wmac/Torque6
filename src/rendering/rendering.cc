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


#include "rendering.h"
#include "console/consoleInternal.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "scene/scene.h"
#include "rendering/transparency.h"
#include "renderCamera.h"

#include <bgfx/bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   // Canvas (TODO: remove/refactor this)
   bool        windowSizeChanged = false;
   U32         windowWidth = 0;
   U32         windowHeight = 0;

   // Render Data
   RenderData  renderDataList[TORQUE_MAX_RENDER_DATA];
   U32         renderDataCount = 0;

   // Render Cameras, Textures, and Hooks.
   Vector<RenderCamera*>   renderCameraList;
   Vector<RenderTexture*>  renderTextureList;
   Vector<RenderHook*>     renderHookList;

   void init()
   {

   }

   void destroy()
   {
      for (S32 n = 0; n < renderTextureList.size(); ++n)
      {
         RenderTexture* rt = renderTextureList[n];
         if (bgfx::isValid(rt->handle))
            bgfx::destroyTexture((rt->handle));
      }
   }

   void updateWindow(U32 width, U32 height)
   {
      windowSizeChanged = (windowWidth != width || windowHeight != height );
      windowWidth = width;
      windowHeight = height;

      if (windowSizeChanged)
      {
         Graphics::reset();
         resize();
         Scene::refresh();
      }
   }

   S32 QSORT_CALLBACK compareRenderCameraPriority(const void* a, const void* b)
   {
      RenderCamera* cameraA = *((RenderCamera**)a);
      RenderCamera* cameraB = *((RenderCamera**)b);
      return cameraA->getRenderPriority() > cameraB->getRenderPriority();
   }

   // Process Frame
   void render()
   {
      // Reset the view table. This clears bgfx view settings and temporary views.
      Graphics::resetViews();

      // We don't continue with rendering until preprocessing is complete (for now)
      if (Scene::isPreprocessingActive(true))
         return;

      // Render Hooks also get notified about begin/end of frame.
      for (S32 n = 0; n < renderHookList.size(); ++n)
         renderHookList[n]->beginFrame();

      // Sort Cameras
      dQsort(renderCameraList.address(), renderCameraList.size(), sizeof(RenderCamera*), compareRenderCameraPriority);

      // Render each camera.
      for (S32 i = 0; i < renderCameraList.size(); ++i)
      {
         RenderCamera* camera = renderCameraList[i];
         camera->render();
      }

      // End of frame
      for (S32 n = 0; n < renderHookList.size(); ++n)
         renderHookList[n]->endFrame();
   }

   void resize()
   {
      
   }

   // ----------------------------------------
   //   Render Data
   // ----------------------------------------

   RenderData* createRenderData()
   {
      RenderData* item = NULL;
      for ( U32 n = 0; n < renderDataCount; ++n )
      {
         if ( renderDataList[n].flags & RenderData::Deleted )
         {
            item = &renderDataList[n];
            break;
         }
      }

      if ( item == NULL )
      {
         item = &renderDataList[renderDataCount];
         renderDataCount++;
      }

      // Reset Values
      item->flags                   = 0;
      item->instances               = NULL;
      item->dynamicIndexBuffer.idx  = bgfx::invalidHandle;
      item->dynamicVertexBuffer.idx = bgfx::invalidHandle;
      item->indexBuffer.idx         = bgfx::invalidHandle;
      item->vertexBuffer.idx        = bgfx::invalidHandle;
      item->shader.idx              = bgfx::invalidHandle;
      item->material                = NULL;
      item->transformCount          = 0;
      item->transformTable          = NULL;
      item->textures                = NULL;
      item->stateRGBA               = 0;

      item->state = 0 | BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
         | BGFX_STATE_DEPTH_TEST_LESS
         | BGFX_STATE_DEPTH_WRITE
         | BGFX_STATE_CULL_CW;
      
      return item;
   }

   RenderData* getRenderDataList()
   {
      return renderDataList;
   }

   U32 getRenderDataCount()
   {
      return renderDataCount;
   }

   // ----------------------------------------
   //   Utility Functions
   // ----------------------------------------
   
   Point2I worldToScreen(Point3F worldPos)
   {
      RenderCamera* activeCamera = getPriorityRenderCamera();

      F32 viewProjMatrix[16];
      bx::mtxMul(viewProjMatrix, activeCamera->viewMatrix, activeCamera->projectionMatrix);

      F32 projectedOutput[3];
      F32 projectedInput[3] = {worldPos.x, worldPos.y, worldPos.z};
      bx::vec3MulMtxH(projectedOutput, projectedInput, viewProjMatrix);

      projectedOutput[0] = (projectedOutput[0] + 1.0f) / 2.0f;
      projectedOutput[1] = ((projectedOutput[1] + 1.0f) / 2.0f);
      projectedOutput[0] *= Rendering::windowWidth;
      projectedOutput[1] *= Rendering::windowHeight;
      projectedOutput[1] = Rendering::windowHeight - projectedOutput[1];

      return Point2I((S32)projectedOutput[0], (S32)projectedOutput[1]);
   }

   Point3F screenToWorld(Point2I screenPos)
   {
      RenderCamera* activeCamera = getPriorityRenderCamera();

      F32 x = (2.0f * screenPos.x) / Rendering::windowWidth - 1.0f;
      F32 y = 1.0f - (2.0f * screenPos.y) / Rendering::windowHeight;
      F32 z = -1.0f;
      Point4F ray_clip(x * -1.0f, y * -1.0f, z, -1.0);

      F32 invProjMtx[16];
      bx::mtxInverse(invProjMtx, activeCamera->projectionMatrix);

      Point4F ray_eye;
      bx::vec4MulMtx(ray_eye, ray_clip, invProjMtx);
      ray_eye.z = -1.0f;
      ray_eye.w = 0.0f;

      F32 invViewMtx[16];
      bx::mtxInverse(invViewMtx, activeCamera->viewMatrix);

      Point4F ray_wor;
      bx::vec4MulMtx(ray_wor, ray_eye, invViewMtx);
      Point3F ray_final(ray_wor.x, ray_wor.y, ray_wor.z);
      ray_final.normalize();
      ray_final = ray_final * -1.0f;

      return ray_final;
   }

   bool closestPointsOnTwoLines(Point3F& closestPointLine1, Point3F& closestPointLine2, Point3F linePoint1, Point3F lineVec1, Point3F linePoint2, Point3F lineVec2)
   {
      closestPointLine1 = Point3F::Zero;
      closestPointLine2 = Point3F::Zero;

      float a = mDot(lineVec1, lineVec1);
      float b = mDot(lineVec1, lineVec2);
      float e = mDot(lineVec2, lineVec2);

      float d = a*e - b*b;

      //lines are not parallel
      if (d != 0.0f) {

         Point3F r = linePoint1 - linePoint2;
         float c = mDot(lineVec1, r);
         float f = mDot(lineVec2, r);

         float s = (b*f - c*e) / d;
         float t = (a*f - c*b) / d;

         closestPointLine1 = linePoint1 + lineVec1 * s;
         closestPointLine2 = linePoint2 + lineVec2 * t;

         return true;
      }

      else {
         return false;
      }
   }

   // ----------------------------------------
   //   Render Camera
   // ----------------------------------------

   RenderCamera* createRenderCamera(StringTableEntry name, StringTableEntry renderingPath)
   {
      RenderCamera* camera = getRenderCamera(name);
      if (camera != NULL)
         return camera;

      camera = new RenderCamera(renderingPath);
      camera->setName(name);
      renderCameraList.push_back(camera);

      return camera;
   }

   RenderCamera* getRenderCamera(StringTableEntry name)
   {
      for (Vector< RenderCamera* >::iterator itr = renderCameraList.begin(); itr != renderCameraList.end(); ++itr)
      {
         if ((*itr)->getName() == name)
         {
            return (*itr);
         }
      }
      return NULL;
   }

   // Return highest priorty render camera.
   RenderCamera* getPriorityRenderCamera()
   {
      if (renderCameraList.size() < 1)
         return NULL;

      dQsort(renderCameraList.address(), renderCameraList.size(), sizeof(RenderCamera*), compareRenderCameraPriority);
      return renderCameraList[renderCameraList.size() - 1];
   }

   bool destroyRenderCamera(RenderCamera* camera)
   {
      for (Vector< RenderCamera* >::iterator itr = renderCameraList.begin(); itr != renderCameraList.end(); ++itr)
      {
         if ((*itr) == camera)
         {
            renderCameraList.erase(itr);
            SAFE_DELETE(camera);
            return true;
         }
      }

      return false;
   }

   bool destroyRenderCamera(StringTableEntry name)
   {
      for (Vector< RenderCamera* >::iterator itr = renderCameraList.begin(); itr != renderCameraList.end(); ++itr)
      {
         if ((*itr)->getName() == name)
         {
            Rendering::RenderCamera* camera = (*itr);
            renderCameraList.erase(itr);
            SAFE_DELETE(camera);
            return true;
         }
      }

      return false;
   }

   // ----------------------------------------
   //   Render Hooks
   // ----------------------------------------

   void addRenderHook(RenderHook* hook)
   {
      renderHookList.push_back(hook);
   }

   bool removeRenderHook(RenderHook* hook)
   {
      for (Vector< RenderHook* >::iterator itr = renderHookList.begin(); itr != renderHookList.end(); ++itr)
      {
         if ((*itr) == hook)
         {
            renderHookList.erase(itr);
            return true;
         }
      }
      return false;
   }

   Vector<RenderHook*>* getRenderHookList()
   {
      return &renderHookList;
   }

   // ----------------------------------------
   //   Render Textures
   // ----------------------------------------

   RenderTexture* createRenderTexture(StringTableEntry name, bgfx::BackbufferRatio::Enum ratio)
   {
      RenderTexture* rt = getRenderTexture(name);
      if (rt != NULL)
         return rt;

      const U32 flags = 0
         | BGFX_TEXTURE_RT
         | BGFX_TEXTURE_MIN_POINT
         | BGFX_TEXTURE_MAG_POINT
         | BGFX_TEXTURE_MIP_POINT
         | BGFX_TEXTURE_U_CLAMP
         | BGFX_TEXTURE_V_CLAMP;

      rt = new RenderTexture();
      rt->name    = name;
      rt->handle  = bgfx::createTexture2D(ratio, 1, bgfx::TextureFormat::BGRA8, flags);
      rt->width   = Rendering::windowWidth;
      rt->height  = Rendering::windowHeight;
      renderTextureList.push_back(rt);
      return rt;
   }

   RenderTexture* createRenderTexture(StringTableEntry name, U32 width, U32 height)
   {
      RenderTexture* rt = getRenderTexture(name);
      if (rt != NULL)
         return rt;

      const U32 flags = 0
         | BGFX_TEXTURE_RT
         | BGFX_TEXTURE_MIN_POINT
         | BGFX_TEXTURE_MAG_POINT
         | BGFX_TEXTURE_MIP_POINT
         | BGFX_TEXTURE_U_CLAMP
         | BGFX_TEXTURE_V_CLAMP;

      rt = new RenderTexture();
      rt->name    = name;
      rt->handle  = bgfx::createTexture2D(width, height, 1, bgfx::TextureFormat::BGRA8, flags);
      rt->width   = width;
      rt->height  = height;
      renderTextureList.push_back(rt);
      return rt;
   }

   RenderTexture* getRenderTexture(StringTableEntry name)
   {
      for (Vector< RenderTexture* >::iterator itr = renderTextureList.begin(); itr != renderTextureList.end(); ++itr)
      {
         if ((*itr)->name == name)
         {
            return (*itr);
         }
      }
      return NULL;
   }

   bool destroyRenderTexture(StringTableEntry name)
   {
      for (Vector< RenderTexture* >::iterator itr = renderTextureList.begin(); itr != renderTextureList.end(); ++itr)
      {
         if ((*itr)->name == name)
         {
            Rendering::RenderTexture* rt = (*itr);
            renderTextureList.erase(itr);
            SAFE_DELETE(rt);
            return true;
         }
      }
      return false;
   }
}
