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
#include "physicsBaseComponent.h"
#include "graphics/core.h"
#include "scene/object.h"
#include "rendering/rendering.h"
#include "physics/physics.h"
#include "physics/physicsThread.h"

// Script bindings.
#include "physicsBaseComponent_Binding.h"

// Debug Profiling.
#include "debug/profiler.h"

// bgfx/bx
#include <bgfx/bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

// Assimp - Asset Import Library
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

namespace Scene
{
   IMPLEMENT_CONOBJECT(PhysicsBaseComponent);

   PhysicsBaseComponent::PhysicsBaseComponent()
   {
      mTypeString          = "Physics";
      mOnCollideFunction   = StringTable->insert("");
      mCollisionType       = StringTable->insert("");
      mLastTime            = 0.0f;
      mStatic              = false;

      mPhysicsObject          = NULL;
      mExpectedOwnerPosition  = Point3F(0.0f, 0.0f, 0.0f);
      mLastOwnerPosition      = Point3F(0.0f, 0.0f, 0.0f);
      mNextOwnerPosition      = Point3F(0.0f, 0.0f, 0.0f);

      // These are applied to the target.
      mScale.set(1.0f, 1.0f, 1.0f);
      mPosition.set(0.0f, 0.0f, 0.0f);
      mRotation.set(0.0f, 0.0f, 0.0f);
   }

   void PhysicsBaseComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addGroup("PhysicsBaseComponent");
         addField("OnCollideFunction", TypeString,    Offset(mOnCollideFunction, PhysicsBaseComponent), "");
         addField("CollisionType",     TypeString,    Offset(mCollisionType, PhysicsBaseComponent), "");
         addField("Static",            TypeBool,      Offset(mStatic, PhysicsBaseComponent), "");
      endGroup("PhysicsBaseComponent");
   }

   void PhysicsBaseComponent::onAddToScene()
   {  
      if ( mOwnerObject->mGhosted && mOwnerObject->isClientObject() )
         return;

      mPhysicsObject->setStatic(mStatic);
      mPhysicsObject->mOnCollideDelegate.bind(this, &PhysicsBaseComponent::onCollide);

      mLastOwnerPosition = mOwnerObject->mPosition;
      mNextOwnerPosition = mOwnerObject->mPosition;

      setProcessTicks(true);
   }

   void PhysicsBaseComponent::onRemoveFromScene()
   {
      setProcessTicks(false);
   }

   void PhysicsBaseComponent::refresh()
   {
      Parent::refresh();
   }

   void PhysicsBaseComponent::onCollide(void* _hitUser)
   {
      PhysicsBaseComponent* collideComp = (PhysicsBaseComponent*)_hitUser;
      if ( dStrlen(mOnCollideFunction) > 0 )
         Con::executef(mOwnerObject, 3, mOnCollideFunction, Con::getIntArg(collideComp->mOwnerObject->getId()), "");
   }

   void PhysicsBaseComponent::setLinearVelocity( Point3F pVel )
   {
      mPhysicsObject->setLinearVelocity(pVel);
   }

   void PhysicsBaseComponent::interpolateTick(F32 delta)
   {
      mOwnerObject->mPosition.interpolate(mLastOwnerPosition, mNextOwnerPosition, 1.0f - delta);
      mExpectedOwnerPosition = mOwnerObject->mPosition;
      mOwnerObject->refresh();
   }

   void PhysicsBaseComponent::processTick()
   {
      if (mOwnerObject->mPosition != mExpectedOwnerPosition)
      {
         mPhysicsObject->setPosition(mOwnerObject->mPosition);

         mExpectedOwnerPosition  = mOwnerObject->mPosition;
         mLastOwnerPosition      = mOwnerObject->mPosition;
         mNextOwnerPosition      = mOwnerObject->mPosition;
      }
      else {
         mLastOwnerPosition   = mNextOwnerPosition;
         mNextOwnerPosition   = mPhysicsObject->getPosition() - mPosition;
      }
   }

   void PhysicsBaseComponent::advanceTime(F32 timeDelta)
   {

   }
}