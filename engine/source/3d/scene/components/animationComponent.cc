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
#include "animationComponent.h"
#include "graphics/utilities.h"
#include "3d/scene/rendering/forwardRendering.h"
#include "3d/scene/components/meshComponent.h"
#include "3d/assets/shaderAsset.h"

// Script bindings.
#include "AnimationComponent_ScriptBinding.h"

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
   IMPLEMENT_CONOBJECT(AnimationComponent);

   AnimationComponent::AnimationComponent()
   {
      // These are applied to the target.
      mScale.set(0.0f, 0.0f, 0.0f);
      mPosition.set(0.0f, 0.0f, 0.0f);
      mRotation.set(0.0f, 0.0f, 0.0f);
      mAnimationTime = 0.0f;
      mSpeed = 1.0f;
      mTargetName = StringTable->EmptyString;
   }

   void AnimationComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("Speed", TypeF32, Offset(mSpeed, AnimationComponent), "");
      addField("Target", TypeString, Offset(mTargetName, AnimationComponent), "");
      addProtectedField("MeshAsset", TypeAssetId, Offset(mMeshAssetId, AnimationComponent), &setMesh, &defaultProtectedGetFn, "The image asset Id used for the image."); 
   }


   void AnimationComponent::onAddToScene()
   {  
      Con::printf("Animation Component Added!");

      // Load Target
      if ( mTargetName != StringTable->EmptyString )
      {
         mTarget = dynamic_cast<MeshComponent*>(mOwnerEntity->findComponent(mTargetName));
      }

      setProcessTicks(true);
   }

   void AnimationComponent::setMesh( const char* pImageAssetId )
   {
      // Sanity!
      AssertFatal( pImageAssetId != NULL, "Cannot use a NULL asset Id." );

      // Fetch the asset Id.
      mMeshAssetId = StringTable->insert(pImageAssetId);
      mMeshAsset.setAssetId(mMeshAssetId);

      if ( mMeshAsset.isNull() )
         Con::errorf("[Forward Render Component] Failed to load mesh asset.");
   }

   void AnimationComponent::interpolateTick( F32 delta )
   {  
      // Unused at the moment.
   }

   void AnimationComponent::processTick()
   {  
      if ( !mTarget.isNull() )
      {
         //Con::printf("Found Target: %s", mTargetName);
         mMeshAsset->getAnimatedTransforms(mAnimationTime, mTarget->mTransformTable[1]) + 1;
         mTarget->refresh();
      }
   }

   void AnimationComponent::advanceTime( F32 timeDelta )
   {  
      mAnimationTime += (timeDelta * mSpeed);
   }
}