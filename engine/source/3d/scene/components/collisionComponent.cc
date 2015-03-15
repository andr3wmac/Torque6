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
#include "collisionComponent.h"
#include "graphics/utilities.h"
#include "3d/rendering/forwardRendering.h"
#include "collision/collisionThread.h"

// Script bindings.
#include "collisionComponent_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

// bgfx/bx
#include <bgfx.h>
#include <bx/fpumath.h>

// Assimp - Asset Import Library
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

namespace Scene
{
   IMPLEMENT_CONOBJECT(CollisionComponent);

   CollisionComponent::CollisionComponent()
   {
      mCollisionObject = NULL;
      mOnCollideFunction = StringTable->insert("");

      // These are applied to the target.
      mScale.set(0.0f, 0.0f, 0.0f);
      mPosition.set(0.0f, 0.0f, 0.0f);
      mRotation.set(0.0f, 0.0f, 0.0f);
   }

   void CollisionComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("onCollideFunction", TypeString, Offset(mOnCollideFunction, CollisionComponent), "");
   }


   void CollisionComponent::onAddToScene()
   {  
      setProcessTicks(true);

      if ( CollisionThread::lock() )
      {
         CollisionObject a;
         CollisionThread::smCollisionObjects.push_back(a);
         mCollisionObject = &CollisionThread::smCollisionObjects.back();
         mCollisionObject->onCollideDelegate.bind(this, &CollisionComponent::onCollide);
         mCollisionObject->worldBoundingBox = mOwnerEntity->mBoundingBox;

         CollisionThread::unlock();
      }
   }

   void CollisionComponent::interpolateTick( F32 delta )
   {  
      //
   }

   void CollisionComponent::processTick()
   {  
      //
   }

   void CollisionComponent::advanceTime( F32 timeDelta )
   {  
      //
   }

   void CollisionComponent::refresh()
   {
      if ( !mCollisionObject )
         return;

      if ( CollisionThread::lock() )
      {
         mCollisionObject->worldBoundingBox = mOwnerEntity->mBoundingBox;
         CollisionThread::unlock();
      }
   }

   void CollisionComponent::onCollide(CollisionObject hit)
   {
      //Con::printf("COLLISION OCCURED FROM COMPONENT!");
      if ( dStrlen(mOnCollideFunction) > 0 )
         Con::executef(mOwnerEntity, 1, mOnCollideFunction);
   }
}