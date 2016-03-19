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

#include "skyLight.h"
#include "console/consoleInternal.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "lighting/lighting.h"
#include "rendering/rendering.h"
#include "rendering/renderCamera.h"
#include "scene/components/cameraComponent.h"
#include "scene/scene.h"

#include <bx/fpumath.h>

namespace Scene
{
   // ----------------------------------------------
   // SkyLight Component
   // ----------------------------------------------

   IMPLEMENT_CONOBJECT(SkyLight);

   SkyLight::SkyLight()
   {
      //mName = "Sky Light";
      mState   = 0;
      mShader  = Graphics::getDefaultShader("components/skyLight/skyLight_vs.tsh", "components/skyLight/skyLight_fs.tsh");

      // Input
      mSkyLightCubemap.idx    = bgfx::invalidHandle;
      mSkyLightCamera         = NULL;
      mSkyLightCameraFilter   = new SkyLightFilter();
      
      for (U8 n = 0; n < 6; ++n)
      {
         mSkyLightCubemapBuffers[n].idx = bgfx::invalidHandle;
      }

      // Cubemap Processor
      mCubemapProcessor = new Lighting::GPUCubemapProcessor();

      // Output
      mRadianceCubemap.idx    = bgfx::invalidHandle;
      mIrradianceCubemap.idx  = bgfx::invalidHandle;
      mBRDFTexture.idx        = bgfx::invalidHandle;

      initBuffers();
   }

   SkyLight::~SkyLight()
   {
      SAFE_DELETE(mSkyLightCameraFilter);

      destroyBuffers();
   }

   void SkyLight::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();
   }

   void SkyLight::resize()
   {
      refresh();
   }

   void SkyLight::onAddToScene()
   {
      Rendering::addRenderHook(this);
   }

   void SkyLight::onRemoveFromScene()
   {
      Rendering::removeRenderHook(this);
   }

   void SkyLight::initBuffers()
   {
      destroyBuffers();

      mSkyLightCubemap     = bgfx::createTextureCube(512, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT);
      mRadianceCubemap     = bgfx::createTextureCube(512, 6, bgfx::TextureFormat::RGBA16F, BGFX_TEXTURE_RT);
      mIrradianceCubemap   = bgfx::createTextureCube(128, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT);
      mBRDFTexture         = bgfx::createTexture2D(512, 512, 1, bgfx::TextureFormat::RG16F, BGFX_TEXTURE_RT);
   }

   void SkyLight::destroyBuffers()
   {
      if (bgfx::isValid(mRadianceCubemap))
         bgfx::destroyTexture(mRadianceCubemap);

      if (bgfx::isValid(mIrradianceCubemap))
         bgfx::destroyTexture(mIrradianceCubemap);

      if (bgfx::isValid(mBRDFTexture))
         bgfx::destroyTexture(mBRDFTexture);

      mRadianceCubemap.idx    = bgfx::invalidHandle;
      mIrradianceCubemap.idx  = bgfx::invalidHandle;
      mBRDFTexture.idx        = bgfx::invalidHandle;
   }

   void SkyLight::refresh()
   {
      Parent::refresh();

      if (mState < 4)
         return;

      Lighting::setEnvironmentLight(mRadianceCubemap, mIrradianceCubemap, mBRDFTexture);
   }

   void SkyLight::beginFrame()
   {

   }

   void SkyLight::endFrame()
   {
      if (mState == 0)
      {
         startSkyLightCapture();
         return;
      }

      if (mState == 1)
      {
         captureSkyLight();
         return;
      }

      if (mState == 2)
      {
         endSkyLightCapture();
         return;
      }

      if (mState == 3)
      {
         if (!mCubemapProcessor->isFinished())
         {
            mCubemapProcessor->process();
            return;
         }

         mState = 4;
         refresh();
      }
   }

   void SkyLight::startSkyLightCapture()
   {
      mSkyLightCamera                  = Rendering::createRenderCamera("SkyLightCamera", "ForwardShading");
      mSkyLightCamera->width           = 512;
      mSkyLightCamera->height          = 512;
      mSkyLightCamera->matchWindowSize = false;
      mSkyLightCamera->fov             = 90.0f;
      mSkyLightCaptureSide             = 0;

      // Add a render filter that will prevent dynamic objects from rendering.
      mSkyLightCamera->addRenderFilter(mSkyLightCameraFilter);

      mState++;
   }

   void SkyLight::captureSkyLight()
   {
      VectorF up = Point3F(0.0f, 0.0f, 1.0f);
      Point3F look = mWorldPosition;

      switch (mSkyLightCaptureSide)
      {
         case 0: // +x
            look  = Point3F(1.0f, 0.0f, 0.0f);
            up    = Point3F(0.0f, 0.0f, 1.0f);
            break;
         case 1: // -x
            look  = Point3F(-1.0f, 0.0f, 0.0f);
            up    = Point3F(0.0f, 0.0f, 1.0f);
            break;
         case 2: // +z
            look  = Point3F(0.0f, 0.0f, 1.0f);
            up    = Point3F(0.0f, 1.0f, 0.0f);
            break;
         case 3: // -z
            look  = Point3F(0.0f, 0.0f, -1.0f);
            up    = Point3F(0.0f, 1.0f, 0.0f);
            break;
         case 4: // -y
            look  = Point3F(0.0f, -1.0f, 0.0f);
            up    = Point3F(0.0f, 0.0f, 1.0f);
            break;
         case 5: // +y
            look  = Point3F(0.0f, 1.0f, 0.0f);
            up    = Point3F(0.0f, 0.0f, 1.0f);
            break;
      }

      look += mWorldPosition;
      bx::mtxLookAt(mSkyLightCamera->viewMatrix, mWorldPosition, look, up);

      bgfx::Attachment frameBufferAttachment;
      frameBufferAttachment.handle  = mSkyLightCubemap;
      frameBufferAttachment.layer   = mSkyLightCaptureSide;
      frameBufferAttachment.mip     = 0;

      mSkyLightCubemapBuffers[mSkyLightCaptureSide] = bgfx::createFrameBuffer(1, &frameBufferAttachment);
      mSkyLightCamera->setRenderFrameBuffer(mSkyLightCubemapBuffers[mSkyLightCaptureSide]);

      mSkyLightCaptureSide++;
      if (mSkyLightCaptureSide == 6)
      {
         mState++;
         return;
      }
   }

   void SkyLight::endSkyLightCapture()
   {
      Rendering::destroyRenderCamera(mSkyLightCamera);
      mSkyLightCamera = NULL;

      mCubemapProcessor->init(mSkyLightCubemap, 512, mRadianceCubemap, 512, mIrradianceCubemap, 128, mBRDFTexture);
      mState++;
   }

   void SkyLight::preRender(Rendering::RenderCamera* camera)
   {

   }

   void SkyLight::render(Rendering::RenderCamera* camera)
   {
      if (mState < 4)
         return;

      if (!camera->getRenderPath()->hasAmbientBuffer())
         return;

      Graphics::ViewTableEntry* view = camera->getRenderPath()->getAmbientBufferView();

      // This projection matrix is used because its a full screen quad.
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
      bgfx::setViewTransform(view->id, NULL, proj);
      bgfx::setViewRect(view->id, 0, 0, camera->width, camera->height);

      // Setup textures
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), camera->getRenderPath()->getColorTexture());   // Deferred Albedo
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), camera->getRenderPath()->getNormalTexture());  // Normals
      bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), camera->getRenderPath()->getMatInfoTexture()); // Material Info
      bgfx::setTexture(3, Graphics::Shader::getTextureUniform(3), camera->getRenderPath()->getDepthTexture());   // Depth
      bgfx::setTexture(4, Lighting::environmentLight.brdfTextureUniform, Lighting::environmentLight.brdfTexture);
      bgfx::setTexture(5, Lighting::environmentLight.radianceCubemapUniform, Lighting::environmentLight.radianceCubemap);
      bgfx::setTexture(6, Lighting::environmentLight.irradianceCubemapUniform, Lighting::environmentLight.irradianceCubemap);

      bgfx::setState(0
         | BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
         );

      fullScreenQuad((F32)camera->width, (F32)camera->height);

      bgfx::submit(view->id, mShader->mProgram);
   }

   void SkyLight::postRender(Rendering::RenderCamera* camera)
   {

   }

   // ----------------------------------------------
   // SkyLight Filter : Filters all objects. Only renders the skybox.
   // ----------------------------------------------

   SkyLightFilter::SkyLightFilter()
   {
      mPriority = 5000;
   }

   void SkyLightFilter::execute()
   {
      Rendering::RenderData* renderData = Rendering::getRenderDataList();

      // Loop through each render data and cull it.
      for (U32 n = 0; n < Rendering::getRenderDataCount(); ++n, ++renderData)
      {
         // Items without bounds are not subject to frustum culling.
         if (!(renderData->flags & Rendering::RenderData::HasBounds))
            continue;

         // Filter all objects.
         renderData->flags |= Rendering::RenderData::Filtered;
      }
   }
}
