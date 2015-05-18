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

#ifndef _PHYSICS_ENGINE_H_
#define _PHYSICS_ENGINE_H_

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

#ifndef _SCENEENTITY_H_
#include "3d/entity/entity.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

#ifndef _PHYSICS_COMPONENT_H_
#include "3d/entity/components/physicsComponent.h"
#endif

// ------------------------------------------------------------------------------
//  How the Physics Engine works:
// ------------------------------------------------------------------------------
//
//   1) PhysicsEngine::InterpolateTick is called
//   2) If time passed is >= 1.0f/32.0f PhysicsEngine::processPhysics is called
//   3) If single-threaded, step is processed, physics is updated.
//   4) If multi-threaded:
//      a) Attempt to lock Execute mutex. The Physics Thread should be done by
//           now so this should be immediately available.
//      b) Unlock Finished mutex which lets the Physics Thread lock it.
//           The physics thread is now stuck trying to lock Execute again, but we 
//           hold that. This means we're synced and free to operate on the data.
//      c) Sync data between physics thread and main thread.
//      d) Unlock Execute mutex, letting the physics thread start it's next step.
//      e) Lock Finished mutex until the next time Physics Engine ticks.
//
// ------------------------------------------------------------------------------

namespace Physics 
{
   class PhysicsThread;

   // Shared data between main thread and physics thread.
   struct PhysicsObject
   {
      // Not safe to access from the physics thread.
      const char*                         typeStr;
      Scene::SceneEntity*                 entity;
      Scene::PhysicsComponent*            component;

      // Safe to access from the physics thread.
      bool                                updated;
      bool                                deleted;
      Box3F                               worldBoundingBox;

      Point3F                             position;
      Point3F                             velocity;
      Delegate<void(PhysicsObject hit)>   onCollideDelegate;

      PhysicsObject()
      {
         updated = false;
         entity = NULL;
         typeStr = "";
         deleted = true;

         velocity.set(0.0f, 0.0f, 0.0f);
         position.set(0.0f, 0.0f, 0.0f);

         onCollideDelegate.clear();
      }
   };

   // Occurs when objects collide.
   class PhysicsEvent : public SimEvent
   {
      PhysicsObject mObjA;
      PhysicsObject mObjB;

      public:
         PhysicsEvent(PhysicsObject objA, PhysicsObject objB)
         {
            mObjA = objA;
            mObjB = objB;
         }

         virtual void process(SimObject *object);
   };

   class PhysicsEngine : public virtual Tickable
   {
      PhysicsThread* mPhysicsThread;
      F64 mPreviousTime;
      F64 mAccumulatorTime;
      Vector<Scene::PhysicsComponent*> mComponents;
      bool mRunning;
      bool mDirty;

   public:
      PhysicsEngine();
      ~PhysicsEngine();

      void processPhysics();
      void updatePhysics();
      void addComponent(Scene::PhysicsComponent* comp);
      void removeComponent(Scene::PhysicsComponent* comp);

      void setRunning(bool value);

      virtual void interpolateTick( F32 delta );
      virtual void processTick();
      virtual void advanceTime( F32 timeDelta );

      // Static Variables
      static void*            smPhysicsExecuteMutex;
      static void*            smPhysicsFinishedMutex;
      static PhysicsObject    smPhysicsObjects[2048];
      static U32              smPhysicsObjectCount;
      static bool             smPhysicsObjectUpdated;
      static U32              smPhysicsSteps;

      // Static Functions
      static void             stepPhysics(F32 dt);
   };
}

#endif // _PHYSICS_THREAD_H_
