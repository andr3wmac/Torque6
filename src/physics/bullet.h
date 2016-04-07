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
   // This is actually max objects per type. 
   // There are currently 4 types: Box, Sphere, Mesh, and Character.
   #define MAX_PHYSICS_OBJECTS 1024

   class BulletPhysicsObject
   {
      public:
         // Variables prefixed with underscore are not thread safe.
         btRigidBody*               _rigidBody;
         btDefaultMotionState*      _motionState;
         btDiscreteDynamicsWorld*   _world;

         BulletPhysicsObject() :
            _rigidBody(NULL),
            _motionState(NULL),
            _world(NULL)
         {
            //
         }

         virtual void initialize(btDiscreteDynamicsWorld* world) { }
         virtual void destroy() { }
         virtual void update() { }
   };

   class BulletPhysicsBox : public PhysicsBox, public BulletPhysicsObject
   {
      public:
         btBoxShape* _shape;

         BulletPhysicsBox();
         ~BulletPhysicsBox();

         virtual void initialize(btDiscreteDynamicsWorld* world);
         virtual void destroy();
         virtual void update();
   };

   class BulletPhysicsSphere : public PhysicsSphere, public BulletPhysicsObject
   {
      public:
         btSphereShape* _shape;

         BulletPhysicsSphere();
         ~BulletPhysicsSphere();

         virtual void initialize(btDiscreteDynamicsWorld* world);
         virtual void destroy();
         virtual void update();
   };

   class BulletPhysicsMesh : public PhysicsMesh, public BulletPhysicsObject
   {
      public:
         btTriangleMesh*            _mesh;
         btConvexTriangleMeshShape* _shape;

         BulletPhysicsMesh();
         ~BulletPhysicsMesh();

         virtual void initialize(btDiscreteDynamicsWorld* world);
         virtual void destroy();
         virtual void update();
   };

   class BulletPhysicsCharacter : public PhysicsCharacter, public BulletPhysicsObject
   {
      public:
         btCapsuleShape* _shape;

         BulletPhysicsCharacter();
         ~BulletPhysicsCharacter();

         virtual void initialize(btDiscreteDynamicsWorld* world);
         virtual void destroy();
         virtual void update();
   };

   class BulletPhysicsEngine : public PhysicsEngine
   {
      protected:
         btBroadphaseInterface*                 mBroadphase;
         btDiscreteDynamicsWorld*               mDynamicsWorld;
         btDefaultCollisionConfiguration*       mCollisionConfiguration;
         btCollisionDispatcher*                 mDispatcher;
         btSequentialImpulseConstraintSolver*   mSolver;

         BulletPhysicsBox                       mPhysicsBoxes[MAX_PHYSICS_OBJECTS];
         BulletPhysicsSphere                    mPhysicsSpheres[MAX_PHYSICS_OBJECTS];
         BulletPhysicsMesh                      mPhysicsMeshes[MAX_PHYSICS_OBJECTS];
         BulletPhysicsCharacter                 mPhysicsCharacters[MAX_PHYSICS_OBJECTS];

      public:
         BulletPhysicsEngine();
         ~BulletPhysicsEngine();

         virtual Vector<PhysicsObject*>   getPhysicsObjects();
         virtual PhysicsBox*              createPhysicsBox(Point3F position, Point3F rotation, Point3F scale, void* _user = NULL);
         virtual PhysicsSphere*           createPhysicsSphere(Point3F position, Point3F rotation, F32 radius, void* _user = NULL);
         virtual PhysicsMesh*             createPhysicsMesh(Point3F position, Point3F rotation, Point3F scale, const Graphics::MeshData& meshData, void* _user = NULL);
         virtual PhysicsCharacter*        createPhysicsCharacter(Point3F position, Point3F rotation, F32 radius, F32 height, void* _user = NULL);
         virtual void                     deletePhysicsObject(PhysicsObject* _obj);

         virtual void simulate(F32 dt);
         virtual void update();
   };
}

#endif // _PHYSICS_THREAD_H_
