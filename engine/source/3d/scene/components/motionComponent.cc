//-----------------------------------------------------------------------------
// Copyright (c) 2014 Andrew Mac
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
#include "motionComponent.h"
#include "graphics/utilities.h"
#include "3d/scene/rendering/forwardRendering.h"
#include "3d/assets/shaderAsset.h"

// Script bindings.
#include "motionComponent_ScriptBinding.h"

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
   IMPLEMENT_CONOBJECT(MotionComponent);

   MotionComponent::MotionComponent()
   {
      // These are applied to the target.
      mScale.set(0.0f, 0.0f, 0.0f);
      mPosition.set(0.0f, 0.0f, 0.0f);
      mRotation.set(0.0f, 0.0f, 0.0f);
      mTickCount = 0.0f;
      mInterval = 0.0f;
   }

   void MotionComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("Interval", TypeF32, Offset(mInterval, MotionComponent), "");
   }


   void MotionComponent::onAddToScene()
   {  
      setProcessTicks(true);
   }

   void MotionComponent::interpolateTick( F32 delta )
   {  
      if ( mPosition.isZero() && mRotation.isZero() && mScale.isZero() ) 
         return;

      if ( mInterval > 0 )
      {
          mTickCount += delta;
         if ( mTickCount >= mInterval )
            mTickCount = 0.0f;
         else
            return;
      }

      mOwnerEntity->mPosition += mPosition * delta;
      mOwnerEntity->mRotation += mRotation * delta;
      mOwnerEntity->mScale += mScale * delta;

      mOwnerEntity->refresh();
   }

   void MotionComponent::processTick()
   {  
      //
   }

   void MotionComponent::advanceTime( F32 timeDelta )
   {  
      //Con::printf("Animation Component advanceTime");
   }
}