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

#include "physicsEngine.h"

#include "math/mMath.h"
#include <bx/timer.h>

namespace Physics 
{
   void* PhysicsEngine::smPhysicsExecuteMutex   = Mutex::createMutex();
   void* PhysicsEngine::smPhysicsFinishedMutex  = Mutex::createMutex();

   PhysicsEngine::PhysicsEngine()
   {
      mAccumulatorTime  = 0.0f;
      mStepSize         = 1.0f / 60.0f;

      mPreviousTime = (F64)( bx::getHPCounter()/F64(bx::getHPFrequency()) );

#ifdef TORQUE_MULTITHREAD
      // We lock this to cause the physics thread to block until we release it.
      Mutex::lockMutex(PhysicsThread::smPhysicsFinishedMutex);

      mPhysicsThread = new PhysicsThread(mStepSize, simulate);
      mPhysicsThread->start();
#endif

      mRunning = true;
      setProcessTicks(true);
   }

   PhysicsEngine::~PhysicsEngine()
   {
#ifdef TORQUE_MULTITHREAD
      Mutex::unlockMutex(PhysicsThread::smPhysicsExecuteMutex);
      Mutex::unlockMutex(PhysicsThread::smPhysicsFinishedMutex);
      SAFE_DELETE(mPhysicsThread);
#endif
   }

   void PhysicsEngine::processPhysics()
   {  
#ifdef TORQUE_MULTITHREAD
      // This will block until the physics thread is finished execution.
      if( Mutex::lockMutex(PhysicsThread::smPhysicsExecuteMutex) )
      {
         // Unlocking this will stop the physics thread from blocking at the
         // end of it's tick.
         Mutex::unlockMutex(PhysicsThread::smPhysicsFinishedMutex);

         // We're sycned, it's a great time for updates.
         update();

         // We're going to unlock this so the physics thread can 
         // execute while we get on with the rest of the frames.
         Mutex::unlockMutex(PhysicsThread::smPhysicsExecuteMutex);

         // And lock this so the physics thread will lock again at 
         // the end of execution of a time step.
         Mutex::lockMutex(PhysicsThread::smPhysicsFinishedMutex);
      }
#else
      while ( mAccumulatorTime >= mStepSize )
      {
         simulate(mStepSize);
         mAccumulatorTime -= mStepSize;
      }
      update();
#endif
   }

   PhysicsObject* PhysicsEngine::getPhysicsObject(void* _user)
   {
      return NULL;
   }

   void PhysicsEngine::deletePhysicsObject(PhysicsObject* _obj)
   {
      //
   }

   void PhysicsEngine::simulate(F32 dt)
   {
      //
   }

   void PhysicsEngine::update()
   {
      //
   }

   void PhysicsEngine::interpolateTick( F32 delta )
   {  
      //
   }

   void PhysicsEngine::processTick()
   {  
      //
   }

   void PhysicsEngine::advanceTime( F32 timeDelta )
   {  
      if ( !mRunning ) return;

      F64 time          = (F64)( bx::getHPCounter() / F64(bx::getHPFrequency()) );
      F64 dt            = (time - mPreviousTime);
      mPreviousTime     = time;
      mAccumulatorTime  += dt;

      if ( mAccumulatorTime >= mStepSize )
         processPhysics();
   }

   void PhysicsEngine::setRunning(bool value)
   {
      mRunning = value;
   }

   // Thread Safe Collision Event
   void PhysicsEvent::process(SimObject *object)
   {
      if ( !mObjA.onCollideDelegate.empty() )
         mObjA.onCollideDelegate(mObjB.user);

      if ( !mObjB.onCollideDelegate.empty() )
         mObjB.onCollideDelegate(mObjA.user);
   }
}