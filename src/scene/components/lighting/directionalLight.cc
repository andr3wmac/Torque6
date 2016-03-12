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

#include "directionalLight.h"
#include "console/consoleInternal.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "scene/scene.h"
#include "rendering/rendering.h"
#include "rendering/renderCamera.h"
#include "scene/components/cameraComponent.h"

#include <bx/fpumath.h>

namespace Scene
{
   IMPLEMENT_CONOBJECT(DirectionalLight);

   DirectionalLight::DirectionalLight()
   {
      mEnabled = false;

      mLightShader       = Graphics::getDefaultShader("components/directionalLight/dirlight_vs.tsh", "components/directionalLight/dirlight_fs.tsh");
      mDebugLightShader  = Graphics::getDefaultShader("components/directionalLight/dirlight_debug_vs.tsh", "components/directionalLight/dirlight_debug_fs.tsh");

      // ShadowMap size (per cascade)
      mCascadeSize         = 2048;
      mSplitDistribution   = 0.95f;
      mFarPlane            = 200.0f;
      mShadowMap.idx       = bgfx::invalidHandle;
      mShadowMapBuffer.idx = bgfx::invalidHandle;

      mColor.set(1.0f, 1.0f, 1.0f, 1.0f);
      mDirection.set(0.0f, -1.0f, 1.0f);

      // Initialize ShadowMap textures/buffers
      initBuffers();

      // Render to ShadowMap shaders
      mPCFShader        = Graphics::getDefaultShader("components/directionalLight/pcf_vs.tsh", "components/directionalLight/pcf_fs.tsh");
      mPCFSkinnedShader = Graphics::getDefaultShader("components/directionalLight/pcf_skinned_vs.tsh", "components/directionalLight/pcf_fs.tsh");

      // Shadow Params
      mShadowParamsUniform = bgfx::createUniform("u_shadowParams", bgfx::UniformType::Vec4);
      mBias                = 0.001f;
      mNormalOffset        = 1.0f;

      // ShadowMap cascade matrices
      mShadowMtxUniform = bgfx::createUniform("u_shadowMtx", bgfx::UniformType::Mat4, 4);

      // Get views for cascades
      mCascadeViews[0] = NULL;
      mCascadeViews[1] = NULL;
      mCascadeViews[2] = NULL;
      mCascadeViews[3] = NULL;
   }

   DirectionalLight::~DirectionalLight()
   {
      bgfx::destroyUniform(mShadowMtxUniform);

      destroyBuffers();
   }

   void DirectionalLight::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addGroup("DirectionalLight");

         addField("Color", TypeColorF, Offset(mColor, DirectionalLight), "");
         addField("Direction", TypePoint3F, Offset(mDirection, DirectionalLight), "");

      endGroup("DirectionalLight");

      addGroup("Shadows");

         addField("SplitDistribution", TypeF32, Offset(mSplitDistribution, DirectionalLight), "");
         addField("FarPlane", TypeF32, Offset(mFarPlane, DirectionalLight), "");
         addField("Bias", TypeF32, Offset(mBias, DirectionalLight), "");
         addField("NormalOffset", TypeF32, Offset(mNormalOffset, DirectionalLight), "");

      endGroup("Shadows");
   }

   void DirectionalLight::resize()
   {
      refresh();
   }

   void DirectionalLight::onAddToScene()
   {
      CameraComponent* camera = mOwnerObject->findComponentByType<CameraComponent>();
      if (!camera)
         return;

      mCamera = camera->getRenderCamera();
      if (!mCamera)
         return;

      Rendering::addRenderHook(this);

      mCascadeViews[0] = Graphics::getView("ShadowMap_Cascade0", 500, NULL);
      mCascadeViews[1] = Graphics::getView("ShadowMap_Cascade1");
      mCascadeViews[2] = Graphics::getView("ShadowMap_Cascade2");
      mCascadeViews[3] = Graphics::getView("ShadowMap_Cascade3");

      mEnabled = true;
   }

   void DirectionalLight::onRemoveFromScene()
   {
      mEnabled = false;

      if (mCamera)
         Rendering::removeRenderHook(this);
   }

   void DirectionalLight::initBuffers()
   {
      destroyBuffers();

      const U32 samplerFlags = 0
         | BGFX_TEXTURE_RT
         | BGFX_TEXTURE_MIN_POINT
         | BGFX_TEXTURE_MAG_POINT
         | BGFX_TEXTURE_MIP_POINT
         | BGFX_TEXTURE_U_CLAMP
         | BGFX_TEXTURE_V_CLAMP;

      // Create ShadowMap
      mShadowMap = bgfx::createTexture2D(mCascadeSize * 4, mCascadeSize, 1, bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT | BGFX_TEXTURE_COMPARE_LEQUAL);
      bgfx::TextureHandle fbtextures[] =
      {
         mShadowMap
      };
      mShadowMapBuffer = bgfx::createFrameBuffer(BX_COUNTOF(fbtextures), fbtextures);
   }

   void DirectionalLight::destroyBuffers()
   {
      // Destroy ShadowMap
      if (bgfx::isValid(mShadowMap))
         bgfx::destroyTexture(mShadowMap);

      // Destroy ShadowMap Buffer
      if (bgfx::isValid(mShadowMapBuffer))
         bgfx::destroyFrameBuffer(mShadowMapBuffer);
   }

   // TODO: Move this into Rendering or Camera?
   void DirectionalLight::worldSpaceFrustumCorners(F32* _corners24f, F32 _near, F32 _far, F32 _projWidth, F32 _projHeight, const F32* __restrict _invViewMtx)
   {
      // Define frustum corners in view space.
      const F32 nw = _near * _projWidth;
      const F32 nh = _near * _projHeight;
      const F32 fw = _far  * _projWidth;
      const F32 fh = _far  * _projHeight;

      const U8 numCorners = 8;
      const F32 corners[numCorners][3] =
      {
         { -nw,  nh, _near },
         { nw,  nh, _near },
         { nw, -nh, _near },
         { -nw, -nh, _near },
         { -fw,  fh, _far },
         { fw,  fh, _far },
         { fw, -fh, _far },
         { -fw, -fh, _far },
      };

      // Convert them to world space.
      F32(*out)[3] = (F32(*)[3])_corners24f;
      for (U8 ii = 0; ii < numCorners; ++ii)
      {
         bx::vec3MulMtxH((F32*)&out[ii], (F32*)&corners[ii], _invViewMtx);
      }
   }

   /**
   * _splits = { near0, far0, near1, far1... nearN, farN }
   * N = _numSplits
   */
   void DirectionalLight::splitFrustum(F32* _splits, U8 _numSplits, F32 _near, F32 _far, F32 _splitWeight)
   {
      const F32 logWeight = _splitWeight;
      const F32 ratio = _far / _near;
      const S8 numSlices = _numSplits * 2;
      const F32 numSlicesf = F32(numSlices);

      for (U8 nn = 2, ff = 1; nn < numSlices; nn += 2, ff += 2)
      {
         F32 step = F32(S8(ff)) / numSlicesf;
         F32 logSplit = _near * mPow(ratio, step);
         F32 linearSplit = _near + (_far - _near) * step;

         // Interpolate between log and linear split.
         _splits[nn] = mLerp(linearSplit, logSplit, mClampF(logWeight, 0.0f, 1.0f));
         _splits[ff] = _splits[nn];
      }

      // First and Last slice.
      _splits[0] = _near;
      _splits[numSlices - 1] = _far;
   }

   void DirectionalLight::refresh()
   {
      if (!mEnabled)
         return;

      // Used for forward rendering
      Rendering::setDirectionalLight(mDirection, mColor);
   }

   void DirectionalLight::preRender(Rendering::RenderCamera* camera)
   {
      
   }

   void DirectionalLight::render(Rendering::RenderCamera* camera)
   {
      if (!mEnabled)
         return;

      // Only render ShadowMaps for the camera we're attached to.
      if (camera == mCamera)
      {
         renderShadows();
      }

      // Set Uniforms
      bgfx::setUniform(Graphics::Shader::getUniformVec4("dirLightDirection"), Point4F(mDirection.x, mDirection.y, mDirection.z, 0.0f));
      bgfx::setUniform(Graphics::Shader::getUniformVec4("dirLightColor"), &mColor.red);
      bgfx::touch(0);

      if (!camera->getRenderPath()->hasLightBuffer())
         return;

      // Directional Light
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);

      // Normals, Material Info, Depth
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), camera->getRenderPath()->getNormalTexture());
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), camera->getRenderPath()->getMatInfoTexture());
      bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), camera->getRenderPath()->getDepthTexture());

      // ShadowMap Cascades
      bgfx::setTexture(3, Graphics::Shader::getTextureUniform(3), mShadowMap);

      // Draw Directional Light
      bgfx::setTransform(proj);
      bgfx::setState(0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad((F32)Rendering::canvasWidth, (F32)Rendering::canvasHeight);

      if (ShadowMapCascadeDebug::CascadeDebugEnabled)
         bgfx::submit(camera->getRenderPath()->getLightBufferView()->id, mDebugLightShader->mProgram);
      else
         bgfx::submit(camera->getRenderPath()->getLightBufferView()->id, mLightShader->mProgram);
   }

   void DirectionalLight::postRender(Rendering::RenderCamera* camera)
   {

   }

   void DirectionalLight::renderShadows()
   {
      // Settings
      bool m_stabilize = true;
      F32 m_near = mCamera->nearPlane;
      F32 m_far = mFarPlane;

      // Flip Y for OpenGL
      bgfx::RendererType::Enum renderer = bgfx::getRendererType();
      bool ogl = false
         || renderer == bgfx::RendererType::OpenGL
         || renderer == bgfx::RendererType::OpenGLES
         ;
      const float ymul = (ogl) ? 0.5f : -0.5f;

      float zadd = 0.5f;
      const float mtxBias[16] =
      {
         0.5f, 0.0f, 0.0f, 0.0f,
         0.0f, ymul, 0.0f, 0.0f,
         0.0f, 0.0f, 0.5f, 0.0f,
         0.5f, 0.5f, zadd, 1.0f,
      };

      // Setup light view mtx.
      const F32 eye[3] =
      {
         mDirection.x,
         mDirection.y,
         mDirection.z,
      };
      const F32 up[3] = { 0.0f, 0.0f, 1.0f };
      const F32 at[3] = { 0.0f, 0.0f, 0.0f };
      bx::mtxLookAt(mLightView, eye, at, up);

      // Compute camera inverse view mtx.
      F32 mtxViewInv[16];
      bx::mtxInverse(mtxViewInv, mCamera->viewMatrix);

      F32 splitSlices[8];
      splitFrustum(splitSlices, 4, m_near, m_far, mSplitDistribution);

      float mtxProj[16];
      bx::mtxOrtho(mtxProj, 1.0f, -1.0f, 1.0f, -1.0f, -m_far, m_far, 0.0f, ogl);

      const U8 numCorners = 8;
      float frustumCorners[4][numCorners][3];
      for (U8 ii = 0, nn = 0, ff = 1; ii < 4; ++ii, nn += 2, ff += 2)
      {
         // Compute frustum corners for one split in world space.
         worldSpaceFrustumCorners((F32*)frustumCorners[ii], splitSlices[nn], splitSlices[ff], mCamera->projectionWidth, mCamera->projectionHeight, mtxViewInv);

         float min[3] = { 9000.0f,  9000.0f,  9000.0f };
         float max[3] = { -9000.0f, -9000.0f, -9000.0f };

         for (U8 jj = 0; jj < numCorners; ++jj)
         {
            // Transform to light space.
            float lightSpaceFrustumCorner[3];
            bx::vec3MulMtxH(lightSpaceFrustumCorner, frustumCorners[ii][jj], mLightView);

            // Update bounding box.
            min[0] = bx::fmin(min[0], lightSpaceFrustumCorner[0]);
            max[0] = bx::fmax(max[0], lightSpaceFrustumCorner[0]);
            min[1] = bx::fmin(min[1], lightSpaceFrustumCorner[1]);
            max[1] = bx::fmax(max[1], lightSpaceFrustumCorner[1]);
            min[2] = bx::fmin(min[2], lightSpaceFrustumCorner[2]);
            max[2] = bx::fmax(max[2], lightSpaceFrustumCorner[2]);
         }

         F32 minproj[3];
         F32 maxproj[3];
         bx::vec3MulMtxH(minproj, min, mtxProj);
         bx::vec3MulMtxH(maxproj, max, mtxProj);

         F32 offsetx, offsety;
         F32 scalex, scaley;

         scalex = 2.0f / (maxproj[0] - minproj[0]);
         scaley = 2.0f / (maxproj[1] - minproj[1]);

         // Cascade Stabilization
         if (m_stabilize)
         {
            //scaley = scalex = (getMin(scalex, scaley));
            const F32 quantizer = 64.0f;
            scalex = quantizer / ceilf(quantizer / scalex);
            scaley = quantizer / ceilf(quantizer / scaley);
         }

         offsetx = 0.5f * (maxproj[0] + minproj[0]) * scalex;
         offsety = 0.5f * (maxproj[1] + minproj[1]) * scaley;

         // Cascade Stabilization
         if (m_stabilize)
         {
            const F32 halfSize = F32(mCascadeSize) * 0.5f;
            offsetx = ceilf(offsetx * halfSize) / halfSize;
            offsety = ceilf(offsety * halfSize) / halfSize;
         }

         F32 mtxCrop[16];
         bx::mtxIdentity(mtxCrop);
         mtxCrop[0] = scalex;
         mtxCrop[5] = scaley;
         mtxCrop[12] = offsetx;
         mtxCrop[13] = offsety;

         bx::mtxMul(mLightProj[ii], mtxCrop, mtxProj);

         float mtxTmp[16];
         bx::mtxMul(mtxTmp, mLightProj[ii], mtxBias);
         bx::mtxMul(mShadowMtx[ii], mLightView, mtxTmp); // lViewProjCropBias
      }

      // Parameters
      F32 shadowParams[4] = { mBias, mNormalOffset, mCascadeSize * 4.0f, 0.0f };
      bgfx::setUniform(mShadowParamsUniform, shadowParams, 1);

      // Shadow Map Cascade Matrices
      bgfx::setUniform(mShadowMtxUniform, mShadowMtx, 4);

      // Setup Cascades
      for (U32 i = 0; i < 4; ++i)
      {
         // Set Cascade View
         bgfx::setViewRect(mCascadeViews[i]->id, mCascadeSize * i, 0, mCascadeSize, mCascadeSize);
         bgfx::setViewFrameBuffer(mCascadeViews[i]->id, mShadowMapBuffer);
         bgfx::setViewTransform(mCascadeViews[i]->id, mLightView, mLightProj[i]);

         // Clear Cascade
         bgfx::setViewClear(mCascadeViews[i]->id
            , BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
            , 0x000000000
            , 1.0f
            , 0
            );
         bgfx::touch(mCascadeViews[i]->id);
      }

      U64 state = 0
         | BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
         | BGFX_STATE_DEPTH_WRITE
         | BGFX_STATE_DEPTH_TEST_LESS
         | BGFX_STATE_CULL_CW
         ;

      // Render each shadow casting object.
      Rendering::RenderData* item = Rendering::getRenderDataList();
      for (U32 n = 0; n < Rendering::getRenderDataCount(); ++n, ++item)
      {
         if (item->flags & Rendering::RenderData::Deleted
            || (item->flags & Rendering::RenderData::Hidden)
            || !(item->flags & Rendering::RenderData::CastShadow))
            continue;

         // Render to each of the 4 cascades.
         for (U32 i = 0; i < 4; ++i)
         {
            // Transform Table.
            bgfx::setTransform(item->transformTable, item->transformCount);

            // Buffers
            bgfx::setVertexBuffer(item->vertexBuffer);
            bgfx::setIndexBuffer(item->indexBuffer);

            // Set render states.
            bgfx::setState(state);

            // Submit primitive
            // We need to use a different shader if its a skinned mesh. 
            // Note: this may break down if a normal mesh uses more transforms
            //       for something.
            if (item->transformCount > 1)
               bgfx::submit(mCascadeViews[i]->id, mPCFSkinnedShader->mProgram);
            else
               bgfx::submit(mCascadeViews[i]->id, mPCFShader->mProgram);
         }
      }
   }

   // ----------------------------------------
   //   ShadowMapCascadeDebug : Displays the ShadowMap cascades for debugging purposes
   // ----------------------------------------

   IMPLEMENT_DEBUG_MODE("ShadowMapCascade", ShadowMapCascadeDebug);

   bool ShadowMapCascadeDebug::CascadeDebugEnabled = false;

   void ShadowMapCascadeDebug::onEnable()
   {
      CascadeDebugEnabled = true;
   }

   void ShadowMapCascadeDebug::onDisable()
   {
      CascadeDebugEnabled = false;
   }
}
