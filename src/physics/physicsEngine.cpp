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

#include "physics.h"
#include "physicsEngine.h"
#include "physicsThread.h"

#include "math/mMath.h"
#include <bx/timer.h>
#include <debugdraw/debugdraw.h>

namespace Physics 
{
   void* PhysicsEngine::smPhysicsExecuteMutex   = Mutex::createMutex();
   void* PhysicsEngine::smPhysicsFinishedMutex  = Mutex::createMutex();

   void physicsSimulate(F32 dt);

   PhysicsEngine::PhysicsEngine()
   {
      mAccumulatorTime  = 0.0f;
      mStepSize         = 1.0f / 60.0f;

      mPreviousTime = (F64)( bx::getHPCounter()/F64(bx::getHPFrequency()) );

#ifdef TORQUE_MULTITHREAD
      // We lock this to cause the physics thread to block until we release it.
      Mutex::lockMutex(smPhysicsFinishedMutex);

      mPhysicsThread = new PhysicsThread(mStepSize, physicsSimulate);
      mPhysicsThread->start();
#endif

      mRunning = true;
      setProcessTicks(true);
   }

   PhysicsEngine::~PhysicsEngine()
   {
#ifdef TORQUE_MULTITHREAD
      Mutex::unlockMutex(smPhysicsExecuteMutex);
      Mutex::unlockMutex(smPhysicsFinishedMutex);
      SAFE_DELETE(mPhysicsThread);
#endif
   }

   void PhysicsEngine::processPhysics()
   {  
#ifdef TORQUE_MULTITHREAD
      // This will block until the physics thread is finished execution.
      if( Mutex::lockMutex(smPhysicsExecuteMutex) )
      {
         // Unlocking this will stop the physics thread from blocking at the
         // end of it's tick.
         Mutex::unlockMutex(smPhysicsFinishedMutex);

         // We're sycned, it's a great time for updates.
         update();

         // We're going to unlock this so the physics thread can 
         // execute while we get on with the rest of the frames.
         Mutex::unlockMutex(smPhysicsExecuteMutex);

         // And lock this so the physics thread will lock again at 
         // the end of execution of a time step.
         Mutex::lockMutex(smPhysicsFinishedMutex);
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

   void physicsSimulate(F32 dt)
   {
      if ( engine != NULL )
         engine->simulate(dt);
   }

   // ----------------------------------------
   //   PhysicsDebug : Displays physics objects.
   // ----------------------------------------

   IMPLEMENT_DEBUG_MODE("Physics", PhysicsDebug);

   void PhysicsDebug::render(Rendering::RenderCamera* camera)
   {
      ddPush();
      ddSetColor(BGFXCOLOR_RGBA(0, 255, 0, 128));
      ddSetWireframe(true);
      ddSetState(true, true, true);

      Vector<PhysicsObject*> physicsObjects = Physics::engine->getPhysicsObjects();
      for (U32 i = 0; i < physicsObjects.size(); ++i)
      {
         PhysicsObject* obj = physicsObjects[i];
         if (obj->deleted)
            continue;

         Box3F bounds;
         bounds.minExtents.set(-1.0, -1.0, -1.0);
         bounds.maxExtents.set(1.0, 1.0, 1.0);

         MatrixF transform;
         bx::mtxSRT(transform, obj->getScale().x, obj->getScale().y, obj->getScale().z,
            obj->getRotation().x, obj->getRotation().y, obj->getRotation().z,
            obj->getPosition().x, obj->getPosition().y, obj->getPosition().z);

         bounds.transform(transform);

         Aabb box;
         box.m_min[0] = bounds.minExtents.x;
         box.m_min[1] = bounds.minExtents.y;
         box.m_min[2] = bounds.minExtents.z;
         box.m_max[0] = bounds.maxExtents.x;
         box.m_max[1] = bounds.maxExtents.y;
         box.m_max[2] = bounds.maxExtents.z;
         ddDraw(box);
      }

      ddPop();
   }
}