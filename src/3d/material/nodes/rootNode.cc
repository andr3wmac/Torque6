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

#include "rootNode.h"

// bgfx/bx
#include <bgfx.h>
#include <bx/fpumath.h>

namespace Scene 
{
   IMPLEMENT_CONOBJECT(RootNode);

   RootNode::RootNode()
   {
      type = "Root";
      isRootNode = true;
   }

   void RootNode::generateVertex(MaterialTemplate* matTemplate, ReturnType refType)
   {
      Parent::generateVertex(matTemplate, refType);
      matTemplate->addVertexHeader("#include <torque6.sc>");
   }

   void RootNode::generatePixel(MaterialTemplate* matTemplate, ReturnType refType)
   {
      Parent::generatePixel(matTemplate, refType);
      matTemplate->addPixelHeader("#include <torque6.sc>");
   }
}