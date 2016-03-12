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
#include "cosNode.h"
#include "cosNode_Binding.h"

namespace Materials
{
   IMPLEMENT_CONOBJECT(CosNode);

   CosNode::CosNode()
   {
      type = "Cos";
      mXSrc = StringTable->insert("");
   }

   void CosNode::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("XSrc", TypeString, Offset(mXSrc, CosNode), "");
   }

   void CosNode::generateVertex(const MaterialGenerationSettings &settings, ReturnType refType)
   {
      MaterialTemplate* matTemplate = settings.matTemplate;

      BaseNode* xNode = findNode(settings, mXSrc);
      xNode->generateVertex(settings, ReturnFloat);

      matTemplate->addVertexBody("float %s = cos(%s);", getInternalName(), xNode->getVertexReference(settings, ReturnFloat));
   }

   const char* CosNode::getVertexReference(const MaterialGenerationSettings &settings, ReturnType refType)
   {
      return getPixelReference(settings, refType);
   }

   void CosNode::generatePixel(const MaterialGenerationSettings &settings, ReturnType refType)
   {
      MaterialTemplate* matTemplate = settings.matTemplate;

      BaseNode* xNode = findNode(settings, mXSrc);
      xNode->generatePixel(settings, ReturnFloat);

      matTemplate->addPixelBody("float %s = cos(%s);", getInternalName(), xNode->getPixelReference(settings, ReturnFloat));
   }

   const char* CosNode::getPixelReference(const MaterialGenerationSettings &settings, ReturnType refType)
   {
      StringTableEntry name = getInternalName();

      switch (refType)
      {
         case ReturnFloat:
            dSprintf(mReturnBuf, 64, "%s", name);
            break;
         case ReturnVec2:
            dSprintf(mReturnBuf, 64, "vec2(%s, %s)", name, name);
            break;
         case ReturnVec3:
            dSprintf(mReturnBuf, 64, "vec3(%s, %s, %s)", name, name, name);
            break;
         case ReturnVec4:
            dSprintf(mReturnBuf, 64, "vec4(%s, %s, %s, %s)", name, name, name, name);
            break;
         
         case ReturnName:
         default:
            dSprintf(mReturnBuf, 64, "%s", name);
            break;
      }

      return mReturnBuf;
   }
}