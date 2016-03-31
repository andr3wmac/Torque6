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
#include "physicsCharacterComponent.h"
#include "graphics/core.h"
#include "scene/object.h"
#include "rendering/rendering.h"
#include "physics/physics.h"
#include "physics/physicsThread.h"

// Script bindings.
#include "physicsCharacterComponent_Binding.h"

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
   IMPLEMENT_CONOBJECT(PhysicsCharacterComponent);

   PhysicsCharacterComponent::PhysicsCharacterComponent()
   {
      mTypeString       = "PhysicsCharacter";
      mPhysicsCharacter = NULL;
      mRadius           = 0.6f;
      mHeight           = 1.82f;
   }

   void PhysicsCharacterComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addGroup("PhysicsCharacterComponent");
         addField("Radius", TypeF32, Offset(mRadius, PhysicsCharacterComponent), "");
         addField("Height", TypeF32, Offset(mHeight, PhysicsCharacterComponent), "");
      endGroup("PhysicsCharacterComponent");
   }

   void PhysicsCharacterComponent::onAddToScene()
   {
      mPhysicsCharacter = Physics::getPhysicsCharacter(mOwnerObject->mPosition + mPosition, mRotation, mRadius, mHeight, this);
      mPhysicsObject    = mPhysicsCharacter;

      // We have to call this AFTER mPhysicsObject is created and set.
      Parent::onAddToScene();
   }

   void PhysicsCharacterComponent::onRemoveFromScene()
   {
      Parent::onRemoveFromScene();

      if (mPhysicsCharacter != NULL)
         Physics::deletePhysicsObject(mPhysicsCharacter);
   }

   void PhysicsCharacterComponent::jump()
   {
      mPhysicsCharacter->applyForce(Point3F(0.0f, 0.0f, 50000.0f));
   }
}