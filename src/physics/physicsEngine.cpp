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
      if ( !mObjA->mOnCollideDelegate.empty() )
         mObjA->mOnCollideDelegate(mObjB->mUser);

      if ( !mObjB->mOnCollideDelegate.empty() )
         mObjB->mOnCollideDelegate(mObjA->mUser);
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

   PhysicsDebug::PhysicsDebug()
   {
      for (U32 n = 0; n < 2048; ++n)
      {
         mObjectColors[n] = BGFXCOLOR_RGBA(gRandGen.randRangeI(0, 255), gRandGen.randRangeI(0, 255), gRandGen.randRangeI(0, 255), 255);
      }
   }

   void PhysicsDebug::render(Rendering::RenderCamera* camera)
   {
      ddPush();
      ddSetState(true, true, true);

      Vector<PhysicsObject*> physicsObjects = Physics::engine->getPhysicsObjects();
      for (U32 i = 0; i < physicsObjects.size(); ++i)
      {
         PhysicsObject* obj = physicsObjects[i];
         if (obj->mDeleted)
            continue;

         PhysicsBox* box = dynamic_cast<PhysicsBox*>(obj);
         if (box != NULL)
         {
            F32 transform[16];
            bx::mtxSRT(transform, box->getScale().x, box->getScale().y, box->getScale().z,
               box->getRotation().x, box->getRotation().y, box->getRotation().z,
               box->getPosition().x, box->getPosition().y, box->getPosition().z);

            Obb debugBox;
            dMemcpy(debugBox.m_mtx, transform, sizeof(transform));

            ddSetWireframe(false);
            ddSetColor(mObjectColors[i]);
            ddDraw(debugBox);

            continue;
         }

         PhysicsSphere* sphere = dynamic_cast<PhysicsSphere*>(obj);
         if (sphere != NULL)
         {
            Sphere debugSphere;
            debugSphere.m_center[0] = sphere->getPosition().x;
            debugSphere.m_center[1] = sphere->getPosition().y;
            debugSphere.m_center[2] = sphere->getPosition().z;
            debugSphere.m_radius = sphere->getRadius();

            ddSetWireframe(true);
            ddSetColor(mObjectColors[i]);
            ddDraw(debugSphere);

            continue;
         }

         PhysicsCharacter* character = dynamic_cast<PhysicsCharacter*>(obj);
         if (character != NULL)
         {
            Sphere debugSphereBottom;
            debugSphereBottom.m_center[0] = character->getPosition().x;
            debugSphereBottom.m_center[1] = character->getPosition().y;
            debugSphereBottom.m_center[2] = character->getPosition().z;
            debugSphereBottom.m_radius = character->getRadius();

            Sphere debugSphereTop;
            debugSphereTop.m_center[0] = character->getPosition().x;
            debugSphereTop.m_center[1] = character->getPosition().y;
            debugSphereTop.m_center[2] = character->getPosition().z + character->getHeight();
            debugSphereTop.m_radius = character->getRadius();

            ddSetWireframe(true);
            ddSetColor(mObjectColors[i]);
            //ddDraw(debugSphereBottom);
            //ddDraw(debugSphereTop);

            continue;
         }
      }

      ddPop();
   }
}