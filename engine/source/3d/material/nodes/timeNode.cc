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
#include "timeNode.h"

namespace Scene 
{
   IMPLEMENT_CONOBJECT(TimeNode);

   TimeNode::TimeNode()
   {
      type = "Time";
      mMultiplier = 1.0f;
   }

   void TimeNode::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("Multiplier", TypeF32, Offset(mMultiplier, TimeNode), "");
   }

   void TimeNode::generateVertex(MaterialTemplate* matTemplate, ReturnType refType)
   {
      matTemplate->addVertexHeader("uniform vec4 u_time;");

      char timeOut[128];
      dSprintf(timeOut, 128, "    float time = u_time.x * %f;", mMultiplier);
      matTemplate->addVertexBody(timeOut);
   }

   const char* TimeNode::getVertexReference(MaterialTemplate* matTemplate, ReturnType refType)
   {
      return getPixelReference(matTemplate, refType);
   }

   void TimeNode::generatePixel(MaterialTemplate* matTemplate, ReturnType refType)
   {
      matTemplate->addPixelHeader("uniform vec4 u_time;");

      char timeOut[128];
      dSprintf(timeOut, 128, "    float time = u_time.x * %f;", mMultiplier);
      matTemplate->addPixelBody(timeOut);
   }

   const char* TimeNode::getPixelReference(MaterialTemplate* matTemplate, ReturnType refType)
   {
      switch (refType)
      {
         case ReturnFloat:
            dSprintf(mReturnBuf, 64, "time");
            break;
         case ReturnVec2:
            dSprintf(mReturnBuf, 64, "vec2(time, time)");
            break;
         case ReturnVec3:
            dSprintf(mReturnBuf, 64, "vec3(time, time, time)");
            break;
         case ReturnVec4:
            dSprintf(mReturnBuf, 64, "vec4(time, time, time, time)");
            break;
         
         case ReturnName:
         default:
            dSprintf(mReturnBuf, 64, "u_time");
            break;
      }

      return mReturnBuf;
   }
}