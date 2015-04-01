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
      gBuffer.idx = bgfx::invalidHandle; 
      lightBufferTextures[0].idx = bgfx::invalidHandle;
      lightBufferTextures[1].idx = bgfx::invalidHandle;
      lightBuffer.idx = bgfx::invalidHandle; 

      combineShader = Graphics::getShader("combine_vs.sc", "combine_fs.sc");
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
      gBufferTextures[1] = bgfx::createTexture2D(canvasWidth, canvasHeight, 1, bgfx::TextureFormat::BGRA8, samplerFlags);
      gBufferTextures[2] = Rendering::getDepthTexture();
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

      bgfx::setViewClear(Graphics::ViewTable::DeferredGeometry
         , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
         , 1.0f
         , 0
         , 0
         , 0
      );
      bgfx::setViewRect(Graphics::ViewTable::DeferredGeometry, 0, 0, canvasWidth, canvasHeight);
      bgfx::setViewFrameBuffer(Graphics::ViewTable::DeferredGeometry, gBuffer);
      bgfx::setViewTransform(Graphics::ViewTable::DeferredGeometry, viewMatrix, projectionMatrix);
      bgfx::submit(Graphics::ViewTable::DeferredGeometry);

      // Light Buffer
      bgfx::setViewClear(Graphics::ViewTable::DeferredLight
         , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
         , 1.0f
         , 0
         , 0
         , 0
      );
      bgfx::setViewRect(Graphics::ViewTable::DeferredLight, 0, 0, canvasWidth, canvasHeight);
      bgfx::setViewFrameBuffer(Graphics::ViewTable::DeferredLight, lightBuffer);
      bgfx::setViewTransform(Graphics::ViewTable::DeferredLight, viewMatrix, projectionMatrix);
      bgfx::submit(Graphics::ViewTable::DeferredLight);
   }

   void DeferredRendering::render()
   {
      // For now the actual rendering is done with a single list in combinedRendering.cpp
   }

   void DeferredRendering::postRender()
   {
      // This projection matrix is used because its a full screen quad.
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
      bgfx::setViewTransform(Graphics::ViewTable::RenderLayer1, NULL, proj);
      bgfx::setViewRect(Graphics::ViewTable::RenderLayer1, 0, 0, canvasWidth, canvasHeight);

      // Combine Color + Light
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), gBuffer, 0);
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), lightBuffer, 0);
      bgfx::setProgram(combineShader->mProgram);

      bgfx::setState(0
         | BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
         | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
         );

      fullScreenQuad(canvasWidth, canvasHeight);
      bgfx::submit(Graphics::ViewTable::RenderLayer1);
   }
}