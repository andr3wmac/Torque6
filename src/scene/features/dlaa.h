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


#ifndef _DLAA_FEATURE_H_
#define _DLAA_FEATURE_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _RENDERING_H_
#include "rendering/rendering.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx/bgfx.h>
#endif

#ifndef _POST_RENDERING_H_
#include "rendering/postRendering.h"
#endif

namespace Scene
{
   // DLAA: Directionally Localized Anti-Aliasing
   // http://iryoku.com/aacourse/downloads/12-Anti-Aliasing-from-a-Different-Perspective-(DLAA).pdf

   class DLAA : public Rendering::PostRenderFeature
   {
      private:
         typedef Rendering::PostRenderFeature Parent;

      protected:
         Graphics::Shader*          mEdgeShader;
         Graphics::Shader*          mFinalShader;
         Graphics::ViewTableEntry*  mEdgeView;
         Graphics::ViewTableEntry*  mFinalView;

      public:
         DLAA();
         ~DLAA();

         virtual void onActivate();
         virtual void onDeactivate();
         virtual void render();

         DECLARE_CONOBJECT(DLAA);
   };
}

#endif