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

#include "platform/platform.h"
#include "console/console.h"
#include "sim/simBase.h"
#include "collisionThread.h"
#include "math/mMath.h"

Vector<CollisionObject> CollisionThread::smCollisionObjects;
void* CollisionThread::smCollisionObjectsMutex = Mutex::createMutex();

bool CollisionThread::lock()
{
   return Mutex::lockMutex(smCollisionObjectsMutex);
}

void CollisionThread::unlock()
{
   Mutex::unlockMutex(smCollisionObjectsMutex);
}

CollisionThread::CollisionThread()
{

}

void CollisionThread::run(void *arg)
{
   MutexHandle mh;
   while ( !shouldStop )
   {
      if( mh.lock( smCollisionObjectsMutex, true ) )
      {
         for (U32 a = 0; a < smCollisionObjects.size(); a++)
         {
            for(U32 b = 0; b < smCollisionObjects.size(); b++)
            {
               if ( a == b ) continue;

               if ( smCollisionObjects[a].worldBoundingBox.isOverlapped(smCollisionObjects[b].worldBoundingBox) )
               {
                  // Bounding Boxes Collide.
                  Sim::postEvent(Sim::getRootGroup(), new CollisionEvent(smCollisionObjects[a], smCollisionObjects[b]), -1);
               }
            }
         }

         mh.unlock();
      }

      Platform::sleep(100);
   }
}

void CollisionEvent::process(SimObject *object)
{
   if ( !mObjA.onCollideDelegate.empty() )
      mObjA.onCollideDelegate(mObjB);
}