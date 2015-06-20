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


#ifndef _POST_RENDERING_H_
#define _POST_RENDERING_H_

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
   class PostFX 
   {
      public:
         virtual void render() = 0;
   };

   class PostRendering : public virtual Renderable
   {
      protected:
         Graphics::Shader*          finalShader;
         Graphics::Shader*          finalFXAAShader;
         Graphics::ViewTableEntry*  v_Final;

      public:
         Vector<PostFX*> postFXList;

         PostRendering();
         ~PostRendering();

         virtual void preRender();
         virtual void render();
         virtual void postRender();
   };

   extern PostRendering* _postRenderingInst;
   void postInit();
   void postDestroy();
   void addPostFX(PostFX* fx);

   extern bgfx::FrameBufferHandle   _postBuffers[2];
   extern U32                       _postBufferIdx;
   bgfx::FrameBufferHandle          getPostSource();
   bgfx::FrameBufferHandle          getPostTarget();
   void                             flipPostBuffers();
}

#endif