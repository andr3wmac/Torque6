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


#ifndef _DEFERREDRENDERING_H_
#define _DEFERREDRENDERING_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _RENDERINGCOMMON_H_
#include "common.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx.h>
#endif

#ifndef _RENDERABLE_H_
#include <3d/rendering/renderable.h>
#endif

namespace Rendering 
{
   class DeferredRendering : public virtual Renderable
   {
      protected:
         bgfx::TextureHandle        gBufferTextures[4];
         bgfx::FrameBufferHandle    gBuffer; 
         bgfx::FrameBufferHandle    lightBuffer;
         Graphics::Shader*          combineShader; 
         Graphics::Shader*          dirLightShader;

         Graphics::ViewTableEntry*  v_DeferredGeometry;
         Graphics::ViewTableEntry*  v_DeferredLight;
         Graphics::ViewTableEntry*  v_RenderLayer0;

         void initBuffers();
         void destroyBuffers();

      public:
         // Lighting - Ambient Cubemap
         bgfx::UniformHandle u_ambientCube;
         bgfx::TextureHandle ambientCubemap;
         bgfx::UniformHandle u_ambientIrrCube;
         bgfx::TextureHandle ambientIrrCubemap;

         DeferredRendering();
         ~DeferredRendering();

         virtual void preRender();
         virtual void render();
         virtual void postRender();
   };

   extern DeferredRendering* _deferredRenderingInst;
   DeferredRendering* getDeferredRendering();
   void deferredInit();
   void deferredDestroy();
}

#endif