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

   DeferredRendering* getDeferredRendering()
   {
      return _deferredRenderingInst;
   }

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
      lightBuffer.idx = bgfx::invalidHandle; 

      dirLightShader = Graphics::getShader("deferred/dirlight_vs.sc", "deferred/dirlight_fs.sc");
      combineShader = Graphics::getShader("deferred/combine_vs.sc", "deferred/combine_fs.sc");

      // Load Ambient Cubemap ( TEMP )
      ambientCubemap.idx = bgfx::invalidHandle;
      TextureObject* ambientCubemapTex = TextureManager::loadTexture("pisa_lod.dds", TextureHandle::BitmapKeepTexture, false);
      if ( ambientCubemapTex != NULL )
         ambientCubemap = ambientCubemapTex->getBGFXTexture();
      u_ambientCube = Graphics::Shader::getUniform("u_ambientCube", bgfx::UniformType::Int1);

      ambientIrrCubemap.idx = bgfx::invalidHandle;
      TextureObject* ambientIrrCubemapTex = TextureManager::loadTexture("pisa_irr.dds", TextureHandle::BitmapKeepTexture, false);
      if ( ambientIrrCubemapTex != NULL )
         ambientIrrCubemap = ambientIrrCubemapTex->getBGFXTexture();
      u_ambientIrrCube = Graphics::Shader::getUniform("u_ambientIrrCube", bgfx::UniformType::Int1);

      // Get Views
      v_DeferredGeometry = Graphics::getView("DeferredGeometry", "RenderLayer0", true);
      v_DeferredLight = Graphics::getView("DeferredLight");
      v_RenderLayer0 = Graphics::getView("RenderLayer0");

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
      lightBuffer = bgfx::createFrameBuffer(canvasWidth, canvasHeight, bgfx::TextureFormat::BGRA8);
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
   }

   void DeferredRendering::preRender()
   {
      // G-Buffer
      bgfx::setClearColor(0, UINT32_C(0x00000000) );

      bgfx::setViewClear(v_DeferredGeometry->id
         , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
         , 1.0f
         , 0
         , 0
         , 0
         , 0
      );
      bgfx::setViewRect(v_DeferredGeometry->id, 0, 0, canvasWidth, canvasHeight);
      bgfx::setViewFrameBuffer(v_DeferredGeometry->id, gBuffer);
      bgfx::setViewTransform(v_DeferredGeometry->id, viewMatrix, projectionMatrix);
      bgfx::submit(v_DeferredGeometry->id);

      // Light Buffer
      bgfx::setViewClear(v_DeferredLight->id
         , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
         , 1.0f
         , 0
         , 0
      );
      bgfx::setViewRect(v_DeferredLight->id, 0, 0, canvasWidth, canvasHeight);
      bgfx::setViewFrameBuffer(v_DeferredLight->id, lightBuffer);
      bgfx::setViewTransform(v_DeferredLight->id, viewMatrix, projectionMatrix);
      bgfx::submit(v_DeferredLight->id);
   }

   void DeferredRendering::render()
   {
      // Directional Light
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);

      bgfx::setProgram(dirLightShader->mProgram);

      // Set Uniforms
      Point3F dir = Scene::directionalLightDir;
      bgfx::setUniform(Graphics::Shader::getUniformVec4("dirLightDirection"), Point4F(dir.x, dir.y, dir.z, 0.0f));
      bgfx::setUniform(Graphics::Shader::getUniformVec4("dirLightColor"), &Scene::directionalLightColor.red);
      bgfx::setUniform(Graphics::Shader::getUniformVec4("dirLightAmbient"), &Scene::directionalLightAmbient.red);

      // Color, Normals, Material Info, Depth
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getColorTexture());
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), Rendering::getNormalTexture());
      bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), Rendering::getMatInfoTexture());
      bgfx::setTexture(3, Graphics::Shader::getTextureUniform(3), Rendering::getDepthTexture());

      // ShadowMap Cascades
      bgfx::setTexture(4, Graphics::Shader::getTextureUniform(4), Rendering::getShadowMap(0));
      bgfx::setTexture(5, Graphics::Shader::getTextureUniform(5), Rendering::getShadowMap(1));
      bgfx::setTexture(6, Graphics::Shader::getTextureUniform(6), Rendering::getShadowMap(2));
      bgfx::setTexture(7, Graphics::Shader::getTextureUniform(7), Rendering::getShadowMap(3));

      // Ambient Cubemap, Ambient Irradience Cubemap
      bgfx::setTexture(8, u_ambientCube, ambientCubemap);
      bgfx::setTexture(9, u_ambientIrrCube, ambientIrrCubemap);

      // Draw Directional Light
      bgfx::setTransform(proj);
      bgfx::setState(0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad((F32)canvasWidth, (F32)canvasHeight);
      bgfx::submit(v_DeferredLight->id);
   }

   void DeferredRendering::postRender()
   {
      // This projection matrix is used because its a full screen quad.
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
      bgfx::setViewTransform(v_RenderLayer0->id, NULL, proj);
      bgfx::setViewRect(v_RenderLayer0->id, 0, 0, canvasWidth, canvasHeight);

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
      bgfx::submit(v_RenderLayer0->id);
   }
}
