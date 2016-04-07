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
      _world = world;
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

      _world->addRigidBody(_rigidBody);
      mInitialized = true;
   }

   void BulletPhysicsBox::destroy()
   {
      if ( !mInitialized )
         return;

      _world->removeRigidBody(_rigidBody);

      SAFE_DELETE(_shape);
      SAFE_DELETE(_rigidBody);
      SAFE_DELETE(_motionState);

      mInitialized = false;
   }

   void BulletPhysicsBox::update()
   {
      if (mShouldBeDeleted)
      {
         destroy();
         mDeleted = true;
         return;
      }

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
      _world = world;
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

      _world->addRigidBody(_rigidBody);
      mInitialized = true;
   }

   void BulletPhysicsSphere::destroy()
   {
      if (!mInitialized)
         return;

      _world->removeRigidBody(_rigidBody);

      SAFE_DELETE(_shape);
      SAFE_DELETE(_rigidBody);
      SAFE_DELETE(_motionState);

      mInitialized = false;
   }

   void BulletPhysicsSphere::update()
   {
      if (mShouldBeDeleted)
      {
         destroy();
         mDeleted = true;
         return;
      }

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
   // Physics Mesh
   // --------------------------------------

   BulletPhysicsMesh::BulletPhysicsMesh()
   {
      _mesh          = NULL;
      _shape         = NULL;
      _motionState   = NULL;
      _rigidBody     = NULL;
   }

   BulletPhysicsMesh::~BulletPhysicsMesh()
   {
      destroy();
   }

   void BulletPhysicsMesh::initialize(btDiscreteDynamicsWorld* world)
   {
      _world = world;
      _mesh = new btTriangleMesh();

      for (S32 i = 0; i < mMeshData.faces.size(); ++i)
      {
         Graphics::MeshFace* face = &mMeshData.faces[i];
         Graphics::PosUVTBNBonesVertex* vertA = &mMeshData.verts[face->verts[0]];
         Graphics::PosUVTBNBonesVertex* vertB = &mMeshData.verts[face->verts[1]];
         Graphics::PosUVTBNBonesVertex* vertC = &mMeshData.verts[face->verts[2]];

         _mesh->addTriangle(btVector3(vertA->m_x, vertA->m_y, vertA->m_z),
            btVector3(vertB->m_x, vertB->m_y, vertB->m_z),
            btVector3(vertC->m_x, vertC->m_y, vertC->m_z));
      }

      _shape = new btConvexTriangleMeshShape(_mesh);

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

      _world->addRigidBody(_rigidBody);
      mInitialized = true;
   }

   void BulletPhysicsMesh::destroy()
   {
      if (!mInitialized)
         return;

      _world->removeRigidBody(_rigidBody);

      SAFE_DELETE(_shape);
      SAFE_DELETE(_rigidBody);
      SAFE_DELETE(_motionState);

      mInitialized = false;
   }

   void BulletPhysicsMesh::update()
   {
      if (mShouldBeDeleted)
      {
         destroy();
         mDeleted = true;
         return;
      }

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

      _world->addRigidBody(_rigidBody);
      mInitialized = true;
   }

   void BulletPhysicsCharacter::destroy()
   {
      if (!mInitialized)
         return;

      _world->removeRigidBody(_rigidBody);

      SAFE_DELETE(_shape);
      SAFE_DELETE(_rigidBody);
      SAFE_DELETE(_motionState);

      mInitialized = false;
   }

   void BulletPhysicsCharacter::update()
   {
      if (mShouldBeDeleted)
      {
         destroy();
         mDeleted = true;
         return;
      }

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
      for (U32 i = 0; i < MAX_PHYSICS_OBJECTS; ++i)
      {
         mPhysicsBoxes[i].destroy();
         mPhysicsSpheres[i].destroy();
         mPhysicsMeshes[i].destroy();
         mPhysicsCharacters[i].destroy();
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
      for (U32 i = 0; i < MAX_PHYSICS_OBJECTS; ++i)
      {
         if (!mPhysicsBoxes[i].mDeleted)
            results.push_back(&mPhysicsBoxes[i]);

         if (!mPhysicsSpheres[i].mDeleted)
            results.push_back(&mPhysicsSpheres[i]);

         if (!mPhysicsMeshes[i].mDeleted)
            results.push_back(&mPhysicsMeshes[i]);

         if (!mPhysicsCharacters[i].mDeleted)
            results.push_back(&mPhysicsCharacters[i]);
      }
      return results;
   }

   PhysicsBox* BulletPhysicsEngine::createPhysicsBox(Point3F position, Point3F rotation, Point3F scale, void* _user)
   {
      for (U32 i = 0; i < MAX_PHYSICS_OBJECTS; ++i)
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

   PhysicsSphere* BulletPhysicsEngine::createPhysicsSphere(Point3F position, Point3F rotation, F32 radius, void* _user)
   {
      for (U32 i = 0; i < MAX_PHYSICS_OBJECTS; ++i)
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

   PhysicsMesh* BulletPhysicsEngine::createPhysicsMesh(Point3F position, Point3F rotation, Point3F scale, const Graphics::MeshData& meshData, void* _user)
   {
      for (U32 i = 0; i < MAX_PHYSICS_OBJECTS; ++i)
      {
         if (mPhysicsMeshes[i].mDeleted)
         {
            mPhysicsMeshes[i].mPosition   = position;
            mPhysicsMeshes[i].mRotation   = rotation;
            mPhysicsMeshes[i].mScale      = scale;
            mPhysicsMeshes[i].mMeshData   = meshData;   
            mPhysicsMeshes[i].mDeleted    = false;
            mPhysicsMeshes[i].mUser       = _user;
            return &mPhysicsMeshes[i];
         }
      }
      return NULL;
   }

   PhysicsCharacter* BulletPhysicsEngine::createPhysicsCharacter(Point3F position, Point3F rotation, F32 radius, F32 height, void* _user)
   {
      for (U32 i = 0; i < MAX_PHYSICS_OBJECTS; ++i)
      {
         if (mPhysicsCharacters[i].mDeleted)
         {
            mPhysicsCharacters[i].mPosition  = position;
            mPhysicsCharacters[i].mRotation  = rotation;
            mPhysicsCharacters[i].mRadius    = radius;
            mPhysicsCharacters[i].mHeight    = height;
            mPhysicsCharacters[i].mDeleted   = false;
            mPhysicsCharacters[i].mUser      = _user;
            return &mPhysicsCharacters[i];
         }
      }
      return NULL;
   }

   template<typename T>
   void _deletePhysicsObject(T s, PhysicsObject* _obj)
   {
      for (U32 i = 0; i < MAX_PHYSICS_OBJECTS; ++i)
      {
         if (&s[i] == _obj)
         {
            s[i].mShouldBeDeleted = true;
            return;
         }
      }
   }

   void BulletPhysicsEngine::deletePhysicsObject(PhysicsObject* _obj)
   {
      _deletePhysicsObject<BulletPhysicsBox*>(mPhysicsBoxes, _obj);
      _deletePhysicsObject<BulletPhysicsSphere*>(mPhysicsSpheres, _obj);
      _deletePhysicsObject<BulletPhysicsMesh*>(mPhysicsMeshes, _obj);
      _deletePhysicsObject<BulletPhysicsCharacter*>(mPhysicsCharacters, _obj);
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

   template<typename T>
   void _updatePhysicsObject(T s, btDiscreteDynamicsWorld* world)
   {
      for (U32 i = 0; i < MAX_PHYSICS_OBJECTS; ++i)
      {
         T obj = &s[i];
         if (obj->mDeleted)
            continue;

         if (!obj->mInitialized && !obj->mShouldBeDeleted)
            obj->initialize(world);
         else
            obj->update();
      }
   }

   void BulletPhysicsEngine::update()
   {
      _updatePhysicsObject<BulletPhysicsBox*>(mPhysicsBoxes, mDynamicsWorld);
      _updatePhysicsObject<BulletPhysicsSphere*>(mPhysicsSpheres, mDynamicsWorld);
      _updatePhysicsObject<BulletPhysicsMesh*>(mPhysicsMeshes, mDynamicsWorld);
      _updatePhysicsObject<BulletPhysicsCharacter*>(mPhysicsCharacters, mDynamicsWorld);
   }
}