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

#include "console/consoleTypes.h"
#include "sinNode.h"
#include "sinNode_Binding.h"

namespace Scene 
{
   IMPLEMENT_CONOBJECT(SinNode);

   SinNode::SinNode()
   {
      type = "Sin";
      mXSrc = StringTable->insert("");
   }

   void SinNode::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("XSrc", TypeString, Offset(mXSrc, SinNode), "");
   }

   void SinNode::generateVertex(MaterialTemplate* matTemplate, ReturnType refType)
   {
      BaseNode* xNode = findNode(matTemplate, mXSrc);
      xNode->generateVertex(matTemplate, ReturnFloat);

      char buf[256];
      dSprintf(buf, 256, "float sin_result = sin(%s);", xNode->getVertexReference(matTemplate, ReturnFloat));
      matTemplate->addVertexBody(buf);
   }

   const char* SinNode::getVertexReference(MaterialTemplate* matTemplate, ReturnType refType)
   {
      return getPixelReference(matTemplate, refType);
   }

   void SinNode::generatePixel(MaterialTemplate* matTemplate, ReturnType refType)
   {
      BaseNode* xNode = findNode(matTemplate, mXSrc);
      xNode->generatePixel(matTemplate, ReturnFloat);

      char buf[256];
      dSprintf(buf, 256, "float sin_result = sin(%s);", xNode->getPixelReference(matTemplate, ReturnFloat));
      matTemplate->addPixelBody(buf);
   }

   const char* SinNode::getPixelReference(MaterialTemplate* matTemplate, ReturnType refType)
   {
      switch (refType)
      {
         case ReturnFloat:
            dSprintf(mReturnBuf, 64, "sin_result");
            break;
         case ReturnVec2:
            dSprintf(mReturnBuf, 64, "vec2(sin_result, sin_result)");
            break;
         case ReturnVec3:
            dSprintf(mReturnBuf, 64, "vec3(sin_result, sin_result, sin_result)");
            break;
         case ReturnVec4:
            dSprintf(mReturnBuf, 64, "vec4(sin_result, sin_result, sin_result, sin_result)");
            break;
         
         case ReturnName:
         default:
            dSprintf(mReturnBuf, 64, "sin_result");
            break;
      }

      return mReturnBuf;
   }
}