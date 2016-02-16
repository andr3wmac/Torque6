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
#include "scene/scene.h"
#include "rendering/renderCamera.h"

#include <bgfx/bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   Transparency::Transparency(RenderCamera* camera)
   {
      mInitialized   = false;
      mCamera        = camera;

      mBufferTextures[0].idx  = bgfx::invalidHandle;
      mBufferTextures[1].idx  = bgfx::invalidHandle;
      mBufferTextures[2].idx  = bgfx::invalidHandle;
      mBuffer.idx             = bgfx::invalidHandle;
      mTransparencyBufferView = NULL;
      mTransparencyFinalView  = NULL;
      mOITCombineShader       = NULL;
   }

   Transparency::~Transparency()
   {
      destroyBuffers();
   }

   void Transparency::initBuffers()
   {
      destroyBuffers();

      // Get Views
      mTransparencyBufferView = Graphics::getView("TransparencyBuffer", 3000);
      mTransparencyFinalView = Graphics::getView("TransparencyFinal");

      // Opaque + Transparency Combine Shader.
      mOITCombineShader = Graphics::getDefaultShader("rendering/oit_combine_vs.tsh", "rendering/oit_combine_fs.tsh");

      const U32 samplerFlags = 0
         | BGFX_TEXTURE_RT
         | BGFX_TEXTURE_MIN_POINT
         | BGFX_TEXTURE_MAG_POINT
         | BGFX_TEXTURE_MIP_POINT
         | BGFX_TEXTURE_U_CLAMP
         | BGFX_TEXTURE_V_CLAMP;

      // First texture contains color data, second is weighting for transparency.
      mBufferTextures[0]   = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, 1, bgfx::TextureFormat::RGBA16F, samplerFlags);
      mBufferTextures[1]   = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, 1, bgfx::TextureFormat::R8, samplerFlags);
      mBufferTextures[2]   = mCamera->getDepthTexture();
      mBuffer              = bgfx::createFrameBuffer(BX_COUNTOF(mBufferTextures), mBufferTextures, false);

      mInitialized = true;
   }

   void Transparency::destroyBuffers()
   {
      // Destroy Frame Buffers
      if (bgfx::isValid(mBuffer))
         bgfx::destroyFrameBuffer(mBuffer);

      // Destroy T-Buffer Textures
      if (bgfx::isValid(mBufferTextures[0]))
         bgfx::destroyTexture(mBufferTextures[0]);
      if (bgfx::isValid(mBufferTextures[1]))
         bgfx::destroyTexture(mBufferTextures[1]);
   }

   void Transparency::render(bgfx::FrameBufferHandle output)
   {
      if (!mInitialized)
         initBuffers();

      // Set clear color palette for index 0
      bgfx::setPaletteColor(0, 0.0f, 0.0f, 0.0f, 0.0f);

      // Set clear color palette for index 1
      bgfx::setPaletteColor(1, 0.0f, 0.0f, 0.0f, 1.0f);

      bgfx::setViewClear(mTransparencyBufferView->id
         , BGFX_CLEAR_COLOR
         , 1.0f // Depth
         , 0    // Stencil
         , 1    // FB texture 0, color palette 1
         , 0    // FB texture 1, color palette 1
         );

      bgfx::setViewClear(mTransparencyFinalView->id
         , BGFX_CLEAR_COLOR
         , 1.0f // Depth
         , 0    // Stencil
         , 0    // Color palette 0
         );

      bgfx::touch(mTransparencyBufferView->id);
      bgfx::touch(mTransparencyFinalView->id);

      bgfx::setViewFrameBuffer(mTransparencyBufferView->id, mBuffer);
      bgfx::setViewRect(mTransparencyBufferView->id, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);
      bgfx::setViewTransform(mTransparencyBufferView->id, mCamera->viewMatrix, mCamera->projectionMatrix);

      // Render blended results into PostSource, then the postfx system takes it from there.
      bgfx::setViewFrameBuffer(mTransparencyFinalView->id, output);

      // This projection matrix is used because its a full screen quad.
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
      bgfx::setViewTransform(mTransparencyFinalView->id, NULL, proj);
      bgfx::setViewRect(mTransparencyFinalView->id, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);

      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), mCamera->getBackBuffer(), 0);
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), mBufferTextures[0]);
      bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), mBufferTextures[1]);
      bgfx::setState(0
         | BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
         );
      fullScreenQuad((F32)Rendering::canvasWidth, (F32)Rendering::canvasHeight);
      bgfx::submit(mTransparencyFinalView->id, mOITCombineShader->mProgram);
   }
}
