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
#include "vec2Node.h"
#include "vec2Node_Binding.h"

namespace Materials
{
   IMPLEMENT_CONOBJECT(Vec2Node);

   Vec2Node::Vec2Node()
   {
      type = "Vec2";
      mUniformName = StringTable->insert("");
      mValue.set(0.0f, 0.0f);
   }

   void Vec2Node::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("UniformName", TypeString, Offset(mUniformName, Vec2Node), "");
      addField("Value", TypePoint3F, Offset(mValue, Vec2Node), "");
   }

   void Vec2Node::generateVertex(const MaterialGenerationSettings &settings, ReturnType refType, U32 flags)
   {
      generatePixel(settings, refType);
   }

   const char* Vec2Node::getVertexReference(const MaterialGenerationSettings &settings, ReturnType refType, U32 flags)
   {
      return getPixelReference(settings, refType);
   }

   void Vec2Node::generatePixel(const MaterialGenerationSettings &settings, ReturnType refType, U32 flags)
   {
      if ( dStrlen(mUniformName) > 0 )
      {
         MaterialTemplate* matTemplate = settings.matTemplate;

         Rendering::UniformData* uniform = matTemplate->uniforms.addUniform();
         uniform->count = 1;
         uniform->uniform = Graphics::Shader::getUniformVec4(mUniformName);
         uniform->setValue(mValue);

         matTemplate->addPixelHeader("uniform vec4 %s;", mUniformName);
      }
   }

   const char* Vec2Node::getPixelReference(const MaterialGenerationSettings &settings, ReturnType refType, U32 flags)
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
               dSprintf(mReturnBuf, 64, "vec3(%s.rg, 1.0)", mUniformName);
               break;
            case ReturnVec4:
               dSprintf(mReturnBuf, 64, "vec4(%s.rg, 0.0, 1.0)", mUniformName);
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
               dSprintf(mReturnBuf, 64, "vec3(%f, %f, 0.0)", mValue.x, mValue.y);
               break;
            case ReturnVec4:
            case ReturnName:
            default:
               dSprintf(mReturnBuf, 64, "vec4(%f, %f, 0.0, 1.0)", mValue.x, mValue.y);
               break;
         }
      }

      return mReturnBuf;
   }
}