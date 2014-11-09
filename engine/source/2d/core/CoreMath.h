//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
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

#ifndef _CORE_MATH_H_
#define _CORE_MATH_H_

#ifndef _MRANDOM_H_
#include "math/mRandom.h"
#endif

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _PLATFORMGL_H_
#include "platform/platformGL.h"
#endif

#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

//-----------------------------------------------------------------------------

struct Vector2;

namespace CoreMath
{
extern RandomLCG gRandomGenerator;

/// Random Float Range.
inline F32 mGetRandomF( F32 from, F32 to ) { return gRandomGenerator.randRangeF( from, to ); }

/// Random Float.
inline F32 mGetRandomF( void ) { return gRandomGenerator.randF(); }

/// Random Integer Range.
inline S32 mGetRandomI( const S32 from, const S32 to ) { return gRandomGenerator.randRangeI( from, to ); }

/// Random Integer.
inline S32 mGetRandomI( void ) { return gRandomGenerator.randI(); }

/// Returns a point on the given line AB that is closest to 'point'.
Vector2 mGetClosestPointOnLine( Vector2 &a, Vector2 &b, Vector2 &point);

/// Calculate point in rectangle.
bool mPointInRectangle( const Vector2& point, const Vector2& rectMin, const Vector2& rectMax );

/// Calculate line/rectangle intersection.
bool mLineRectangleIntersect( const Vector2& startPoint, const Vector2& endPoint, const Vector2& rectMin, const Vector2& rectMax, F32* pTime = NULL );

} // Namespace CoreMath.

#endif // _CORE_UTILITY_H_
