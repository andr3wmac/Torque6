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

#ifndef _PHYSICS_SPHERE_COMPONENT_H_
#define _PHYSICS_SPHERE_COMPONENT_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _BASE_COMPONENT_H_
#include "scene/components/baseComponent.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

#ifndef _PHYSICS_BASE_COMPONENT_H_
#include "physicsBaseComponent.h"
#endif

namespace Physics
{
   class PhysicsSphere;
}

namespace Scene 
{
   class DLL_PUBLIC PhysicsSphereComponent : public PhysicsBaseComponent
   {
      private:
         typedef PhysicsBaseComponent Parent;

         F32                        mRadius;
         Physics::PhysicsSphere*    mPhysicsSphere;

      public:
         PhysicsSphereComponent();

         virtual void onAddToScene();
         virtual void onRemoveFromScene();

         static void initPersistFields();

         DECLARE_CONOBJECT(PhysicsSphereComponent);
   };
}

#endif // _PHYSICS_SPHERE_COMPONENT_H_