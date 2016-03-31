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
#include "memory/safeDelete.h"

#include "bullet.h"

#include "math/mMath.h"
#include <bx/timer.h>

namespace Physics 
{
   // --------------------------------------
   // Physics Box
   // --------------------------------------

   BulletPhysicsBox::BulletPhysicsBox()
   {
      _shape         = NULL;
      _motionState   = NULL;
      _rigidBody     = NULL;
   }

   BulletPhysicsBox::~BulletPhysicsBox()
   {
      destroy();
   }

   void BulletPhysicsBox::initialize(btDiscreteDynamicsWorld* world)
   {
      _shape = new btBoxShape(btVector3(mScale.x, mScale.y, mScale.z));
      
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

      world->addRigidBody(_rigidBody);
      mInitialized = true;
   }

   void BulletPhysicsBox::destroy()
   {
      if ( !mInitialized )
         return;

      SAFE_DELETE(_shape);
      SAFE_DELETE(_rigidBody);
      SAFE_DELETE(_motionState);

      mInitialized = false;
   }

   void BulletPhysicsBox::update()
   {
      // Pull updates from Physics thread.
      btMotionState* objMotion = _rigidBody->getMotionState();
      if (objMotion)
      {
         btTransform trans;
         objMotion->getWorldTransform(trans);

         F32 mat[16];
         trans.getOpenGLMatrix(mat);

         mPosition = Point3F(mat[12], mat[13], mat[14]);
         btQuaternion rot = trans.getRotation();
         mRotation = Point3F(QuatToEuler(rot.x(), rot.y(), rot.z(), rot.w()));
      }

      // Apply actions from Game thread.
      while (mPhysicsActions.size() > 0)
      {
         Physics::PhysicsAction action = mPhysicsActions.front();

         switch (action.actionType)
         {
            case Physics::PhysicsAction::setPosition:
               mPosition = action.vector3Value;
               _rigidBody->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(action.vector3Value.x, action.vector3Value.y, action.vector3Value.z)));
               _rigidBody->activate();
               break;

            case Physics::PhysicsAction::setLinearVelocity:
               _rigidBody->setLinearVelocity(btVector3(action.vector3Value.x, action.vector3Value.y, action.vector3Value.z));
               _rigidBody->activate();
               break;
         }

         mPhysicsActions.pop_front();
      }
   }

   // --------------------------------------
   // Physics Sphere
   // --------------------------------------

   BulletPhysicsSphere::BulletPhysicsSphere()
   {
      _shape = NULL;
      _motionState = NULL;
      _rigidBody = NULL;
   }

   BulletPhysicsSphere::~BulletPhysicsSphere()
   {
      destroy();
   }

   void BulletPhysicsSphere::initialize(btDiscreteDynamicsWorld* world)
   {
      _shape = new btSphereShape(mRadius);

      if (mStatic)
      {
         btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(0, NULL, _shape);
         _rigidBody = new btRigidBody(fallRigidBodyCI);
         _rigidBody->setUserIndex(1);
         _rigidBody->setUserPointer(this);
         _rigidBody->setAngularFactor(btVector3(0, 0, 0));
         _rigidBody->setRestitution(1.0f);
         _rigidBody->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(mPosition.x, mPosition.y, mPosition.z)));
      }
      else {
         _motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(mPosition.x, mPosition.y, mPosition.z)));
         btScalar mass = 1.0f;
         btVector3 fallInertia(0, 0, 0);
         _shape->calculateLocalInertia(mass, fallInertia);

         btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, _motionState, _shape, fallInertia);
         _rigidBody = new btRigidBody(fallRigidBodyCI);
         _rigidBody->setUserIndex(1);
         _rigidBody->setUserPointer(this);
         _rigidBody->setRestitution(1.0f);
         //_rigidBody->setAngularFactor(btVector3(0,1,0));
      }

      //_rigidBody->setCollisionFlags( btCollisionObject::CF_KINEMATIC_OBJECT );
      //_rigidBody->setActivationState( DISABLE_DEACTIVATION );

      world->addRigidBody(_rigidBody);
      mInitialized = true;
   }

   void BulletPhysicsSphere::destroy()
   {
      if (!mInitialized)
         return;

      SAFE_DELETE(_shape);
      SAFE_DELETE(_rigidBody);
      SAFE_DELETE(_motionState);

      mInitialized = false;
   }

   void BulletPhysicsSphere::update()
   {
      // Pull updates from Physics thread.
      btMotionState* objMotion = _rigidBody->getMotionState();
      if (objMotion)
      {
         btTransform trans;
         objMotion->getWorldTransform(trans);

         F32 mat[16];
         trans.getOpenGLMatrix(mat);

         mPosition = Point3F(mat[12], mat[13], mat[14]);
         btQuaternion rot = trans.getRotation();
         mRotation = Point3F(QuatToEuler(rot.x(), rot.y(), rot.z(), rot.w()));
      }

      // Apply actions from Game thread.
      while (mPhysicsActions.size() > 0)
      {
         Physics::PhysicsAction action = mPhysicsActions.front();

         switch (action.actionType)
         {
            case Physics::PhysicsAction::setPosition:
               mPosition = action.vector3Value;
               _rigidBody->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(action.vector3Value.x, action.vector3Value.y, action.vector3Value.z)));
               _rigidBody->activate();
               break;

            case Physics::PhysicsAction::setLinearVelocity:
               _rigidBody->setLinearVelocity(btVector3(action.vector3Value.x, action.vector3Value.y, action.vector3Value.z));
               _rigidBody->activate();
               break;
         }

         mPhysicsActions.pop_front();
      }
   }

   // --------------------------------------
   // Physics Character
   // --------------------------------------

   BulletPhysicsCharacter::BulletPhysicsCharacter()
   {
      _shape = NULL;
      _motionState = NULL;
      _rigidBody = NULL;
   }

   BulletPhysicsCharacter::~BulletPhysicsCharacter()
   {
      destroy();
   }

   void BulletPhysicsCharacter::initialize(btDiscreteDynamicsWorld* world)
   {
      _world = world;
      _shape = new btCapsuleShape(mRadius, mHeight);

      if (mStatic)
      {
         btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(0, NULL, _shape);
         _rigidBody = new btRigidBody(fallRigidBodyCI);
         _rigidBody->setUserIndex(1);
         _rigidBody->setUserPointer(this);
         _rigidBody->setAngularFactor(btVector3(0, 0, 0));
         _rigidBody->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(mPosition.x, mPosition.y, mPosition.z)));
      }
      else {
         _motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(mPosition.x, mPosition.y, mPosition.z)));
         btScalar mass = 80.7f;
         btVector3 fallInertia(0, 0, 0);
         _shape->calculateLocalInertia(mass, fallInertia);

         btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, _motionState, _shape, fallInertia);
         _rigidBody = new btRigidBody(fallRigidBodyCI);
         _rigidBody->setUserIndex(1);
         _rigidBody->setUserPointer(this);
         _rigidBody->setAngularFactor(btVector3(0,0,1));
      }

      //_rigidBody->setCollisionFlags( btCollisionObject::CF_KINEMATIC_OBJECT );
      //_rigidBody->setActivationState( DISABLE_DEACTIVATION );

      world->addRigidBody(_rigidBody);
      mInitialized = true;
   }

   void BulletPhysicsCharacter::destroy()
   {
      if (!mInitialized)
         return;

      SAFE_DELETE(_shape);
      SAFE_DELETE(_rigidBody);
      SAFE_DELETE(_motionState);

      mInitialized = false;
   }

   void BulletPhysicsCharacter::update()
   {
      // Pull updates from Physics thread.
      btMotionState* objMotion = _rigidBody->getMotionState();
      if (objMotion)
      {
         btTransform trans;
         objMotion->getWorldTransform(trans);

         F32 mat[16];
         trans.getOpenGLMatrix(mat);

         mPosition = Point3F(mat[12], mat[13], mat[14]);
         btQuaternion rot = trans.getRotation();
         mRotation = Point3F(QuatToEuler(rot.x(), rot.y(), rot.z(), rot.w()));
      }

      // Ray cast to find ground.
      btVector3 rayStart(mPosition.x, mPosition.y, mPosition.z);
      btVector3 rayEnd(mPosition.x, mPosition.y, mPosition.z - 10);
      btCollisionWorld::AllHitsRayResultCallback rayResult(rayStart, rayEnd);
      _world->rayTest(rayStart, rayEnd, rayResult);

      bool inAir = true;
      Point3F springForce(0.0f, 0.0f, 0.0f);
      for (S32 i = 0; i < rayResult.m_hitPointWorld.size(); i++)
      {
         if (rayResult.m_collisionObjects[i] != _rigidBody)
         {
            btVector3 a       = rayResult.m_hitPointWorld[i];
            Point3F hitPoint  = Point3F(a.x(), a.y(), a.z());
            Point3F dif       = mPosition - hitPoint;

            F32 groundDistance = dif.len();
            if (groundDistance < 1.25)
            {
               F32 springStiffness = 400.0f;
               springForce = springStiffness * Point3F(0.0f, 0.0f, 1.0f) * getMax(mHeight - groundDistance, -0.05f);
               inAir = false;
               break;
            }
         }
      }

      // Apply spring force to keep capsule against ground.
      _rigidBody->applyCentralForce(btVector3(springForce.x, springForce.y, springForce.z));

      // Apply actions from Game thread.
      while (mPhysicsActions.size() > 0)
      {
         Physics::PhysicsAction action = mPhysicsActions.front();
         const btVector3 vel = _rigidBody->getLinearVelocity();

         switch (action.actionType)
         {
         case Physics::PhysicsAction::setPosition:
            mPosition = action.vector3Value;
            _rigidBody->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(action.vector3Value.x, action.vector3Value.y, action.vector3Value.z)));
            _rigidBody->activate();
            break;

         case Physics::PhysicsAction::setLinearVelocity:
            _rigidBody->setLinearVelocity(btVector3(action.vector3Value.x, action.vector3Value.y, vel.getZ()));
            _rigidBody->activate();
            break;

         case Physics::PhysicsAction::applyForce:
            _rigidBody->applyCentralForce(btVector3(action.vector3Value.x, action.vector3Value.y, action.vector3Value.z));
            _rigidBody->activate();
            break;
         }

         mPhysicsActions.pop_front();
      }
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
      mDynamicsWorld->setGravity(btVector3(0.0f, 0.0f, -9.81f));
   }

   BulletPhysicsEngine::~BulletPhysicsEngine()
   {
      for (U32 i = 0; i < 1024; ++i)
      {
         BulletPhysicsBox* box = &mPhysicsBoxes[i];
         if (box->mInitialized )
            mDynamicsWorld->removeRigidBody(box->_rigidBody);

         BulletPhysicsSphere* sphere = &mPhysicsSpheres[i];
         if (sphere->mInitialized)
            mDynamicsWorld->removeRigidBody(sphere->_rigidBody);

         BulletPhysicsCharacter* character = &mPhysicsCharacters[i];
         if (character->mInitialized)
            mDynamicsWorld->removeRigidBody(character->_rigidBody);
      }

      SAFE_DELETE(mDynamicsWorld);
      SAFE_DELETE(mSolver);
      SAFE_DELETE(mCollisionConfiguration);
      SAFE_DELETE(mDispatcher);
      SAFE_DELETE(mBroadphase);
   }

   Vector<PhysicsObject*> BulletPhysicsEngine::getPhysicsObjects()
   {
      Vector<PhysicsObject*> results;
      for (U32 i = 0; i < 1024; ++i)
      {
         if (!mPhysicsBoxes[i].mDeleted)
            results.push_back(&mPhysicsBoxes[i]);

         if (!mPhysicsSpheres[i].mDeleted)
            results.push_back(&mPhysicsSpheres[i]);

         if (!mPhysicsCharacters[i].mDeleted)
            results.push_back(&mPhysicsCharacters[i]);
      }
      return results;
   }

   PhysicsBox* BulletPhysicsEngine::getPhysicsBox(Point3F position, Point3F rotation, Point3F scale, void* _user)
   {
      for (U32 i = 0; i < 1024; ++i)
      {
         if ( mPhysicsBoxes[i].mDeleted )
         {
            mPhysicsBoxes[i].mPosition = position;
            mPhysicsBoxes[i].mRotation = rotation;
            mPhysicsBoxes[i].mScale    = scale;
            mPhysicsBoxes[i].mDeleted  = false;
            mPhysicsBoxes[i].mUser     = _user;
            return &mPhysicsBoxes[i];
         }
      }
      return NULL;
   }

   PhysicsSphere* BulletPhysicsEngine::getPhysicsSphere(Point3F position, Point3F rotation, F32 radius, void* _user)
   {
      for (U32 i = 0; i < 1024; ++i)
      {
         if (mPhysicsSpheres[i].mDeleted)
         {
            mPhysicsSpheres[i].mPosition  = position;
            mPhysicsSpheres[i].mRotation  = rotation;
            mPhysicsSpheres[i].mRadius    = radius;
            mPhysicsSpheres[i].mDeleted   = false;
            mPhysicsSpheres[i].mUser      = _user;
            return &mPhysicsSpheres[i];
         }
      }
      return NULL;
   }

   PhysicsCharacter* BulletPhysicsEngine::getPhysicsCharacter(Point3F position, Point3F rotation, F32 radius, F32 height, void* _user)
   {
      for (U32 i = 0; i < 1024; ++i)
      {
         if (mPhysicsCharacters[i].mDeleted)
         {
            mPhysicsCharacters[i].mPosition = position;
            mPhysicsCharacters[i].mRotation = rotation;
            mPhysicsCharacters[i].mRadius = radius;
            mPhysicsCharacters[i].mHeight = height;
            mPhysicsCharacters[i].mDeleted = false;
            mPhysicsCharacters[i].mUser = _user;
            return &mPhysicsCharacters[i];
         }
      }
      return NULL;
   }

   void BulletPhysicsEngine::deletePhysicsObject(PhysicsObject* _obj)
   {
      // Boxes
      for (U32 i = 0; i < 1024; ++i)
      {
         if ( &mPhysicsBoxes[i] == _obj )
         {
            mPhysicsBoxes[i].mShouldBeDeleted = true;
            return;
         }
      }

      // Spheres
      for (U32 i = 0; i < 1024; ++i)
      {
         if (&mPhysicsSpheres[i] == _obj)
         {
            mPhysicsSpheres[i].mShouldBeDeleted = true;
            return;
         }
      }

      // Characters
      for (U32 i = 0; i < 1024; ++i)
      {
         if (&mPhysicsCharacters[i] == _obj)
         {
            mPhysicsCharacters[i].mShouldBeDeleted = true;
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
      // Initial pass for deletions.
      for (U32 i = 0; i < 1024; ++i)
      {
         // Boxes
         BulletPhysicsBox* box = &mPhysicsBoxes[i];
         if (!box->mDeleted)
         {
            if (box->mShouldBeDeleted)
            {
               if (box->mInitialized)
               {
                  mDynamicsWorld->removeRigidBody(box->_rigidBody);
                  box->destroy();
               }

               box->mDeleted = true;
               continue;
            }
         }

         // Spheres
         BulletPhysicsSphere* sphere = &mPhysicsSpheres[i];
         if (!sphere->mDeleted)
         {
            if (sphere->mShouldBeDeleted)
            {
               if (sphere->mInitialized)
               {
                  mDynamicsWorld->removeRigidBody(sphere->_rigidBody);
                  sphere->destroy();
               }

               sphere->mDeleted = true;
               continue;
            }
         }

         // Characters
         BulletPhysicsCharacter* character = &mPhysicsCharacters[i];
         if (!character->mDeleted)
         {
            if (character->mShouldBeDeleted)
            {
               if (character->mInitialized)
               {
                  mDynamicsWorld->removeRigidBody(character->_rigidBody);
                  character->destroy();
               }

               character->mDeleted = true;
               continue;
            }
         }
      }

      // Update Boxes
      for (U32 i = 0; i < 1024; ++i)
      {
         BulletPhysicsBox* obj = &mPhysicsBoxes[i];
         if ( obj->mDeleted )
            continue;

         if ( !obj->mInitialized )
            obj->initialize(mDynamicsWorld);
         else
            obj->update();
      }

      // Update Spheres
      for (U32 i = 0; i < 1024; ++i)
      {
         BulletPhysicsSphere* obj = &mPhysicsSpheres[i];
         if (obj->mDeleted)
            continue;

         if (!obj->mInitialized)
            obj->initialize(mDynamicsWorld);
         else
            obj->update();
      }

      // Update Characters
      for (U32 i = 0; i < 1024; ++i)
      {
         BulletPhysicsCharacter* obj = &mPhysicsCharacters[i];
         if (obj->mDeleted)
            continue;

         if (!obj->mInitialized)
            obj->initialize(mDynamicsWorld);
         else
            obj->update();
      }
   }
}