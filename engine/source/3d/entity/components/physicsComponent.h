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

#ifndef _PHYSICS_COMPONENT_H_
#define _PHYSICS_COMPONENT_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _BASE_COMPONENT_H_
#include "baseComponent.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

namespace Physics
{
   struct PhysicsObject;
}

namespace Scene 
{
   class PhysicsComponent : public BaseComponent
   {
      private:
         typedef BaseComponent Parent;
         StringTableEntry mOnCollideFunction;
         StringTableEntry mCollisionType;
         F64 mLastTime;

      public:
         PhysicsComponent();

         void onAddToScene();
         void onRemoveFromScene();
         void refresh();
         void onCollide(Physics::PhysicsObject hit);
         void setLinearVelocity(Point3F pVel);
         void interpolate( F32 value );

         static void initPersistFields();

         DECLARE_CONOBJECT(PhysicsComponent);

         Point3F mCurrVelocity;
         Point3F mCurrPosition;
         Point3F mPrevPosition;
         Point3F mNextPosition;
         Point3F mCorrectPosition;
         Point3F mInputVelocity;
         bool mDirty;
   };
}

#endif // _COLLISION_COMPONENT_H_