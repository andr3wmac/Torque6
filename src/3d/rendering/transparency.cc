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

#include "transparency.h"
#include "console/consoleInternal.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "3d/scene/core.h"
#include "3d/rendering/postRendering.h"

#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   OITransparency* _transparencyInst = NULL;

   void transparencyInit()
   {
      if ( _transparencyInst != NULL ) return;
      _transparencyInst = new OITransparency();
   }

   void transparencyDestroy()
   {
      SAFE_DELETE(_transparencyInst);
   }

   OITransparency::OITransparency()
   {
      // Get Views
      v_TransparencyBuffer = Graphics::getView("TransparencyBuffer", "RenderLayer4");
      v_TransparencyFinal = Graphics::getView("TransparencyFinal");

      const U32 samplerFlags = 0
         | BGFX_TEXTURE_RT
         | BGFX_TEXTURE_MIN_POINT
         | BGFX_TEXTURE_MAG_POINT
         | BGFX_TEXTURE_MIP_POINT
         | BGFX_TEXTURE_U_CLAMP
         | BGFX_TEXTURE_V_CLAMP;

      // First texture contains color data, second is weighting for transparency.
      tBufferTextures[0] = bgfx::createTexture2D(canvasWidth, canvasHeight, 1, bgfx::TextureFormat::RGBA16F, samplerFlags);
		tBufferTextures[1] = bgfx::createTexture2D(canvasWidth, canvasHeight, 1, bgfx::TextureFormat::R16F,    samplerFlags);
      tBufferTextures[2] = Rendering::getDepthTexture();
		tBuffer = bgfx::createFrameBuffer(BX_COUNTOF(tBufferTextures), tBufferTextures, false);

      // Opaque + Transparency Combine Shader.
      oitCombineShader = Graphics::getShader("transparency/oit_combine_vs.sc", "transparency/oit_combine_fs.sc");

		// Set clear color palette for index 0
		bgfx::setClearColor(0, 0.0f, 0.0f, 0.0f, 0.0f);

		// Set clear color palette for index 1
		bgfx::setClearColor(1, 1.0f, 1.0f, 1.0f, 1.0f);

		bgfx::setViewClear(v_TransparencyBuffer->id
			, BGFX_CLEAR_COLOR
			, 1.0f // Depth
			, 0    // Stencil
			, 0    // FB texture 0, color palette 0
			, 1    // FB texture 1, color palette 1
			);

		bgfx::setViewClear(v_TransparencyFinal->id
			, BGFX_CLEAR_COLOR
			, 1.0f // Depth
			, 0    // Stencil
			, 0    // Color palette 0
			);

      bgfx::submit(v_TransparencyBuffer->id);
      bgfx::submit(v_TransparencyFinal->id);

      setRendering(true);
   }

   OITransparency::~OITransparency()
   {
      // Destroy Frame Buffers
      if ( bgfx::isValid(tBuffer) )
         bgfx::destroyFrameBuffer(tBuffer);

      // Destroy T-Buffer Textures
      if ( bgfx::isValid(tBufferTextures[0]) )
         bgfx::destroyTexture(tBufferTextures[0]);
      if ( bgfx::isValid(tBufferTextures[1]) )
         bgfx::destroyTexture(tBufferTextures[1]);
   }

   void OITransparency::preRender()
   {
      bgfx::setViewFrameBuffer(v_TransparencyBuffer->id, tBuffer);
      bgfx::setViewRect(v_TransparencyBuffer->id, 0, 0, canvasWidth, canvasHeight);
      bgfx::setViewTransform(v_TransparencyBuffer->id, viewMatrix, projectionMatrix);

      // Render blended results into PostSource, then the postfx system takes it from there.
      bgfx::setViewFrameBuffer(v_TransparencyFinal->id, getPostSource());
   }

   void OITransparency::render()
   {

   }

   void OITransparency::postRender()
   {
      // This projection matrix is used because its a full screen quad.
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
      bgfx::setViewTransform(v_TransparencyFinal->id, NULL, proj);
      bgfx::setViewRect(v_TransparencyFinal->id, 0, 0, canvasWidth, canvasHeight);

      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getColorTexture());
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), tBufferTextures[0]);
		bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), tBufferTextures[1]);
      bgfx::setProgram(oitCombineShader->mProgram);
		bgfx::setState(0
			| BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
			);
		fullScreenQuad((F32)canvasWidth, (F32)canvasHeight);
		bgfx::submit(v_TransparencyFinal->id);
   }
}
