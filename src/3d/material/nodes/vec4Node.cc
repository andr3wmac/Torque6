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
#include "Vec4Node.h"
#include "Vec4Node_Binding.h"

namespace Scene 
{
   IMPLEMENT_CONOBJECT(Vec4Node);

   Vec4Node::Vec4Node()
   {
      type = "Vec4";
      mUniformName = StringTable->insert("");
      mValue.set(0.0f, 0.0f, 0.0f, 0.0f);
   }

   void Vec4Node::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("UniformName", TypeString, Offset(mUniformName, Vec4Node), "");
      addField("Value", TypePoint4F, Offset(mValue, Vec4Node), "");
   }

   void Vec4Node::generateVertex(MaterialTemplate* matTemplate, ReturnType refType)
   {
      generatePixel(matTemplate, refType);
   }

   const char* Vec4Node::getVertexReference(MaterialTemplate* matTemplate, ReturnType refType)
   {
      return getPixelReference(matTemplate, refType);
   }

   void Vec4Node::generatePixel(MaterialTemplate* matTemplate, ReturnType refType)
   {
      if ( dStrlen(mUniformName) > 0 )
      {
         Rendering::UniformData* uniform = matTemplate->uniforms.addUniform();
         uniform->count = 1;
         uniform->uniform = Graphics::Shader::getUniformVec4(mUniformName);
         uniform->setValue(mValue);

         char buf[256];
         dSprintf(buf, 256, "uniform vec4 %s;", mUniformName);
         matTemplate->addPixelHeader(buf);
      }
   }

   const char* Vec4Node::getPixelReference(MaterialTemplate* matTemplate, ReturnType refType)
   {
      if ( dStrlen(mUniformName) > 0 )
      {
         switch (refType)
         {
            case ReturnFloat:
               dSprintf(mReturnBuf, 64, "%s.r", mUniformName);
               break;
            case ReturnVec2:
               dSprintf(mReturnBuf, 64, "%s.rg", mUniformName);
               break;
            case ReturnVec3:
               dSprintf(mReturnBuf, 64, "%s.rgb", mUniformName);
               break;
            case ReturnVec4:
               dSprintf(mReturnBuf, 64, "%s", mUniformName);
               break;
         
            case ReturnName:
            default:
               dSprintf(mReturnBuf, 64, "%s", mUniformName);
               break;
         }
      } else {
         switch (refType)
         {
            case ReturnFloat:
               dSprintf(mReturnBuf, 64, "%f", mValue.x);
               break;
            case ReturnVec2:
               dSprintf(mReturnBuf, 64, "vec2(%f, %f)", mValue.x, mValue.y);
               break;
            case ReturnVec3:
               dSprintf(mReturnBuf, 64, "vec3(%f, %f, %f)", mValue.x, mValue.y, mValue.z);
               break;
            case ReturnVec4:
            case ReturnName:
            default:
               dSprintf(mReturnBuf, 64, "vec4(%f, %f, %f, %f)", mValue.x, mValue.y, mValue.z, mValue.w);
               break;
         }
      }

      return mReturnBuf;
   }
}