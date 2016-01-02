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
#include "textureNode.h"
#include "textureNode_Binding.h"

namespace Scene 
{
   IMPLEMENT_CONOBJECT(TextureNode);

   TextureNode::TextureNode()
   {
      type = "Texture";
      mSlot = 0;
      mPremultiplyAlpha = false;
   }

   void TextureNode::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("Slot", TypeS32, Offset(mSlot, TextureNode), "");
      addField("PremultiplyAlpha", TypeBool, Offset(mPremultiplyAlpha, TextureNode), "");
   }

   void TextureNode::generateVertex(MaterialTemplate* matTemplate, ReturnType refType)
   {
      Parent::generateVertex(matTemplate, refType);
   }

   void TextureNode::generatePixel(MaterialTemplate* matTemplate, ReturnType refType)
   {
      Parent::generatePixel(matTemplate, refType);

      char sampler2DOut[128];
      dSprintf(sampler2DOut, 128, "SAMPLER2D(Texture%d, %d);", mSlot, mSlot);
      matTemplate->addPixelHeader(sampler2DOut);

      matTemplate->addPixelBody("    // Texture Node");
      char texture2DOut[128];
      dSprintf(texture2DOut, 128, "    vec4 Texture%dSample = toLinear(texture2D(Texture%d, v_texcoord0));", mSlot, mSlot);
      matTemplate->addPixelBody(texture2DOut);

      if ( mPremultiplyAlpha )
      {
         char premulOut[128];
         dSprintf(premulOut, 128, "    Texture%dSample.rgb = Texture%dSample.rgb * Texture%dSample.a;", mSlot, mSlot, mSlot);
         matTemplate->addPixelBody(premulOut);
      }

      matTemplate->addPixelBody("");
   }

   const char* TextureNode::getPixelReference(MaterialTemplate* matTemplate, ReturnType refType)
   {
      switch (refType)
      {
         case ReturnFloat:
            dSprintf(mReturnBuf, 64, "Texture%dSample.r", mSlot);
            break;
         case ReturnVec2:
            dSprintf(mReturnBuf, 64, "Texture%dSample.rg", mSlot);
            break;
         case ReturnVec3:
            dSprintf(mReturnBuf, 64, "Texture%dSample.rgb", mSlot);
            break;

         case ReturnName:
         case ReturnVec4:
         default:
            dSprintf(mReturnBuf, 64, "Texture%dSample", mSlot);
            break;
      }

      
      return mReturnBuf;
   }
}