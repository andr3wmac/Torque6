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

#include "math/mTransform.h"
#include "math/mMatrix.h"

#include <bx/fpumath.h>

void Transform::update()
{
   // Translate
   F32 T[16];
   bx::mtxTranslate(T, mPosition.x, mPosition.y, mPosition.z);

   // Rotate
   F32 R[16];
   if (mQuatRotation)
      bx::mtxQuat(R, &mRotationQuat.x);
   else
      bx::mtxRotateXYZ(R, mRotationEuler.x, mRotationEuler.y, mRotationEuler.z);

   // Scale
   F32 S[16];
   bx::mtxScale(S, mScale.x, mScale.y, mScale.z);

   // Transformation Matrix = T * R * S
   F32 temp1[16];
   bx::mtxMul(temp1, S, R);
   bx::mtxMul(matrix, temp1, T);
}

void Transform::set(const Point3F& pPos, const VectorF& pRotation, const VectorF& pScale)
{
   mPosition       = pPos;
   mQuatRotation   = false;
   mRotationEuler  = pRotation;
   mScale          = pScale;

   update();
}

void Transform::set(const Point3F& pPos, const QuatF& pRotation, const VectorF& pScale)
{
   mPosition       = pPos;
   mQuatRotation   = true;
   mRotationQuat   = pRotation;
   mScale          = pScale;

   update();
}

void Transform::setPosition(const Point3F& pPos)
{
   mPosition = pPos;
   update();
}

void Transform::setRotation(const VectorF& pRotation)
{
   mQuatRotation  = false;
   mRotationEuler = pRotation;
   update();
}

void Transform::setRotation(const QuatF& pRotation)
{
   mQuatRotation = true;
   mRotationQuat = pRotation;
   update();
}

void Transform::setScale(const VectorF& pScale)
{
   mScale = pScale;
   update();
}