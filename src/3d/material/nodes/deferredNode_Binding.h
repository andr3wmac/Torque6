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

#include "c-interface/c-interface.h"

namespace Scene{
   extern "C"{
      DLL_PUBLIC DeferredNode* DeferredNodeCreateInstance()
      {
         return new DeferredNode();
      }

      DLL_PUBLIC const char* DeferredNodeGetColorSrc(DeferredNode* DeferredNode)
      {
         return CInterface::GetMarshallableString(DeferredNode->mColorSrc);
      }

      DLL_PUBLIC void DeferredNodeSetColorSrc(DeferredNode* DeferredNode, const char* src)
      {
         DeferredNode->mColorSrc = src;
      }

      DLL_PUBLIC const char* DeferredNodeGetNormalSrc(DeferredNode* DeferredNode)
      {
         return CInterface::GetMarshallableString(DeferredNode->mNormalSrc);
      }

      DLL_PUBLIC void DeferredNodeSetNormalSrc(DeferredNode* DeferredNode, const char* src)
      {
         DeferredNode->mNormalSrc = src;
      }

      DLL_PUBLIC const char* DeferredNodeGetMetallicSrc(DeferredNode* DeferredNode)
      {
         return CInterface::GetMarshallableString(DeferredNode->mMetallicSrc);
      }

      DLL_PUBLIC void DeferredNodeSetMetallicSrc(DeferredNode* DeferredNode, const char* src)
      {
         DeferredNode->mMetallicSrc = src;
      }

      DLL_PUBLIC const char* DeferredNodeGetRoughnessSrc(DeferredNode* DeferredNode)
      {
         return CInterface::GetMarshallableString(DeferredNode->mRoughnessSrc);
      }

      DLL_PUBLIC void DeferredNodeSetRoughnessSrc(DeferredNode* DeferredNode, const char* src)
      {
         DeferredNode->mRoughnessSrc = src;
      }

      DLL_PUBLIC const char* DeferredNodeGetWorldPosOffsetSrc(DeferredNode* DeferredNode)
      {
         return CInterface::GetMarshallableString(DeferredNode->mWorldPosOffsetSrc);
      }

      DLL_PUBLIC void DeferredNodeSetWorldPosOffsetSrc(DeferredNode* DeferredNode, const char* src)
      {
         DeferredNode->mWorldPosOffsetSrc = src;
      }
   }
}