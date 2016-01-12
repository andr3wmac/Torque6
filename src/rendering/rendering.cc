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
   void initBuffers();
   void destroyBuffers();

   bool        canvasSizeChanged = false;
   U32         canvasWidth = 0;
   U32         canvasHeight = 0;
   U32         canvasClearColor = 0;

   RenderData  renderDataList[65535];
   U32         renderDataCount = 0;

   LightData   lightList[2048];
   U32         lightCount = 0;
   Point3F     directionalLightDir;
   ColorF      directionalLightColor;

   Vector<RenderCamera*> cameraList;
   RenderCamera* activeCamera = NULL;

   RenderCamera* getActiveCamera()
   {
      return activeCamera;
   }

   void addRenderCamera(RenderCamera* camera)
   {
      cameraList.push_back(camera);

      if (activeCamera == NULL)
         activeCamera = camera;
   }

   void removeRenderCamera(RenderCamera* camera)
   {
       for (Vector< RenderCamera* >::iterator itr = cameraList.begin(); itr != cameraList.end(); ++itr)
       {
           if ((*itr) == camera)
           {
               cameraList.erase(itr);
               break;
           }
       }

       if (activeCamera == camera)
           activeCamera = NULL;
   }

   struct BackBuffer
   {
      Graphics::ViewTableEntry*  view;
      bgfx::FrameBufferHandle    buffer;
      bgfx::TextureHandle        colorTexture;
      bgfx::TextureHandle        normalTexture;
      bgfx::TextureHandle        matInfoTexture;
      bgfx::TextureHandle        depthTexture;
      bgfx::TextureHandle        depthTextureRead;
   } static gBackBuffer;

   struct CommonUniforms
   {
      bgfx::UniformHandle camPos;
      bgfx::UniformHandle time;
      bgfx::UniformHandle sceneViewMat;
      bgfx::UniformHandle sceneInvViewMat;
      bgfx::UniformHandle sceneProjMat;
      bgfx::UniformHandle sceneInvProjMat;
      bgfx::UniformHandle sceneViewProjMat;
      bgfx::UniformHandle sceneInvViewProjMat;
   } static gCommonUniforms;

   bgfx::FrameBufferHandle getBackBuffer()
   {
      return gBackBuffer.buffer;
   }

   bgfx::TextureHandle getColorTexture()
   {
      return gBackBuffer.colorTexture;
   }

   bgfx::TextureHandle getDepthTexture()
   {
      return gBackBuffer.depthTexture;
   }

   bgfx::TextureHandle getDepthTextureRead()
   {
      return gBackBuffer.depthTextureRead;
   }

   bgfx::TextureHandle getNormalTexture()
   {
      return gBackBuffer.normalTexture;
   }

   bgfx::TextureHandle getMatInfoTexture()
   {
      return gBackBuffer.matInfoTexture;
   }

   void init()
   {
      gBackBuffer.view                 = Graphics::getView("BackBuffer", 2000);
      gBackBuffer.buffer.idx           = bgfx::invalidHandle;
      gBackBuffer.colorTexture.idx     = bgfx::invalidHandle;
      gBackBuffer.normalTexture.idx    = bgfx::invalidHandle;
      gBackBuffer.matInfoTexture.idx   = bgfx::invalidHandle;
      gBackBuffer.depthTexture.idx     = bgfx::invalidHandle;
      gBackBuffer.depthTextureRead.idx = bgfx::invalidHandle;

      // Common Uniforms
      gCommonUniforms.camPos                = Graphics::Shader::getUniformVec4("u_camPos");
      gCommonUniforms.time                  = Graphics::Shader::getUniform("u_time", bgfx::UniformType::Vec4);
      gCommonUniforms.sceneViewMat          = Graphics::Shader::getUniformMat4("u_sceneViewMat", 1);
      gCommonUniforms.sceneInvViewMat       = Graphics::Shader::getUniformMat4("u_sceneInvViewMat", 1);
      gCommonUniforms.sceneProjMat          = Graphics::Shader::getUniformMat4("u_sceneProjMat", 1);
      gCommonUniforms.sceneInvProjMat       = Graphics::Shader::getUniformMat4("u_sceneInvProjMat", 1);
      gCommonUniforms.sceneViewProjMat      = Graphics::Shader::getUniformMat4("u_sceneViewProjMat", 1);
      gCommonUniforms.sceneInvViewProjMat   = Graphics::Shader::getUniformMat4("u_sceneInvViewProjMat", 1);

      initBuffers();
   }

   void destroy()
   {
      destroyBuffers();
   }

   void initBuffers()
   {
      destroyBuffers();

      const U32 samplerFlags = 0
         | BGFX_TEXTURE_RT
         | BGFX_TEXTURE_MIN_POINT
         | BGFX_TEXTURE_MAG_POINT
         | BGFX_TEXTURE_MIP_POINT
         | BGFX_TEXTURE_U_CLAMP
         | BGFX_TEXTURE_V_CLAMP;

      // Create Color Buffer
      gBackBuffer.colorTexture = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, 1, bgfx::TextureFormat::BGRA8, samplerFlags);

      // Create Depth Buffer
      gBackBuffer.depthTexture      = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, 1, bgfx::TextureFormat::D24, samplerFlags);
      gBackBuffer.depthTextureRead  = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, 1, bgfx::TextureFormat::D24, samplerFlags | BGFX_TEXTURE_BLIT_DST);

      // Create Normals Buffer
      gBackBuffer.normalTexture = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, 1, bgfx::TextureFormat::BGRA8, samplerFlags);

      // Create Material Info Buffer
      gBackBuffer.matInfoTexture = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, 1, bgfx::TextureFormat::BGRA8, samplerFlags);

      // Create "Backbuffer"
      bgfx::TextureHandle backBufferTextures[2] = { gBackBuffer.colorTexture, gBackBuffer.depthTexture };
      gBackBuffer.buffer = bgfx::createFrameBuffer(BX_COUNTOF(backBufferTextures), backBufferTextures, false);
   }

   void destroyBuffers() 
   {
      // Destroy backbuffers.
      if (bgfx::isValid(gBackBuffer.buffer))
         bgfx::destroyFrameBuffer(gBackBuffer.buffer);

      // Destroy textures.
      if (bgfx::isValid(gBackBuffer.colorTexture))
         bgfx::destroyTexture(gBackBuffer.colorTexture);
      if (bgfx::isValid(gBackBuffer.depthTexture))
         bgfx::destroyTexture(gBackBuffer.depthTexture);
      if (bgfx::isValid(gBackBuffer.depthTextureRead))
         bgfx::destroyTexture(gBackBuffer.depthTextureRead);
      if (bgfx::isValid(gBackBuffer.normalTexture))
         bgfx::destroyTexture(gBackBuffer.normalTexture);
      if (bgfx::isValid(gBackBuffer.matInfoTexture))
         bgfx::destroyTexture(gBackBuffer.matInfoTexture);
   }

   void updateCanvas(U32 width, U32 height, U32 clearColor)
   {
      canvasSizeChanged = ( canvasWidth != width || canvasHeight != height );
      canvasWidth = width;
      canvasHeight = height;
      canvasClearColor = clearColor;

      if (activeCamera)
      {
         F32 camFovy = 60.0f;
         F32 camAspect = F32(canvasWidth) / F32(canvasHeight);
         activeCamera->projectionHeight = 1.0f / mTan(bx::toRad(camFovy) * 0.5f);
         activeCamera->projectionWidth = activeCamera->projectionHeight * camAspect;
         bx::mtxProj(activeCamera->projectionMatrix, camFovy, camAspect, activeCamera->nearPlane, activeCamera->farPlane);
      }

      if (canvasSizeChanged)
      {
         resize();
         Scene::refresh();
      }
   }

   void setCommonUniforms()
   {
      // Common Uniforms
      //Point3F camPos = Scene::getActiveCamera()->getPosition();
      //bgfx::setUniform(gCommonUniforms.camPos, Point4F(camPos.x, camPos.y, camPos.z, 0.0f));
      F32 time = (F32)Sim::getCurrentTime();
      bgfx::setUniform(gCommonUniforms.time, Point4F(time, 0.0f, 0.0f, 0.0f));

      if (activeCamera)
      {
         bgfx::setUniform(gCommonUniforms.sceneViewMat, activeCamera->viewMatrix, 1);
         bgfx::setUniform(gCommonUniforms.sceneProjMat, activeCamera->projectionMatrix, 1);

         float invViewMat[16];
         bx::mtxInverse(invViewMat, activeCamera->viewMatrix);
         bgfx::setUniform(gCommonUniforms.sceneInvViewMat, invViewMat, 1);

         float invProjMat[16];
         bx::mtxInverse(invProjMat, activeCamera->projectionMatrix);
         bgfx::setUniform(gCommonUniforms.sceneInvProjMat, invProjMat, 1);

         float viewProjMtx[16];
         bx::mtxMul(viewProjMtx, activeCamera->viewMatrix, activeCamera->projectionMatrix);
         bgfx::setUniform(gCommonUniforms.sceneViewProjMat, viewProjMtx, 1);

         float invViewProjMtx[16];
         bx::mtxInverse(invViewProjMtx, viewProjMtx);
         bgfx::setUniform(gCommonUniforms.sceneInvViewProjMat, invViewProjMtx, 1);
      }
   }

   // Process Frame
   void render()
   {
      // Reset the view table. This clears bgfx view settings and temporary views.
      Graphics::resetViews();

      // bgfx::setUniform is tied to the next view that's touched/submitted so
      // we touched view 0 here so these uniforms are set for all views for
      // the whole frame.
      setCommonUniforms();
      bgfx::touch(0);

      if (activeCamera)
      {
         bgfx::setViewFrameBuffer(gBackBuffer.view->id, gBackBuffer.buffer);
         bgfx::setViewRect(gBackBuffer.view->id, 0, 0, canvasWidth, canvasHeight);
         bgfx::setViewTransform(gBackBuffer.view->id, activeCamera->viewMatrix, activeCamera->projectionMatrix);
         bgfx::touch(gBackBuffer.view->id);
      }

      // Render each camera.
      for (S32 i = 0; i < cameraList.size(); ++i)
      {
         RenderCamera* camera = cameraList[i];

         // Active camera is processed last.
         if (camera == activeCamera)
            continue;

         camera->render();
      }

      // Render active camera last.
      if (activeCamera != NULL)
         activeCamera->render();
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
      item->transformCount          = 0;
      item->transformTable          = NULL;
      item->textures                = NULL;
      item->view                    = 0;
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
   //   Lights
   // ----------------------------------------
   LightData* createLightData()
   {
      LightData* light = NULL;
      for (U32 n = 0; n < lightCount; ++n)
      {
         if (lightList[n].flags & LightData::Deleted)
         {
            light = &lightList[n];
            break;
         }
      }

      if (light == NULL)
      {
         light = &lightList[lightCount];
         lightCount++;
      }

      // Reset Values
      light->flags = 0;
      light->color[0] = 0.0f;
      light->color[1] = 0.0f;
      light->color[2] = 0.0f;
      light->attenuation = 0.0f;
      light->intensity = 0.0f;
      light->position.set(0.0f, 0.0f, 0.0f);

      return light;
   }

   Vector<LightData*> getLightList()
   {
      Vector<LightData*> results;
      for (U32 n = 0; n < lightCount; ++n)
      {
         if (lightList[n].flags & LightData::Deleted)
            continue;
         results.push_back(&lightList[n]);
      }
      return results;
   }

   Vector<LightData*> getNearestLights(Point3F position)
   {
      //U64 hpFreq = bx::getHPFrequency() / 1000000.0; // micro-seconds.
      //U64 startTime = bx::getHPCounter();

      Vector<LightData*> results;
      F32 lightDistance[4];

      U32 n;
      S32 i;
      for ( n = 0; n < lightCount; ++n )
      {
         if (lightList[n].flags & LightData::Deleted)
            continue;

         // Get distance.
         Point3F lightVector = position - lightList[n].position;
         F32 distToLight = lightVector.len();

         // We need a max of 4 lights
         if ( results.size() < 4 )
         {
            results.push_back(&lightList[n]);
            lightDistance[results.size() - 1] = distToLight;
            continue;
         }

         // Check for shortest distance.
         for( i = 0; i < 4; ++i )
         {
            if ( lightDistance[i] < distToLight )
               continue;

            lightDistance[i] = distToLight;
            results[i] = &lightList[n];
            break;
         }
      }

      //U64 endTime = bx::getHPCounter();
      //Con::printf("getNearestLights took: %d microseconds. (1 microsecond = 0.001 milliseconds)", (U32)((endTime - startTime) / hpFreq));

      return results;
   }

   // Directional Light
   void setDirectionalLight(Point3F direction, ColorF color)
   {
      directionalLightDir = direction;
      directionalLightColor = color;
   }

   // Debug Function
   void testGetNearestLights()
   {
      /*for( U32 n = 0; n < 10000; ++n )
      {
         Rendering::LightData lData;
         lData.position = Point3F(mRandF(-10000, 10000), mRandF(-10000, 10000), mRandF(-10000, 10000));
         lightList.push_back(lData);
      }
      Vector<LightData*> nearestLights = getNearestLights(Point3F(0, 0, 0));
      for( S32 n = 0; n < nearestLights.size(); ++n )
      {
         Con::printf("Nearest Light: %f %f %f", nearestLights[n]->position.x, nearestLights[n]->position.y, nearestLights[n]->position.z);
      }
      lightList.clear();
      */
   }

   // ----------------------------------------
   //   Utility Functions
   // ----------------------------------------
   
   Point2I worldToScreen(Point3F worldPos)
   {
      F32 viewProjMatrix[16];
      bx::mtxMul(viewProjMatrix, activeCamera->viewMatrix, activeCamera->projectionMatrix);

      F32 projectedOutput[3];
      F32 projectedInput[3] = {worldPos.x, worldPos.y, worldPos.z};
      bx::vec3MulMtxH(projectedOutput, projectedInput, viewProjMatrix);

      projectedOutput[0] = (projectedOutput[0] + 1.0f) / 2.0f;
      projectedOutput[1] = ((projectedOutput[1] + 1.0f) / 2.0f);
      projectedOutput[0] *= Rendering::canvasWidth;
      projectedOutput[1] *= Rendering::canvasHeight;
      projectedOutput[1] = Rendering::canvasHeight - projectedOutput[1];

      return Point2I((S32)projectedOutput[0], (S32)projectedOutput[1]);
   }

   Point3F screenToWorld(Point2I screenPos)
   {
      F32 x = (2.0f * screenPos.x) / canvasWidth - 1.0f;
      F32 y = 1.0f - (2.0f * screenPos.y) / canvasHeight;
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
}
