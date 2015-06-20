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

#include "console/consoleTypes.h"
#include "physicsComponent.h"
#include "graphics/core.h"
#include "3d/rendering/common.h"
#include "physics/physics.h"
#include "physics/physicsThread.h"

// Script bindings.
#include "physicsComponent_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

// bgfx/bx
#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

// Assimp - Asset Import Library
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

namespace Scene
{
   IMPLEMENT_CONOBJECT(PhysicsComponent);

   PhysicsComponent::PhysicsComponent()
   {
      mTypeString          = "Physics";
      mOnCollideFunction   = StringTable->insert("");
      mCollisionType       = StringTable->insert("");
      mPhysicsObject       = NULL;
      mLastTime            = 0;
      mStatic              = false;

      // These are applied to the target.
      mScale.set(1.0f, 1.0f, 1.0f);
      mPosition.set(0.0f, 0.0f, 0.0f);
      mRotation.set(0.0f, 0.0f, 0.0f);
   }

   void PhysicsComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("onCollideFunction", TypeString,    Offset(mOnCollideFunction, PhysicsComponent), "");
      addField("collisionType",     TypeString,    Offset(mCollisionType, PhysicsComponent), "");
      addField("static",            TypeBool,      Offset(mStatic, PhysicsComponent), "");
   }


   void PhysicsComponent::onAddToScene()
   {  
      mPhysicsObject = Physics::getPhysicsObject(this);
      mPhysicsObject->onCollideDelegate.bind(this, &PhysicsComponent::onCollide);
      setProcessTicks(true);
   }

   void PhysicsComponent::onRemoveFromScene()
   {
      if ( mPhysicsObject != NULL )
         Physics::deletePhysicsObject(mPhysicsObject);
      
      setProcessTicks(false);
   }

   void PhysicsComponent::interpolateTick( F32 delta )
   {

   }

   void PhysicsComponent::processTick()
   {
      if ( mPhysicsObject == NULL )
         return;

      Point3F physics_position = mPhysicsObject->getPosition();
      mOwnerEntity->mPosition.set(physics_position);
      Point3F physics_rotation = mPhysicsObject->getRotation();
      mOwnerEntity->mRotation.set(physics_rotation);
      mOwnerEntity->refresh();
   }

   void PhysicsComponent::advanceTime( F32 timeDelta )
   {

   }

   void PhysicsComponent::refresh()
   {
      if ( !mPhysicsObject->initialized )
      {
         mPhysicsObject->setPosition(mOwnerEntity->mPosition + mPosition);
         mPhysicsObject->setScale(mScale * mOwnerEntity->mScale);
         mPhysicsObject->setStatic(mStatic);
      }
   }

   void PhysicsComponent::onCollide(void* _hitUser)
   {
      PhysicsComponent* collideComp = (PhysicsComponent*)_hitUser;
      if ( dStrlen(mOnCollideFunction) > 0 )
         Con::executef(mOwnerEntity, 3, mOnCollideFunction, Con::getIntArg(collideComp->mOwnerEntity->getId()), "");
   }

   void PhysicsComponent::setLinearVelocity( Point3F pVel )
   {
      mPhysicsObject->setLinearVelocity(pVel);
   }
}