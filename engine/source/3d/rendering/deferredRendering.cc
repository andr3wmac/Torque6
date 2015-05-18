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


#include "deferredRendering.h"
#include "console/consoleInternal.h"
#include "graphics/shaders.h"
#include "graphics/dgl.h"
#include "3d/scene/core.h"
#include "3d/rendering/shadows.h"

#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   DeferredRendering* _deferredRenderingInst = NULL;

   void deferredInit()
   {
      if ( _deferredRenderingInst != NULL ) return;
      _deferredRenderingInst = new DeferredRendering();
   }

   void deferredDestroy()
   {
      SAFE_DELETE(_deferredRenderingInst);
   }

   DeferredRendering::DeferredRendering()
   {
      gBufferTextures[0].idx = bgfx::invalidHandle;
      gBufferTextures[1].idx = bgfx::invalidHandle;
      gBufferTextures[2].idx = bgfx::invalidHandle;
      gBufferTextures[3].idx = bgfx::invalidHandle;
      gBuffer.idx = bgfx::invalidHandle; 
      lightBufferTextures[0].idx = bgfx::invalidHandle;
      lightBufferTextures[1].idx = bgfx::invalidHandle;
      lightBuffer.idx = bgfx::invalidHandle; 

      dirLightShader = Graphics::getShader("deferred/dirlight_vs.sc", "deferred/dirlight_fs.sc");
      sceneInvViewMatUniform = Graphics::Shader::getUniform4x4Matrix("u_sceneInvViewMtx", 1);
      sceneViewMatUniform = Graphics::Shader::getUniform4x4Matrix("u_sceneViewMtx", 1);

      combineShader = Graphics::getShader("deferred/combine_vs.sc", "deferred/combine_fs.sc");

      // Load Ambient Cubemap ( TEMP )
      ambientCubemap.idx = bgfx::invalidHandle;
      TextureObject* ambientCubemapTex = TextureManager::loadTexture("pisa_lod.dds", TextureHandle::BitmapKeepTexture, false);
      if ( ambientCubemapTex != NULL )
         ambientCubemap = ambientCubemapTex->getBGFXTexture();
      u_ambientCube = Graphics::Shader::getUniform("u_ambientCube", bgfx::UniformType::Uniform1i);

      ambientIrrCubemap.idx = bgfx::invalidHandle;
      TextureObject* ambientIrrCubemapTex = TextureManager::loadTexture("pisa_irr.dds", TextureHandle::BitmapKeepTexture, false);
      if ( ambientIrrCubemapTex != NULL )
         ambientIrrCubemap = ambientIrrCubemapTex->getBGFXTexture();
      u_ambientIrrCube = Graphics::Shader::getUniform("u_ambientIrrCube", bgfx::UniformType::Uniform1i);

      initBuffers();

      setRendering(true);
   }

   DeferredRendering::~DeferredRendering()
   {
      destroyBuffers();
   }

   void DeferredRendering::initBuffers()
   {
      destroyBuffers();

      const U32 samplerFlags = 0
            | BGFX_TEXTURE_RT
            | BGFX_TEXTURE_MIN_POINT
            | BGFX_TEXTURE_MAG_POINT
            | BGFX_TEXTURE_MIP_POINT
            | BGFX_TEXTURE_U_CLAMP
            | BGFX_TEXTURE_V_CLAMP;

      // G-Buffer
      gBufferTextures[0] = bgfx::createTexture2D(canvasWidth, canvasHeight, 1, bgfx::TextureFormat::BGRA8, samplerFlags);
      gBufferTextures[1] = Rendering::getNormalTexture();
      gBufferTextures[2] = Rendering::getMatInfoTexture();
      gBufferTextures[3] = Rendering::getDepthTexture();
      gBuffer = bgfx::createFrameBuffer(BX_COUNTOF(gBufferTextures), gBufferTextures, false);

      // Light Buffer
      lightBuffer = bgfx::createFrameBuffer(canvasWidth, canvasHeight, bgfx::TextureFormat::BGRA8, samplerFlags);
   }

   void DeferredRendering::destroyBuffers()
   {
      // Destroy Frame Buffers
      if ( bgfx::isValid(gBuffer) )
         bgfx::destroyFrameBuffer(gBuffer);
      if ( bgfx::isValid(lightBuffer) )
         bgfx::destroyFrameBuffer(lightBuffer);

      // Destroy G-Buffer Color/Lighting Textures
      if ( bgfx::isValid(gBufferTextures[0]) )
         bgfx::destroyTexture(gBufferTextures[0]);
      if ( bgfx::isValid(gBufferTextures[0]) )
         bgfx::destroyTexture(gBufferTextures[1]);
   }

   void DeferredRendering::preRender()
   {
      // G-Buffer
      bgfx::setClearColor(0, UINT32_C(0x00000000) );

      bgfx::setViewClear(Graphics::DeferredGeometry
         , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
         , 1.0f
         , 0
         , 0
         , 0
         , 0
      );
      bgfx::setViewRect(Graphics::DeferredGeometry, 0, 0, canvasWidth, canvasHeight);
      bgfx::setViewFrameBuffer(Graphics::DeferredGeometry, gBuffer);
      bgfx::setViewTransform(Graphics::DeferredGeometry, viewMatrix, projectionMatrix);
      bgfx::submit(Graphics::DeferredGeometry);

      // Light Buffer
      bgfx::setViewClear(Graphics::DeferredLight
         , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
         , 1.0f
         , 0
         , 0
         , 0
      );
      bgfx::setViewRect(Graphics::DeferredLight, 0, 0, canvasWidth, canvasHeight);
      bgfx::setViewFrameBuffer(Graphics::DeferredLight, lightBuffer);
      bgfx::setViewTransform(Graphics::DeferredLight, viewMatrix, projectionMatrix);
      bgfx::submit(Graphics::DeferredLight);
   }

   void DeferredRendering::render()
   {
      // Directional Light
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);

      // Set Uniforms
      bgfx::setUniform(Graphics::Shader::getUniformVec3("dirLightDirection"), &Scene::directionalLightDir.x);
      bgfx::setUniform(Graphics::Shader::getUniformVec3("dirLightColor"), &Scene::directionalLightColor.red);
      bgfx::setUniform(Graphics::Shader::getUniformVec3("dirLightAmbient"), &Scene::directionalLightAmbient.red);
         
      float viewProjMtx[16];
      bx::mtxMul(viewProjMtx, Rendering::viewMatrix, Rendering::projectionMatrix);

      float invViewProjMtx[16];
      bx::mtxInverse(invViewProjMtx, viewProjMtx);

      bgfx::setUniform(sceneInvViewMatUniform, invViewProjMtx, 1);
      bgfx::setUniform(sceneViewMatUniform, Rendering::viewMatrix, 1);

      // Color, Normals, Material Info, Depth
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), gBuffer, 0);
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(0), gBuffer, 1);
      bgfx::setTexture(2, Graphics::Shader::getTextureUniform(1), gBuffer, 2);
      bgfx::setTexture(3, Graphics::Shader::getTextureUniform(2), gBuffer, 3);
      bgfx::setTexture(4, Graphics::Shader::getTextureUniform(3), Rendering::getShadowMap());

      if ( bgfx::isValid(ambientCubemap) )
         bgfx::setTexture(6, u_ambientCube, ambientCubemap);
      if ( bgfx::isValid(ambientIrrCubemap) )
         bgfx::setTexture(7, u_ambientIrrCube, ambientIrrCubemap);

      // Draw Directional Light
      bgfx::setTransform(proj);
      bgfx::setProgram(dirLightShader->mProgram);
      bgfx::setState(0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad((F32)canvasWidth, (F32)canvasHeight);
      bgfx::submit(Graphics::DeferredLight);
   }

   void DeferredRendering::postRender()
   {
      // This projection matrix is used because its a full screen quad.
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
      bgfx::setViewTransform(Graphics::RenderLayer0, NULL, proj);
      bgfx::setViewRect(Graphics::RenderLayer0, 0, 0, canvasWidth, canvasHeight);

      // Combine Color + Light
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), gBuffer, 0);
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), gBuffer, 2); // Material Info
      bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), lightBuffer, 0);
      bgfx::setProgram(combineShader->mProgram);

      bgfx::setState(0
         | BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
         );

      fullScreenQuad((F32)canvasWidth, (F32)canvasHeight);
      bgfx::submit(Graphics::RenderLayer0);
   }
}
