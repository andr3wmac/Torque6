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

#include "environmentProbeComponent.h"
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
#include <debugdraw/debugdraw.h>

namespace Scene
{
   // ----------------------------------------------
   // EnvironmentProbe Component
   // ----------------------------------------------

   IMPLEMENT_CONOBJECT(EnvironmentProbeComponent);

   EnvironmentProbeComponent::EnvironmentProbeComponent()
   {
      priority       = 100;
      mState         = 0;
      mDiffuseShader  = Graphics::getDefaultShader("components/environmentProbe/environmentProbe_vs.tsh", "components/environmentProbe/environmentProbeDiffuse_fs.tsh");
      mSpecularShader = Graphics::getDefaultShader("components/environmentProbe/environmentProbe_vs.tsh", "components/environmentProbe/environmentProbeSpecular_fs.tsh");

      // Input
      mEnvironmentCubemap.idx    = bgfx::invalidHandle;
      mEnvironmentCamera         = NULL;
      mEnvironmentCameraFilter   = new EnvironmentProbeFilter();
      
      for (U8 n = 0; n < 6; ++n)
      {
         mEnvironmentCubemapBuffers[n].idx = bgfx::invalidHandle;
      }

      // Cubemap Processor
      mCubemapProcessor = new Lighting::GPUCubemapProcessor();

      // Output
      mRadianceCubemap.idx    = bgfx::invalidHandle;
      mIrradianceCubemap.idx  = bgfx::invalidHandle;
      mBRDFTexture.idx        = bgfx::invalidHandle;

      initBuffers();
   }

   EnvironmentProbeComponent::~EnvironmentProbeComponent()
   {
      SAFE_DELETE(mEnvironmentCameraFilter);

      destroyBuffers();
   }

   void EnvironmentProbeComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();
   }

   void EnvironmentProbeComponent::resize()
   {
      refresh();
   }

   void EnvironmentProbeComponent::onAddToScene()
   {
      Scene::addPreprocessor(this);
      Rendering::addRenderHook(this);
   }

   void EnvironmentProbeComponent::onRemoveFromScene()
   {
      Rendering::removeRenderHook(this);
      Scene::removePreprocessor(this);
   }

   void EnvironmentProbeComponent::initBuffers()
   {
      destroyBuffers();

      mEnvironmentCubemap  = bgfx::createTextureCube(512, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT);
      mRadianceCubemap     = bgfx::createTextureCube(512, 6, bgfx::TextureFormat::RGBA16F, BGFX_TEXTURE_RT);
      mIrradianceCubemap   = bgfx::createTextureCube(128, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT);
      mBRDFTexture         = bgfx::createTexture2D(512, 512, 1, bgfx::TextureFormat::RG16F, BGFX_TEXTURE_RT);
   }

   void EnvironmentProbeComponent::destroyBuffers()
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

   void EnvironmentProbeComponent::refresh()
   {
      Parent::refresh();

      mBoundingBox.set(Point3F(1.0f, 1.0f, 1.0f), Point3F(-1.0f, -1.0f, -1.0f));
      mBoundingBox.transform(mTransform);

      if (mState < 4)
         return;

      //Lighting::setEnvironmentLight(mRadianceCubemap, mIrradianceCubemap, mBRDFTexture);
   }

   void EnvironmentProbeComponent::preprocess()
   {
      if (mState == 0)
      {
         startEnvironmentCapture();
         return;
      }

      if (mState == 1)
      {
         captureEnvironment();
         return;
      }

      if (mState == 2)
      {
         endEnvironmentCapture();
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
         isFinished = true;
         refresh();
      }
   }

   void EnvironmentProbeComponent::startEnvironmentCapture()
   {
      mEnvironmentCamera                  = Rendering::createRenderCamera("EnvironmentCamera", "ForwardShading");
      mEnvironmentCamera->width           = 512;
      mEnvironmentCamera->height          = 512;
      mEnvironmentCamera->matchWindowSize = false;
      mEnvironmentCamera->fov             = 90.0f;
      mEnvironmentCaptureSide             = 0;

      // Add a render filter that will prevent dynamic objects from rendering.
      mEnvironmentCamera->addRenderFilter(mEnvironmentCameraFilter);

      mEnvironmentCamera->render();

      mState++;
   }

   void EnvironmentProbeComponent::captureEnvironment()
   {
      VectorF up = Point3F(0.0f, 0.0f, 1.0f);
      Point3F look = mWorldPosition;

      switch (mEnvironmentCaptureSide)
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
      bx::mtxLookAt(mEnvironmentCamera->viewMatrix, mWorldPosition, look, up);

      bgfx::Attachment frameBufferAttachment;
      frameBufferAttachment.handle  = mEnvironmentCubemap;
      frameBufferAttachment.layer   = mEnvironmentCaptureSide;
      frameBufferAttachment.mip     = 0;

      mEnvironmentCubemapBuffers[mEnvironmentCaptureSide] = bgfx::createFrameBuffer(1, &frameBufferAttachment);
      mEnvironmentCamera->setRenderFrameBuffer(mEnvironmentCubemapBuffers[mEnvironmentCaptureSide]);

      // Because we're a ScenePreprocessor we have to render the camera manually.
      mEnvironmentCamera->render();

      mEnvironmentCaptureSide++;
      if (mEnvironmentCaptureSide == 6)
      {
         mState++;
         return;
      }
   }

   void EnvironmentProbeComponent::endEnvironmentCapture()
   {
      Rendering::destroyRenderCamera(mEnvironmentCamera);
      mEnvironmentCamera = NULL;

      mCubemapProcessor->init(mEnvironmentCubemap, 512, mRadianceCubemap, 512, mIrradianceCubemap, 128, mBRDFTexture);
      mState++;
   }

   void EnvironmentProbeComponent::preRender(Rendering::RenderCamera* camera)
   {

   }

   void EnvironmentProbeComponent::render(Rendering::RenderCamera* camera)
   {
      if (mState < 4)
         return;

      if (!camera->getRenderPath()->hasDiffuseLightBuffer() || !camera->getRenderPath()->hasSpecularLightBuffer())
         return;

      Box3F boundingBox = mBoundingBox;
      boundingBox.transform(mTransformMatrix);
      Point3F center = boundingBox.getCenter();

      F32 volumeBegin[4] = { boundingBox.minExtents.x, boundingBox.minExtents.y, boundingBox.minExtents.z, 0.0f };
      F32 volumePosition[4] = { center.x, center.y, center.z, 0.0f };
      F32 volumeSize[4] = { boundingBox.maxExtents.x - boundingBox.minExtents.x, boundingBox.maxExtents.y - boundingBox.minExtents.y, boundingBox.maxExtents.z - boundingBox.minExtents.z, 0.0f };

      // Diffuse 
      {
         Graphics::ViewTableEntry* diffuseView = camera->getRenderPath()->getDiffuseLightBufferView();

         // Setup textures
         bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), camera->getRenderPath()->getNormalTexture());  // Normals
         bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), camera->getRenderPath()->getDepthTextureRead());   // Depth
         bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), mIrradianceCubemap);

         // Setup uniforms
         bgfx::setUniform(Graphics::Shader::getUniformVec4("u_volumeStart"), volumeBegin);
         bgfx::setUniform(Graphics::Shader::getUniformVec4("u_volumePosition"), volumePosition);
         bgfx::setUniform(Graphics::Shader::getUniformVec4("u_volumeSize"), volumeSize);

         bgfx::setState(0
            | BGFX_STATE_RGB_WRITE
            | BGFX_STATE_ALPHA_WRITE
            | BGFX_STATE_CULL_CCW 
            | BGFX_STATE_BLEND_ADD
            );

         bgfx::setIndexBuffer(Graphics::cubeIB);
         bgfx::setVertexBuffer(Graphics::cubeVB);
         bgfx::setTransform(&mTransformMatrix[0]);

         bgfx::submit(diffuseView->id, mDiffuseShader->mProgram);
      }

      // Specular
      {
         Graphics::ViewTableEntry* specularView = camera->getRenderPath()->getSpecularLightBufferView();

         // Setup textures
         bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), camera->getRenderPath()->getNormalTexture());  // Normals
         bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), camera->getRenderPath()->getMatInfoTexture()); // Material Info
         bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), camera->getRenderPath()->getDepthTextureRead());   // Depth
         bgfx::setTexture(3, Graphics::Shader::getTextureUniform(3), mBRDFTexture);
         bgfx::setTexture(4, Graphics::Shader::getTextureUniform(4), mRadianceCubemap);

         // Setup uniforms
         bgfx::setUniform(Graphics::Shader::getUniformVec4("u_volumeStart"), volumeBegin);
         bgfx::setUniform(Graphics::Shader::getUniformVec4("u_volumePosition"), volumePosition);
         bgfx::setUniform(Graphics::Shader::getUniformVec4("u_volumeSize"), volumeSize);

         bgfx::setState(0
            | BGFX_STATE_RGB_WRITE
            | BGFX_STATE_ALPHA_WRITE
            | BGFX_STATE_CULL_CCW
            );

         bgfx::setIndexBuffer(Graphics::cubeIB);
         bgfx::setVertexBuffer(Graphics::cubeVB);
         bgfx::setTransform(&mTransformMatrix[0]);

         bgfx::submit(specularView->id, mSpecularShader->mProgram);
      }
   }

   void EnvironmentProbeComponent::postRender(Rendering::RenderCamera* camera)
   {

   }

   // ----------------------------------------------
   // Environment Probe Filter : Filters dynamic objects from the environment probe rendering.
   // ----------------------------------------------

   EnvironmentProbeFilter::EnvironmentProbeFilter()
   {
      mPriority = 5000;
   }

   void EnvironmentProbeFilter::execute()
   {
      Rendering::RenderData* renderData = Rendering::getRenderDataList();

      // Loop through each render data and cull it.
      for (U32 n = 0; n < Rendering::getRenderDataCount(); ++n, ++renderData)
      {
         // Items without bounds are not subject to frustum culling.
         if (!(renderData->flags & Rendering::RenderData::HasBounds))
            continue;

         if (!(renderData->flags & Rendering::RenderData::Static))
            renderData->flags |= Rendering::RenderData::Filtered;
      }
   }

   // ----------------------------------------
   //   EnvProbeDebug : Displays bounds of environment probes.
   // ----------------------------------------

   IMPLEMENT_DEBUG_MODE("EnvProbe", EnvProbeDebug);

   bool EnvProbeDebug::EnvProbeDebugEnabled = false;

   void EnvProbeDebug::onEnable()
   {
      EnvProbeDebugEnabled = true;
   }

   void EnvProbeDebug::onDisable()
   {
      EnvProbeDebugEnabled = false;
   }

   void EnvProbeDebug::render(Rendering::RenderCamera* camera)
   {
      ddSetColor(BGFXCOLOR_RGBA(0, 255, 0, 255));
      ddSetWireframe(true);
      ddSetState(true, true, true);

      Vector<SimObject*> probes = Scene::findComponentsByType("EnvironmentProbeComponent");
      for (S32 n = 0; n < probes.size(); ++n)
      {
         EnvironmentProbeComponent* probe = dynamic_cast<EnvironmentProbeComponent*>(probes[n]);
         Box3F bounds = probe->getBoundingBox();
         bounds.transform(probe->getTransform());
         Aabb box;
         box.m_min[0] = bounds.minExtents.x;
         box.m_min[1] = bounds.minExtents.y;
         box.m_min[2] = bounds.minExtents.z;
         box.m_max[0] = bounds.maxExtents.x;
         box.m_max[1] = bounds.maxExtents.y;
         box.m_max[2] = bounds.maxExtents.z;
         ddDraw(box);
      }
   }
}
