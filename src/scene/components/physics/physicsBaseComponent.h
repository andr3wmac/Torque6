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

#ifndef _PHYSICS_BASE_COMPONENT_H_
#define _PHYSICS_BASE_COMPONENT_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _BASE_COMPONENT_H_
#include "scene/components/baseComponent.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

namespace Physics
{
   class PhysicsCharacter;
   class PhysicsObject;
}

namespace Scene 
{
   class DLL_PUBLIC PhysicsBaseComponent : public BaseComponent, public virtual Tickable
   {
      protected:
         typedef BaseComponent Parent;

         Physics::PhysicsObject*    mPhysicsObject;

         StringTableEntry           mOnCollideFunction;
         StringTableEntry           mCollisionType;
         F64                        mLastTime;
         bool                       mStatic;
         
         Point3F                    mExpectedOwnerPosition;
         Point3F                    mLastOwnerPosition;
         Point3F                    mNextOwnerPosition;

      public:
         PhysicsBaseComponent();

         virtual void onAddToScene();
         virtual void onRemoveFromScene();
         virtual void refresh();

         virtual void onCollide(void* _hitUser);
         virtual void setLinearVelocity(Point3F pVel);

         virtual StringTableEntry getOnCollideFunction() { return mOnCollideFunction; }
         virtual void setOnCollideFunction(StringTableEntry func) { mOnCollideFunction = func; }
         virtual StringTableEntry getCollisionType() { return mCollisionType; }
         virtual void setCollisionType(StringTableEntry type) { mCollisionType = type; }
         virtual bool getStatic() { return mStatic; }
         virtual void setStatic(bool isStatic) { mStatic = isStatic; }

         virtual void interpolateTick(F32 delta);
         virtual void processTick();
         virtual void advanceTime(F32 timeDelta);

         static void initPersistFields();

         DECLARE_CONOBJECT(PhysicsBaseComponent);
   };
}

#endif // _PHYSICS_BASE_COMPONENT_H_