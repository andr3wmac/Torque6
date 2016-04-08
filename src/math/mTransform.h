//-----------------------------------------------------------------------------
// Copyright (c) 2016 Andrew Mac
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

#ifndef _MTRANSFORM_H_
#define _MTRANSFORM_H_

#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

class DLL_PUBLIC Transform
{
   protected:
      bool     mQuatRotation;
      Point3F  mPosition;
      VectorF  mRotationEuler;
      QuatF    mRotationQuat;
      VectorF  mScale;

   public:
      MatrixF matrix;

      Transform()
      {
         mPosition      = Point3F(0.0f, 0.0f, 0.0f);
         mRotationEuler = VectorF(0.0f, 0.0f, 0.0f);
         mRotationQuat  = QuatF(0.0f, 0.0f, 0.0f, 1.0f);
         mScale         = VectorF(1.0f, 1.0f, 1.0f);
         mQuatRotation  = false;
      }

      void update();
      void set(const Point3F& pPos, const VectorF& pRotation, const VectorF& pScale);
      void set(const Point3F& pPos, const QuatF& pRotation, const VectorF& pScale);
      void setPosition(const Point3F& pPos);
      void setRotation(const VectorF& pRotation);
      void setRotation(const QuatF& pRotation);
      void setScale(const VectorF& pScale);

      Point3F getPosition()      const { return mPosition; }
      VectorF getRotationEuler() const { return mRotationEuler; }
      QuatF   getRotationQuat()  const { return mRotationQuat; }
      VectorF getScale()         const { return mScale; }
};

#endif
