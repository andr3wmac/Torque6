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

#include "baseComponent.h"
#include "scene/object.h"

// Script bindings.
#include "baseComponent_Binding.h"

// bgfx/bx
#include <bgfx/bgfx.h>
#include <bx/fpumath.h>

namespace Scene 
{
   IMPLEMENT_CONOBJECT(BaseComponent);

   BaseComponent::BaseComponent()
      : mOwnerObject(NULL)
   {
      mTypeString = "Base";
      mBoundingBox.minExtents.set(0, 0, 0);
      mBoundingBox.maxExtents.set(0, 0, 0);
      mTransform.set(Point3F(0.0f, 0.0f, 0.0f), VectorF(0.0f, 0.0f, 0.0f), VectorF(1.0f, 1.0f, 1.0f));
   }

   void BaseComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addGroup("BaseComponent");
         addProtectedField( "Position", TypePoint3F, 0, &BaseComponent::setPositionFn, &BaseComponent::getPositionFn, &defaultProtectedWriteFn, "Position within the object's local space." );
         addProtectedField( "Rotation", TypePoint3F, 0, &BaseComponent::setRotationFn, &BaseComponent::getRotationFn, &defaultProtectedWriteFn, "Rotation (euler) within the object's local space." );
         addProtectedField( "Scale",    TypePoint3F, 0, &BaseComponent::setScaleFn,    &BaseComponent::getScaleFn,    &defaultProtectedWriteFn, "Scale within the object's local space." );
      endGroup("BaseComponent");
   }

   bool BaseComponent::raycast(const Point3F& start, const Point3F& end, Point3F& hitPoint)
   {
      return false;
   }

   void BaseComponent::refresh()
   {
      if ( !mOwnerObject ) return;

      // Combine local and world.
      mTransformMatrix = mOwnerObject->mTransform.matrix * mTransform.matrix;

      // Set world position.
      mWorldPosition.set(mTransformMatrix[12], mTransformMatrix[13], mTransformMatrix[14]);
   }

   void BaseComponent::setUniformVec4(const char* name, Point4F value)
   {
      bgfx::UniformHandle handle = Graphics::Shader::getUniformVec4(StringTable->insert(name));

      if ( !mUniforms.isEmpty() )
      {
         for ( S32 n = 0; n < mUniforms.uniforms->size(); n++ )
         {
            Rendering::UniformData* uni = &mUniforms.uniforms->at(n);
            if ( uni->uniform.idx == handle.idx )
            {
               uni->setValue(value);
               refresh();
               return;
            }
         }
      }

      Rendering::UniformData* uni = mUniforms.addUniform();
      uni->uniform = handle;
      uni->count = 1;
      uni->setValue(value);
      refresh();
   }

   Box3F BaseComponent::getBoundingBox()
   {
      return mBoundingBox;
   }

   bool BaseComponent::setPositionFn(void* obj, const char* data)
   {
      Point3F position;
      Con::setData(TypePoint3F, position, 0, 1, &data);
      static_cast<BaseComponent*>(obj)->mTransform.setPosition(position);
      return false;
   }

   const char* BaseComponent::getPositionFn(void* obj, const char* data)
   {
      return Con::getData(TypePoint3F, static_cast<BaseComponent*>(obj)->mTransform.getPosition(), 0);
   }

   bool BaseComponent::setRotationFn(void* obj, const char* data)
   {
      Point3F rotation;
      Con::setData(TypePoint3F, rotation, 0, 1, &data);
      static_cast<BaseComponent*>(obj)->mTransform.setRotation(rotation);
      return false;
   }

   const char* BaseComponent::getRotationFn(void* obj, const char* data)
   {
      return Con::getData(TypePoint3F, static_cast<BaseComponent*>(obj)->mTransform.getRotationEuler(), 0);
   }

   bool BaseComponent::setScaleFn(void* obj, const char* data)
   {
      Point3F scale;
      Con::setData(TypePoint3F, scale, 0, 1, &data);
      static_cast<BaseComponent*>(obj)->mTransform.setScale(scale);
      return false;
   }

   const char* BaseComponent::getScaleFn(void* obj, const char* data)
   {
      return Con::getData(TypePoint3F, static_cast<BaseComponent*>(obj)->mTransform.getScale(), 0);
   }
}