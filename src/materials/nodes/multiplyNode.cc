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

namespace Materials
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

   void MultiplyNode::generateVertex(const MaterialGenerationSettings &settings, ReturnType refType, U32 flags)
   {
      BaseNode* inputA = findNode(settings, mInputASrc);
      inputA->generateVertex(settings, refType);

      BaseNode* inputB = findNode(settings, mInputBSrc);
      inputB->generateVertex(settings, refType);

      char buf[256];

      switch(refType)
      {
         case ReturnFloat:
            dSprintf(buf, 256, "    float %s = %s * %s;", 
               getInternalName(),
               inputA->getVertexReference(settings, refType),
               inputB->getVertexReference(settings, refType));
            break;
         case ReturnVec2:
            dSprintf(buf, 256, "    vec2 %s = %s * %s;", 
               getInternalName(),
               inputA->getVertexReference(settings, refType),
               inputB->getVertexReference(settings, refType));
            break;
         case ReturnVec3:
            dSprintf(buf, 256, "    vec3 %s = %s * %s;", 
               getInternalName(),
               inputA->getVertexReference(settings, refType),
               inputB->getVertexReference(settings, refType));
            break;
         case ReturnVec4:
            dSprintf(buf, 256, "    vec4 %s = %s * %s;", 
               getInternalName(),
               inputA->getVertexReference(settings, refType),
               inputB->getVertexReference(settings, refType));
            break;
      }

      MaterialTemplate* matTemplate = settings.matTemplate;
      matTemplate->addVertexBody(buf);
   }

   const char* MultiplyNode::getVertexReference(const MaterialGenerationSettings &settings, ReturnType refType, U32 flags)
   {
      return getInternalName();
   }

   void MultiplyNode::generatePixel(const MaterialGenerationSettings &settings, ReturnType refType, U32 flags)
   {
      BaseNode* inputA = findNode(settings, mInputASrc);
      if (inputA == NULL) return;
      inputA->generatePixel(settings, refType);

      BaseNode* inputB = findNode(settings, mInputBSrc);
      if (inputB == NULL) return;
      inputB->generatePixel(settings, refType);

      char buf[256];

      switch(refType)
      {
         case ReturnFloat:
            dSprintf(buf, 256, "    float %s = %s * %s;", 
               getInternalName(),
               inputA->getPixelReference(settings, refType),
               inputB->getPixelReference(settings, refType));
            break;
         case ReturnVec2:
            dSprintf(buf, 256, "    vec2 %s = %s * %s;", 
               getInternalName(),
               inputA->getPixelReference(settings, refType),
               inputB->getPixelReference(settings, refType));
            break;
         case ReturnVec3:
            dSprintf(buf, 256, "    vec3 %s = %s * %s;", 
               getInternalName(),
               inputA->getPixelReference(settings, refType),
               inputB->getPixelReference(settings, refType));
            break;
         case ReturnVec4:
            dSprintf(buf, 256, "    vec4 %s = %s * %s;", 
               getInternalName(),
               inputA->getPixelReference(settings, refType),
               inputB->getPixelReference(settings, refType));
            break;
      }

      MaterialTemplate* matTemplate = settings.matTemplate;
      matTemplate->addPixelBody(buf);
   }

   const char* MultiplyNode::getPixelReference(const MaterialGenerationSettings &settings, ReturnType refType, U32 flags)
   {
      return getInternalName();
   }
}