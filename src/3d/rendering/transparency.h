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


#ifndef _TRANSPARENCY_H_
#define _TRANSPARENCY_H_

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
   // Order Independant Transparency
   // Based On: https://github.com/bkaradzic/bgfx/blob/master/examples/19-oit/

   class OITransparency : public virtual Renderable
   {
      protected:
         Graphics::ViewTableEntry*  v_TransparencyBuffer;
         Graphics::ViewTableEntry*  v_TransparencyFinal;
         bgfx::TextureHandle        tBufferTextures[3];
         bgfx::FrameBufferHandle    tBuffer;
         Graphics::Shader*          oitCombineShader;

      public:
         OITransparency();
         ~OITransparency();

         virtual void preRender();
         virtual void render();
         virtual void postRender();
   };

   // Generic Transparency Functions
   extern OITransparency* _transparencyInst;
   void transparencyInit();
   void transparencyDestroy();
}

#endif