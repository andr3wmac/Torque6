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
#include "lerpNode.h"
#include "lerpNode_Binding.h"

namespace Scene 
{
   IMPLEMENT_CONOBJECT(LerpNode);

   LerpNode::LerpNode()
   {
      type        = "Lerp";
      mInputASrc  = StringTable->insert("");
      mInputBSrc  = StringTable->insert("");
      mAmountSrc  = StringTable->insert("");
      mAmount     = 0.0f;
   }

   void LerpNode::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("InputASrc", TypeString, Offset(mInputASrc, LerpNode), "");
      addField("InputBSrc", TypeString, Offset(mInputBSrc, LerpNode), "");
      addField("AmountSrc", TypeString, Offset(mAmountSrc, LerpNode), "");
      addField("Amount", TypeF32, Offset(mAmount, LerpNode), "");
   }

   void LerpNode::generateVertex(MaterialTemplate* matTemplate, ReturnType refType)
   {
      BaseNode* inputA = findNode(matTemplate, mInputASrc);
      inputA->generateVertex(matTemplate, refType);

      BaseNode* inputB = findNode(matTemplate, mInputBSrc);
      inputB->generateVertex(matTemplate, refType);

      // Optional Amount Input
      BaseNode* inputAmount = findNode(matTemplate, mAmountSrc);
      char amountBuf[16];
      if (inputAmount == NULL)
      {
         dSprintf(amountBuf, 16, "%f", mAmount);
      }
      else
      {
         inputAmount->generateVertex(matTemplate, ReturnType::ReturnFloat);
         dSprintf(amountBuf, 16, "%s", inputAmount->getVertexReference(matTemplate, ReturnType::ReturnFloat));
      }

      char buf[256];

      switch(refType)
      {
         case ReturnFloat:
            dSprintf(buf, 256, "    float %s = mix(%s, %s, %s);", 
               getInternalName(),
               inputA->getVertexReference(matTemplate, refType), 
               inputB->getVertexReference(matTemplate, refType),
               amountBuf);
            break;
         case ReturnVec2:
            dSprintf(buf, 256, "    vec2 %s = mix(%s, %s, %s);", 
               getInternalName(),
               inputA->getVertexReference(matTemplate, refType), 
               inputB->getVertexReference(matTemplate, refType),
               amountBuf);
            break;
         case ReturnVec3:
            dSprintf(buf, 256, "    vec3 %s = mix(%s, %s, %s);", 
               getInternalName(),
               inputA->getVertexReference(matTemplate, refType), 
               inputB->getVertexReference(matTemplate, refType),
               amountBuf);
            break;
         case ReturnVec4:
            dSprintf(buf, 256, "    vec4 %s = mix(%s, %s, %s);", 
               getInternalName(),
               inputA->getVertexReference(matTemplate, refType), 
               inputB->getVertexReference(matTemplate, refType),
               amountBuf);
            break;
      }
      matTemplate->addVertexBody(buf);
   }

   const char* LerpNode::getVertexReference(MaterialTemplate* matTemplate, ReturnType refType)
   {
      return getInternalName();
   }

   void LerpNode::generatePixel(MaterialTemplate* matTemplate, ReturnType refType)
   {
      BaseNode* inputA = findNode(matTemplate, mInputASrc);
      if (inputA == NULL) return;
      inputA->generatePixel(matTemplate, refType);

      BaseNode* inputB = findNode(matTemplate, mInputBSrc);
      if (inputB == NULL) return;
      inputB->generatePixel(matTemplate, refType);

      // Optional Amount Input
      BaseNode* inputAmount = findNode(matTemplate, mAmountSrc);
      char amountBuf[16];
      if (inputAmount == NULL)
      {
         dSprintf(amountBuf, 16, "%f", mAmount);
      }
      else
      {
         inputAmount->generatePixel(matTemplate, ReturnType::ReturnFloat);
         dSprintf(amountBuf, 16, "%s", inputAmount->getPixelReference(matTemplate, ReturnType::ReturnFloat));
      }

      char buf[256];

      switch(refType)
      {
         case ReturnFloat:
            dSprintf(buf, 256, "    float %s = mix(%s, %s, %s);", 
               getInternalName(),
               inputA->getPixelReference(matTemplate, refType), 
               inputB->getPixelReference(matTemplate, refType),
               amountBuf);
            break;
         case ReturnVec2:
            dSprintf(buf, 256, "    vec2 %s = mix(%s, %s, %s);", 
               getInternalName(),
               inputA->getPixelReference(matTemplate, refType), 
               inputB->getPixelReference(matTemplate, refType),
               amountBuf);
            break;
         case ReturnVec3:
            dSprintf(buf, 256, "    vec3 %s = mix(%s, %s, %s);", 
               getInternalName(),
               inputA->getPixelReference(matTemplate, refType), 
               inputB->getPixelReference(matTemplate, refType),
               amountBuf);
            break;
         case ReturnVec4:
            dSprintf(buf, 256, "    vec4 %s = mix(%s, %s, %s);", 
               getInternalName(),
               inputA->getPixelReference(matTemplate, refType), 
               inputB->getPixelReference(matTemplate, refType),
               amountBuf);
            break;
      }
      matTemplate->addPixelBody(buf);
   }

   const char* LerpNode::getPixelReference(MaterialTemplate* matTemplate, ReturnType refType)
   {
      return getInternalName();
   }
}