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
#include "physicsThread.h"
#include "math/mMath.h"

#include <bx/timer.h>

namespace Physics 
{
   PhysicsThread::PhysicsThread(F32 _stepSize, simulateFunc _simulateFunction)
   {
      stepSize = _stepSize;
      simulate = _simulateFunction;
   }

   // This only executes if TORQUE_MULTITHREAD is defined.
   void PhysicsThread::run(void *arg)
   {
      while ( !shouldStop )
      {
         // This won't be available when we're not suppoesd to run.
         if( Mutex::lockMutex(PhysicsEngine::smPhysicsExecuteMutex) )
         {
            if ( simulate != NULL )
               simulate(stepSize);

            // Release execution mutex.
            Mutex::unlockMutex(PhysicsEngine::smPhysicsExecuteMutex);

            // This will block execution until the main thread 
            // obtains the smPhysicsExecuteMutex lock.
            if( Mutex::lockMutex(PhysicsEngine::smPhysicsFinishedMutex) )
               Mutex::unlockMutex(PhysicsEngine::smPhysicsFinishedMutex);
         }
      }
   }
}