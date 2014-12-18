//-----------------------------------------------------------------------------
// Copyright (c) 2014 Andrew Mac
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
   bgfx::TextureHandle        deferredGBufferTextures[3] = { BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE };
	bgfx::FrameBufferHandle    deferredGBuffer = BGFX_INVALID_HANDLE; 
   bgfx::TextureHandle        deferredLightBufferTextures[2] = { BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE };
	bgfx::FrameBufferHandle    deferredLightBuffer = BGFX_INVALID_HANDLE; 
	bgfx::FrameBufferHandle    deferredFinalBuffer = BGFX_INVALID_HANDLE; 
   Graphics::Shader*          deferredCombineShader = NULL;

   void deferredInit()
   {
      deferredCombineShader = new Graphics::Shader("shaders/combine_vs.sc", "shaders/combine_fs.sc");
      deferredInitBuffers();
   }

   void deferredDestroy()
   {
      deferredDestroyBuffers();
      SAFE_DELETE(deferredCombineShader);
   }

   void deferredInitBuffers()
   {
      deferredDestroyBuffers();

      const U32 samplerFlags = 0
				| BGFX_TEXTURE_RT
				| BGFX_TEXTURE_MIN_POINT
				| BGFX_TEXTURE_MAG_POINT
				| BGFX_TEXTURE_MIP_POINT
				| BGFX_TEXTURE_U_CLAMP
				| BGFX_TEXTURE_V_CLAMP;

      // G-Buffer
		deferredGBufferTextures[0] = bgfx::createTexture2D(Scene::canvasWidth, Scene::canvasHeight, 1, bgfx::TextureFormat::BGRA8, samplerFlags);
		deferredGBufferTextures[1] = bgfx::createTexture2D(Scene::canvasWidth, Scene::canvasHeight, 1, bgfx::TextureFormat::BGRA8, samplerFlags);
		deferredGBufferTextures[2] = Rendering::getDepthTexture();
		deferredGBuffer = bgfx::createFrameBuffer(BX_COUNTOF(deferredGBufferTextures), deferredGBufferTextures, false);

      // Light Buffer
      deferredLightBuffer = bgfx::createFrameBuffer(Scene::canvasWidth, Scene::canvasHeight, bgfx::TextureFormat::BGRA8, samplerFlags);

      // Final Buffer
      deferredFinalBuffer = bgfx::createFrameBuffer(1, &Rendering::finalTexture, false);
   }

   void deferredDestroyBuffers()
   {
      // Destroy Frame Buffers
      if ( bgfx::isValid(deferredGBuffer) )
         bgfx::destroyFrameBuffer(deferredGBuffer);
      if ( bgfx::isValid(deferredLightBuffer) )
         bgfx::destroyFrameBuffer(deferredLightBuffer);
      if ( bgfx::isValid(deferredFinalBuffer) )
         bgfx::destroyFrameBuffer(deferredFinalBuffer);

      // Destroy G-Buffer Color/Lighting Textures
      if ( bgfx::isValid(deferredGBufferTextures[0]) )
         bgfx::destroyTexture(deferredGBufferTextures[0]);
      if ( bgfx::isValid(deferredGBufferTextures[0]) )
         bgfx::destroyTexture(deferredGBufferTextures[1]);
   }

   void deferredPreRender()
   {
      // G-Buffer
      bgfx::setViewClear(Graphics::ViewTable::DeferredGeometry
		   , BGFX_CLEAR_COLOR_BIT|BGFX_CLEAR_DEPTH_BIT
		   , 1.0f
		   , 0
		   , 1
		);
      bgfx::setViewRect(Graphics::ViewTable::DeferredGeometry, 0, 0, Scene::canvasWidth, Scene::canvasHeight);
      bgfx::setViewFrameBuffer(Graphics::ViewTable::DeferredGeometry, deferredGBuffer);
      bgfx::setViewTransform(Graphics::ViewTable::DeferredGeometry, Scene::viewMatrix, Scene::projectionMatrix);

      // Light Buffer
      bgfx::setViewClear(Graphics::ViewTable::DeferredLight
		   , BGFX_CLEAR_COLOR_BIT|BGFX_CLEAR_DEPTH_BIT
		   , 1.0f
		   , 0
		   , 1
		);
      bgfx::setViewRect(Graphics::ViewTable::DeferredLight, 0, 0, Scene::canvasWidth, Scene::canvasHeight);
      bgfx::setViewFrameBuffer(Graphics::ViewTable::DeferredLight, deferredLightBuffer);
      bgfx::setViewTransform(Graphics::ViewTable::DeferredLight, Scene::viewMatrix, Scene::projectionMatrix);

      // Final Buffer
      bgfx::setViewClear(Graphics::ViewTable::DeferredFinal
		   , BGFX_CLEAR_COLOR_BIT|BGFX_CLEAR_DEPTH_BIT
		   , 1.0f
		   , 0
		   , 1
		);
      bgfx::setViewRect(Graphics::ViewTable::DeferredFinal, 0, 0, Scene::canvasWidth, Scene::canvasHeight);
      bgfx::setViewFrameBuffer(Graphics::ViewTable::DeferredFinal, deferredFinalBuffer);
      bgfx::setViewTransform(Graphics::ViewTable::DeferredFinal, Scene::viewMatrix, Scene::projectionMatrix);
   }

   void deferredPostRender()
   {
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, (F32)Scene::canvasWidth, (float)Scene::canvasHeight, 0.0f, 0.0f, 1000.0f);
      bgfx::setViewTransform(Graphics::ViewTable::DeferredFinal, NULL, proj);
      bgfx::setViewRect(Graphics::ViewTable::DeferredFinal, 0, 0, Scene::canvasWidth, Scene::canvasHeight);

      // Combine Color + Light
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), deferredGBuffer, 0);
		bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), deferredLightBuffer, 0);
		bgfx::setProgram(deferredCombineShader->mProgram);

		bgfx::setState(0
			| BGFX_STATE_RGB_WRITE
			| BGFX_STATE_ALPHA_WRITE
			);

		dglScreenQuad(0, 0, Scene::canvasWidth, Scene::canvasHeight);
      bgfx::submit(Graphics::ViewTable::DeferredFinal);
   }
}