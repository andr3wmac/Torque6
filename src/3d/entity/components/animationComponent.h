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

#ifndef _ANIMATION_COMPONENT_H_
#define _ANIMATION_COMPONENT_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _BASE_COMPONENT_H_
#include "baseComponent.h"
#endif

#ifndef _MESH_COMPONENT_H_
#include "meshComponent.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

namespace Scene 
{
   class AnimationComponent : public BaseComponent, public virtual Tickable
   {
      private:
         typedef BaseComponent Parent;
         StringTableEntry                 mMeshAssetId;
         AssetPtr<MeshAsset>              mMeshAsset;

         SimObjectPtr<MeshComponent>      mTarget;
         StringTableEntry                 mTargetName;

      public:
         AnimationComponent();

         void onAddToScene();
         void onRemoveFromScene();
         void refresh() { }

         static void initPersistFields();

         DECLARE_CONOBJECT(AnimationComponent);

      protected:
         F64 mAnimationTime;
         F32 mSpeed;

         virtual void interpolateTick( F32 delta );
         virtual void processTick();
         virtual void advanceTime( F32 timeDelta );

         void setMesh( const char* pMeshAssetId );
         static bool setMesh(void* obj, const char* data) { static_cast<AnimationComponent*>(obj)->setMesh( data ); return false; }
   };
}

#endif _ANIMATION_COMPONENT_H_