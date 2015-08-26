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
#include "multiplyNode.h"
#include "multiplyNode_Binding.h"

namespace Scene 
{
   IMPLEMENT_CONOBJECT(MultiplyNode);

   MultiplyNode::MultiplyNode()
   {
      type = "Multiply";
      mInputASrc = StringTable->insert("");
      mInputBSrc = StringTable->insert("");
   }

   void MultiplyNode::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("InputASrc", TypeString, Offset(mInputASrc, MultiplyNode), "");
      addField("InputBSrc", TypeString, Offset(mInputBSrc, MultiplyNode), "");
   }

   void MultiplyNode::generateVertex(MaterialTemplate* matTemplate, ReturnType refType)
   {
      BaseNode* inputA = findNode(matTemplate, mInputASrc);
      inputA->generateVertex(matTemplate, refType);

      BaseNode* inputB = findNode(matTemplate, mInputBSrc);
      inputB->generateVertex(matTemplate, refType);

      char buf[256];

      switch(refType)
      {
         case ReturnFloat:
            dSprintf(buf, 256, "float mul_result = %s * %s;", 
               inputA->getVertexReference(matTemplate, refType), 
               inputB->getVertexReference(matTemplate, refType));
            break;
         case ReturnVec2:
            dSprintf(buf, 256, "vec2 mul_result = %s * %s;", 
               inputA->getVertexReference(matTemplate, refType), 
               inputB->getVertexReference(matTemplate, refType));
            break;
         case ReturnVec3:
            dSprintf(buf, 256, "vec3 mul_result = %s * %s;", 
               inputA->getVertexReference(matTemplate, refType), 
               inputB->getVertexReference(matTemplate, refType));
            break;
         case ReturnVec4:
            dSprintf(buf, 256, "vec4 mul_result = %s * %s;", 
               inputA->getVertexReference(matTemplate, refType), 
               inputB->getVertexReference(matTemplate, refType));
            break;
      }
      matTemplate->addVertexBody(buf);
   }

   const char* MultiplyNode::getVertexReference(MaterialTemplate* matTemplate, ReturnType refType)
   {
      return "mul_result";
   }

   void MultiplyNode::generatePixel(MaterialTemplate* matTemplate, ReturnType refType)
   {
      BaseNode* inputA = findNode(matTemplate, mInputASrc);
      inputA->generatePixel(matTemplate, refType);

      BaseNode* inputB = findNode(matTemplate, mInputBSrc);
      inputB->generatePixel(matTemplate, refType);

      char buf[256];

      switch(refType)
      {
         case ReturnFloat:
            dSprintf(buf, 256, "float mul_result = %s * %s;", 
               inputA->getPixelReference(matTemplate, refType), 
               inputB->getPixelReference(matTemplate, refType));
            break;
         case ReturnVec2:
            dSprintf(buf, 256, "vec2 mul_result = %s * %s;", 
               inputA->getPixelReference(matTemplate, refType), 
               inputB->getPixelReference(matTemplate, refType));
            break;
         case ReturnVec3:
            dSprintf(buf, 256, "vec3 mul_result = %s * %s;", 
               inputA->getPixelReference(matTemplate, refType), 
               inputB->getPixelReference(matTemplate, refType));
            break;
         case ReturnVec4:
            dSprintf(buf, 256, "vec4 mul_result = %s * %s;", 
               inputA->getPixelReference(matTemplate, refType), 
               inputB->getPixelReference(matTemplate, refType));
            break;
      }
      matTemplate->addPixelBody(buf);
   }

   const char* MultiplyNode::getPixelReference(MaterialTemplate* matTemplate, ReturnType refType)
   {
      return "mul_result";
   }
}