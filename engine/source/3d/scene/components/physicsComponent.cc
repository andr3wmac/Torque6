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
#include "graphics/utilities.h"
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
      mTypeString = "Physics";

      mOnCollideFunction = StringTable->insert("");
      mCollisionType = StringTable->insert("");

      // These are applied to the target.
      mScale.set(0.0f, 0.0f, 0.0f);
      mPosition.set(0.0f, 0.0f, 0.0f);
      mRotation.set(0.0f, 0.0f, 0.0f);

      mInputVelocity.set(0.0f, 0.0f, 0.0f);
      mCurrVelocity.set(0.0f, 0.0f, 0.0f);
      mCurrPosition.set(0.0f, 0.0f, 0.0f);
      mPrevPosition.set(0.0f, 0.0f, 0.0f);
      mNextPosition.set(0.0f, 0.0f, 0.0f);

      mLastTime = 0;
      mDirty = false;
   }

   void PhysicsComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("onCollideFunction", TypeString, Offset(mOnCollideFunction, PhysicsComponent), "");
      addField("collisionType", TypeString, Offset(mCollisionType, PhysicsComponent), "");
      addField("LinearVelocity", TypePoint3F, Offset(mInputVelocity, PhysicsComponent), "");
   }


   void PhysicsComponent::onAddToScene()
   {  
      if ( Physics::engine != NULL )
         Physics::engine->addComponent(this);
   }

   void PhysicsComponent::onRemoveFromScene()
   {
      if ( Physics::engine != NULL )
         Physics::engine->removeComponent(this);
   }

   void PhysicsComponent::interpolate( F32 value )
   {  
      if ( mCurrPosition == mNextPosition && mCurrPosition == mPrevPosition )
         return;

      if ( value <= 0.0f && value >= 1.0f )
         return;

      if ( mCurrPosition == mNextPosition )
      {
         mOwnerEntity->mPosition.interpolate(mNextPosition, mNextPosition + (mCurrVelocity * (1.0f/32.0f)), value);
         mCurrPosition = mOwnerEntity->mPosition;
         mOwnerEntity->refresh();
         return;
      }

      mOwnerEntity->mPosition.interpolate(mPrevPosition, mNextPosition, value);
      mCurrPosition = mOwnerEntity->mPosition;
      mOwnerEntity->refresh();
   }

   void PhysicsComponent::refresh()
   {
      //if ( mOwnerEntity->mPosition != mCurrPosition )
      //   mDirty = true;
   }

   void PhysicsComponent::onCollide(Physics::PhysicsObject hit)
   {
      if ( dStrlen(mOnCollideFunction) > 0 )
         Con::executef(mOwnerEntity, 3, mOnCollideFunction, Con::getIntArg(hit.entity->getId()), hit.typeStr);
   }

   void PhysicsComponent::setLinearVelocity( Point3F pVel )
   {
      mInputVelocity = pVel * 50.0f;
      mDirty = true;
      refresh();
   }
}