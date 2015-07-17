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

#include "bullet.h"

#include "math/mMath.h"
#include <bx/timer.h>

namespace Physics 
{
   // --------------------------------------
   // Physics Object
   // --------------------------------------

   BulletPhysicsObject::BulletPhysicsObject()
   {
      _shape         = NULL;
      _motionState   = NULL;
      _rigidBody     = NULL;
   }

   BulletPhysicsObject::~BulletPhysicsObject()
   {
      destroy();
   }

   void BulletPhysicsObject::initialize()
   {
      _shape = new btBoxShape(btVector3(mScale.x / 2.0f, mScale.y / 2.0f, mScale.z / 2.0f));

      if ( mStatic )
      {
         btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(0, NULL, _shape);
         _rigidBody = new btRigidBody(fallRigidBodyCI);
         _rigidBody->setUserIndex(1);
         _rigidBody->setUserPointer(this);
         _rigidBody->setAngularFactor(btVector3(0,0,0));
         _rigidBody->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(mPosition.x, mPosition.y, mPosition.z)));
      } else {
         _motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(mPosition.x, mPosition.y, mPosition.z)));
         btScalar mass = 1.0f;
         btVector3 fallInertia(0, 0, 0);
         _shape->calculateLocalInertia(mass, fallInertia);

         btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, _motionState, _shape, fallInertia);
         _rigidBody = new btRigidBody(fallRigidBodyCI);
         _rigidBody->setUserIndex(1);
         _rigidBody->setUserPointer(this);
         //_rigidBody->setAngularFactor(btVector3(0,1,0));
      }

      //_rigidBody->setCollisionFlags( btCollisionObject::CF_KINEMATIC_OBJECT );
      //_rigidBody->setActivationState( DISABLE_DEACTIVATION );
      initialized = true;
   }

   void BulletPhysicsObject::destroy()
   {
      if ( !initialized )
         return;

      SAFE_DELETE(_shape);
      SAFE_DELETE(_rigidBody);
      SAFE_DELETE(_motionState);

      initialized = false;
   }

   // --------------------------------------
   // Physics Engine
   // --------------------------------------

   BulletPhysicsEngine::BulletPhysicsEngine()
   {  
      mBroadphase             = new btDbvtBroadphase();
      mCollisionConfiguration = new btDefaultCollisionConfiguration();
      mDispatcher             = new btCollisionDispatcher(mCollisionConfiguration);
      mSolver                 = new btSequentialImpulseConstraintSolver;
      mDynamicsWorld          = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);

      // Gravity
      mDynamicsWorld->setGravity(btVector3(0, -98.1, 0));
   }

   BulletPhysicsEngine::~BulletPhysicsEngine()
   {
      for (U32 i = 0; i < 1024; ++i)
      {
         BulletPhysicsObject* obj = &mPhysicsObjects[i];
         if ( obj->initialized )
            mDynamicsWorld->removeRigidBody(obj->_rigidBody);
      }

      SAFE_DELETE(mDynamicsWorld);
      SAFE_DELETE(mSolver);
      SAFE_DELETE(mCollisionConfiguration);
      SAFE_DELETE(mDispatcher);
      SAFE_DELETE(mBroadphase);
   }

   PhysicsObject* BulletPhysicsEngine::getPhysicsObject(void* _user)
   {
      for (U32 i = 0; i < 1024; ++i)
      {
         if ( mPhysicsObjects[i].deleted )
         {
            mPhysicsObjects[i].deleted = false;
            mPhysicsObjects[i].user = _user;
            return &mPhysicsObjects[i];
         }
      }
      return NULL;
   }

   void BulletPhysicsEngine::deletePhysicsObject(PhysicsObject* _obj)
   {
      for (U32 i = 0; i < 1024; ++i)
      {
         if ( &mPhysicsObjects[i] == _obj )
         {
            mPhysicsObjects[i].shouldBeDeleted = true;
            return;
         }
      }
   }

   void BulletPhysicsEngine::simulate(F32 dt)
   {
      if ( mDynamicsWorld == NULL ) return;

      // Step Physics Simulation
      mDynamicsWorld->stepSimulation(dt, 10);

      // Detect Collisions
      int numManifolds = mDynamicsWorld->getDispatcher()->getNumManifolds();
      for (int i = 0; i < numManifolds; i++)
      {
         btPersistentManifold* contactManifold = mDynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
         S32 indexA = contactManifold->getBody0()->getUserIndex();
         S32 indexB = contactManifold->getBody1()->getUserIndex();
         if ( indexA == 1 && indexB == 1 )
         {
            Physics::PhysicsObject* objA = (Physics::PhysicsObject*)contactManifold->getBody0()->getUserPointer();
            Physics::PhysicsObject* objB = (Physics::PhysicsObject*)contactManifold->getBody1()->getUserPointer();
            Sim::postEvent(Sim::getRootGroup(), new PhysicsEvent(*objA, *objB), -1);
         }
      } 
   }

   void BulletPhysicsEngine::update()
   {
      for (U32 i = 0; i < 1024; ++i)
      {
         BulletPhysicsObject* obj = &mPhysicsObjects[i];
         if ( obj->deleted )
            continue;

         if ( obj->shouldBeDeleted )
         {
            if ( obj->initialized )
            {
               mDynamicsWorld->removeRigidBody(obj->_rigidBody);
               obj->destroy();
            }

            obj->deleted = true;
            continue;
         }
      }

      for (U32 i = 0; i < 1024; ++i)
      {
         BulletPhysicsObject* obj = &mPhysicsObjects[i];
         if ( obj->deleted )
            continue;

         if ( !obj->initialized )
         {
            obj->initialize();
            mDynamicsWorld->addRigidBody(obj->_rigidBody);
         } else {
            // Pull updates from Physics thread.
            btMotionState* objMotion = obj->_rigidBody->getMotionState();
            if ( objMotion )
            {
               btTransform trans;
               objMotion->getWorldTransform(trans);

               F32 mat[16];
               trans.getOpenGLMatrix(mat);

               obj->mPosition.set(mat[12], mat[13], mat[14]);
               btQuaternion rot = trans.getRotation();
               obj->mRotation.set(QuatToEuler(rot.x(), rot.y(), rot.z(), rot.w()));
            }

            // Apply actions from Game thread.
            while ( obj->mPhysicsActions.size() > 0 )
            {
               Physics::PhysicsAction action = obj->mPhysicsActions.front();

               switch(action.actionType)
               {
                  case Physics::PhysicsAction::setPosition:
                     obj->mPosition = action.vector3Value;
                     obj->_rigidBody->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1),btVector3(action.vector3Value.x, action.vector3Value.y, action.vector3Value.z)));
                     obj->_rigidBody->activate();
                     break;

                  case Physics::PhysicsAction::setLinearVelocity:
                     obj->_rigidBody->setLinearVelocity(btVector3(action.vector3Value.x * 25.0f, action.vector3Value.y * 25.0f, action.vector3Value.z * 25.0f));
                     obj->_rigidBody->activate();
                     break;
               }

               obj->mPhysicsActions.pop_front();
            }
         }
      }
   }
}