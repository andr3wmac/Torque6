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


#include "forwardRendering.h"
#include "console/consoleInternal.h"
#include "graphics/shaders.h"
#include "graphics/utilities.h"
#include "3d/scene/core.h"

#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   bgfx::TextureHandle forwardBufferTextures[2] = { BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE };
   bgfx::FrameBufferHandle forwardBuffer = BGFX_INVALID_HANDLE; 

   void forwardInit()
   {
      forwardInitBuffers();
   }

   void forwardDestroy()
   {
      forwardDestroyBuffers();
   }

   void forwardInitBuffers()
   {
      forwardDestroyBuffers();

      const U32 samplerFlags = 0
			| BGFX_TEXTURE_RT
			| BGFX_TEXTURE_MIN_POINT
			| BGFX_TEXTURE_MAG_POINT
			| BGFX_TEXTURE_MIP_POINT
			| BGFX_TEXTURE_U_CLAMP
			| BGFX_TEXTURE_V_CLAMP;

      forwardBufferTextures[0] = Rendering::finalTexture;
		forwardBufferTextures[1] = Rendering::getDepthTexture();
		forwardBuffer = bgfx::createFrameBuffer(BX_COUNTOF(forwardBufferTextures), forwardBufferTextures, false);
   }

   void forwardDestroyBuffers()
   {
      if ( bgfx::isValid(forwardBuffer) )
         bgfx::destroyFrameBuffer(forwardBuffer);
   }

   void forwardPreRender()
   {
      bgfx::setViewFrameBuffer(Graphics::ViewTable::Forward, forwardBuffer);

      // Setup Camera/View
      bgfx::setViewTransform(Graphics::ViewTable::Forward, Scene::viewMatrix, Scene::projectionMatrix);
      bgfx::setViewRect(Graphics::ViewTable::Forward, 0, 0, Scene::canvasWidth, Scene::canvasHeight);
   }

   void forwardPostRender()
   {

   }
}