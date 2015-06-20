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

#ifndef _BULLET_H_
#define _BULLET_H_

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

#ifndef _PHYSICS_ENGINE_H_
#include "physics/physicsEngine.h"
#endif

#ifndef BULLET_DYNAMICS_COMMON_H
#include <btBulletDynamicsCommon.h>
#endif

namespace Physics 
{
   class BulletPhysicsObject : public PhysicsObject
   {
      public:
         // Variables prefixed with underscore are not thread safe.
         btCollisionShape*       _shape;
         btRigidBody*            _rigidBody;
         btDefaultMotionState*   _motionState;

         BulletPhysicsObject();
         ~BulletPhysicsObject();

         virtual void initialize();
         virtual void destroy();
   };

   class BulletPhysicsEngine : public PhysicsEngine
   {
      protected:
         btBroadphaseInterface*                 mBroadphase;
         btDiscreteDynamicsWorld*               mDynamicsWorld;
         btDefaultCollisionConfiguration*       mCollisionConfiguration;
         btCollisionDispatcher*                 mDispatcher;
         btSequentialImpulseConstraintSolver*   mSolver;

         BulletPhysicsObject                    mPhysicsObjects[1024];

      public:
         BulletPhysicsEngine();
         ~BulletPhysicsEngine();

         virtual PhysicsObject* getPhysicsObject(void* _user = NULL);
         virtual void           deletePhysicsObject(PhysicsObject* _obj);
         virtual void simulate(F32 dt);
         virtual void update();
   };
}

#endif // _PHYSICS_THREAD_H_
