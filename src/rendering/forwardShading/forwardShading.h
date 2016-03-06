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


#ifndef _FORWARD_SHADING_H_
#define _FORWARD_SHADING_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _RENDERING_H_
#include "rendering/rendering.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx/bgfx.h>
#endif

#ifndef _RENDER_CAMERA_H_
#include "rendering/renderCamera.h"
#endif

namespace Rendering 
{
   class ForwardShading : public RenderPath
   {
      protected:
         // BackBuffer
         bgfx::FrameBufferHandle    mBackBuffer;
         bgfx::TextureHandle        mBackBufferTexture;
         bgfx::TextureHandle        mDepthBuffer;
         bgfx::TextureHandle        mDepthBufferRead;
         Graphics::ViewTableEntry*  mBackBufferView;
         S32                        mForwardMaterialVariantIndex;

         void init();
         void destroy();

      public:
         Graphics::Shader*          mDefaultShader;

         ForwardShading(RenderCamera* camera);
         ~ForwardShading();

         virtual void preRender();
         virtual void render();
         virtual void postRender();
         virtual void resize();

         // Render Targets
         bgfx::FrameBufferHandle getBackBuffer();
         bgfx::TextureHandle     getDepthTexture();
         bgfx::TextureHandle     getDepthTextureRead();

         DECLARE_RENDER_PATH("ForwardShading", ForwardShading);
   };
}

#endif // _FORWARD_SHADING_H_