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

#ifndef _COLLISION_H_
#define _COLLISION_H_

#ifndef _PLATFORM_THREADS_THREAD_H_
#include "platform/threads/thread.h"
#endif

#ifndef _SIM_EVENT_H_
#include "sim/simEvent.h"
#endif

#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

#ifndef _UTIL_DELEGATE_H_
#include "delegates/delegate.h"
#endif

//----------------------------------------------------------------------------
// Process collision on a separate thread.
//----------------------------------------------------------------------------

// Structure containing information nessicary to 
// test collision.
struct CollisionObject
{
   Box3F worldBoundingBox;
   Delegate<void(CollisionObject hit)> onCollideDelegate;

   CollisionObject()
   {
      onCollideDelegate.clear();
   }
};

// Threaded collision checking.
class CollisionThread : public Thread
{
public:
   CollisionThread();

   virtual void run(void *arg = 0);

   static bool lock();
   static void unlock();
   static void* smCollisionObjectsMutex;
   static Vector<CollisionObject> smCollisionObjects;
};

// Occurs when objects collide.
class CollisionEvent : public SimEvent
{
   CollisionObject mObjA;
   CollisionObject mObjB;

public:
   CollisionEvent(CollisionObject objA, CollisionObject objB)
   {
      mObjA = objA;
      mObjB = objB;
   }

   virtual void process(SimObject *object);
};

#endif // _COLLISION_THREAD_H_
