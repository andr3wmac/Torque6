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
#include "3d/scene/core.h"
#include "3d/rendering/common.h"

namespace Scene
{
   IMPLEMENT_CONOBJECT(DirectionalLight);

   DirectionalLight::DirectionalLight()
   {
      mName = "Directional Light";

      mDeferredLightView = Graphics::getView("DeferredLight", 1500);
      mLightShader       = Graphics::getDefaultShader("features/directionalLight/dirlight_vs.sc", "features/directionalLight/dirlight_fs.sc");

      // ShadowMap size (per cascade)
      mCascadeSize = 2048;

      // Default Values
      for (U32 i = 0; i < 4; ++i)
      {
         mCascadeTextures[i].idx = bgfx::invalidHandle;
         mCascadeBuffers[i].idx  = bgfx::invalidHandle;
      }

      // Initialize shadowmap textures/buffers
      initBuffers();

      // Render to shadowmap shaders
      mPCFShader        = Graphics::getDefaultShader("features/directionalLight/pcf_vs.sc", "features/directionalLight/pcf_fs.sc");
      mPCFSkinnedShader = Graphics::getDefaultShader("features/directionalLight/pcf_skinned_vs.sc", "features/directionalLight/pcf_fs.sc");

      // ShadowMap cascade matricies
      mCascadeMtxUniforms[0] = bgfx::createUniform("u_cascadeMtx0", bgfx::UniformType::Mat4);
      mCascadeMtxUniforms[1] = bgfx::createUniform("u_cascadeMtx1", bgfx::UniformType::Mat4);
      mCascadeMtxUniforms[2] = bgfx::createUniform("u_cascadeMtx2", bgfx::UniformType::Mat4);
      mCascadeMtxUniforms[3] = bgfx::createUniform("u_cascadeMtx3", bgfx::UniformType::Mat4);

      // Get views for cascades
      mCascadeViews[0] = Graphics::getView("ShadowMap_Cascade0", 500);
      mCascadeViews[1] = Graphics::getView("ShadowMap_Cascade1");
      mCascadeViews[2] = Graphics::getView("ShadowMap_Cascade2");
      mCascadeViews[3] = Graphics::getView("ShadowMap_Cascade3");
   }

   DirectionalLight::~DirectionalLight()
   {
      for (U8 i = 0; i < 4; ++i)
         bgfx::destroyUniform(mCascadeMtxUniforms[i]);

      destroyBuffers();
   }

   void DirectionalLight::resize()
   {
      initBuffers();
   }

   void DirectionalLight::onActivate()
   {
      setRendering(true);
   }

   void DirectionalLight::onDeactivate()
   {
      setRendering(false);
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

      // Create 4 Cascades
      for (U32 i = 0; i < 4; ++i)
      {
         mCascadeTextures[i] = bgfx::createTexture2D(mCascadeSize, mCascadeSize, 1, bgfx::TextureFormat::D16, BGFX_TEXTURE_COMPARE_LEQUAL);
         bgfx::TextureHandle fbtextures[] =
         {
            mCascadeTextures[i]
         };
         mCascadeBuffers[i] = bgfx::createFrameBuffer(BX_COUNTOF(fbtextures), fbtextures);
      }
   }

   void DirectionalLight::destroyBuffers()
   {
      // Destroy Cascade Frame Buffers and Textures.
      for (U32 i = 0; i < 4; ++i)
      {
         if (bgfx::isValid(mCascadeBuffers[i]))
            bgfx::destroyFrameBuffer(mCascadeBuffers[i]);
         if (bgfx::isValid(mCascadeTextures[i]))
            bgfx::destroyTexture(mCascadeTextures[i]);
      }
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
         bx::vec3MulMtx((F32*)&out[ii], (F32*)&corners[ii], _invViewMtx);
      }
   }

   /**
   * _splits = { near0, far0, near1, far1... nearN, farN }
   * N = _numSplits
   */
   void DirectionalLight::splitFrustum(F32* _splits, U8 _numSplits, F32 _near, F32 _far, F32 _splitWeight)
   {
      const F32 l = _splitWeight;
      const F32 ratio = _far / _near;
      const S8 numSlices = _numSplits * 2;
      const F32 numSlicesf = F32(numSlices);

      // First slice.
      _splits[0] = _near;

      for (U8 nn = 2, ff = 1; nn < numSlices; nn += 2, ff += 2)
      {
         float si = F32(S8(ff)) / numSlicesf;

         const F32 nearp = l*(_near*powf(ratio, si)) + (1 - l)*(_near + (_far - _near)*si);
         _splits[nn] = nearp;          //near
         _splits[ff] = nearp * 1.005f; //far from previous split
      }

      // Last slice.
      _splits[numSlices - 1] = _far;
   }

   void DirectionalLight::refresh()
   {
      // Settings
      bool m_stabilize = true;
      F32 m_near = Rendering::nearPlane;
      F32 m_far = Rendering::farPlane;
      F32 m_splitDistribution = 0.8f;

      // Flip Y for OpenGL
      bgfx::RendererType::Enum renderer = bgfx::getRendererType();
      bool flipV = false
         || renderer == bgfx::RendererType::OpenGL
         || renderer == bgfx::RendererType::OpenGLES
         ;
      const float ymul = (flipV) ? 0.5f : -0.5f;

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
         Scene::directionalLightDir.x,
         Scene::directionalLightDir.y,
         Scene::directionalLightDir.z,
      };
      const F32 at[3] = { 0.0f, 0.0f, 0.0f };
      bx::mtxLookAt(mLightView, eye, at);

      // Compute camera inverse view mtx.
      F32 mtxViewInv[16];
      bx::mtxInverse(mtxViewInv, Rendering::viewMatrix);

      F32 splitSlices[8];
      splitFrustum(splitSlices, 4, m_near, m_far, m_splitDistribution);

      float mtxProj[16];
      bx::mtxOrtho(mtxProj, 1.0f, -1.0f, 1.0f, -1.0f, -m_far, m_far);

      const U8 numCorners = 8;
      float frustumCorners[4][numCorners][3];
      for (U8 ii = 0, nn = 0, ff = 1; ii < 4; ++ii, nn += 2, ff += 2)
      {
         // Compute frustum corners for one split in world space.
         worldSpaceFrustumCorners((F32*)frustumCorners[ii], splitSlices[nn], splitSlices[ff], Rendering::projectionWidth, Rendering::projectionHeight, mtxViewInv);

         float min[3] = { 1000.0f,  1000.0f,  1000.0f };
         float max[3] = { -1000.0f, -1000.0f, -1000.0f };

         for (U8 jj = 0; jj < numCorners; ++jj)
         {
            // Transform to light space.
            float lightSpaceFrustumCorner[3];
            bx::vec3MulMtx(lightSpaceFrustumCorner, frustumCorners[ii][jj], mLightView);

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
         bx::mtxMul(mCascadeMtx[ii], mLightView, mtxTmp); // lViewProjCropBias
      }
   }

   void DirectionalLight::preRender()
   {
      // TODO: This doesn't need to happen every frame.
      refresh();

      // Setup Cascades
      for (U32 i = 0; i < 4; ++i)
      {
         // Set Cascade View
         bgfx::setViewRect(mCascadeViews[i]->id, 0, 0, mCascadeSize, mCascadeSize);
         bgfx::setViewFrameBuffer(mCascadeViews[i]->id, mCascadeBuffers[i]);
         bgfx::setViewTransform(mCascadeViews[i]->id, mLightView, mLightProj[i]);

         // Clear Cascade
         bgfx::setViewClear(mCascadeViews[i]->id
            , BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
            , 0xfefefefe // blur fails on completely white regions
            , 1.0f
            , 0
            );
         bgfx::touch(mCascadeViews[i]->id);

         // ShadowMap Cascade Matrix
         bgfx::setUniform(mCascadeMtxUniforms[i], mCascadeMtx[i]);
      }
   }

   void DirectionalLight::render()
   {
      U64 state = 0
         | BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
         | BGFX_STATE_DEPTH_WRITE
         | BGFX_STATE_DEPTH_TEST_LESS
         | BGFX_STATE_CULL_CW
         | BGFX_STATE_MSAA
         ;

      // Render each shadow casting object.
      for (U32 n = 0; n < Rendering::renderCount; ++n)
      {
         Rendering::RenderData* item = &Rendering::renderList[n];
         if (item->flags & Rendering::RenderData::Deleted || !(item->flags & Rendering::RenderData::CastShadow)) continue;

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

      // Directional Light
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);

      // Set Uniforms
      Point3F dir = Scene::directionalLightDir;
      bgfx::setUniform(Graphics::Shader::getUniformVec4("dirLightDirection"), Point4F(dir.x, dir.y, dir.z, 0.0f));
      bgfx::setUniform(Graphics::Shader::getUniformVec4("dirLightColor"), &Scene::directionalLightColor.red);
      //bgfx::setUniform(Graphics::Shader::getUniformVec4("dirLightAmbient"), &Scene::directionalLightAmbient.red);

      // Normals, Material Info, Depth
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getNormalTexture());
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), Rendering::getMatInfoTexture());
      bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), Rendering::getDepthTexture());

      // ShadowMap Cascades
      bgfx::setTexture(3, Graphics::Shader::getTextureUniform(3), mCascadeTextures[0]);
      bgfx::setTexture(4, Graphics::Shader::getTextureUniform(4), mCascadeTextures[1]);
      bgfx::setTexture(5, Graphics::Shader::getTextureUniform(5), mCascadeTextures[2]);
      bgfx::setTexture(6, Graphics::Shader::getTextureUniform(6), mCascadeTextures[3]);

      // Draw Directional Light
      bgfx::setTransform(proj);
      bgfx::setState(0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad((F32)Rendering::canvasWidth, (F32)Rendering::canvasHeight);

      bgfx::submit(mDeferredLightView->id, mLightShader->mProgram);
   }

   void DirectionalLight::postRender()
   {

   }
}
