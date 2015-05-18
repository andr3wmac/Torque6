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

// Script bindings.
#include "baseComponent_ScriptBinding.h"

// bgfx/bx
#include <bgfx.h>
#include <bx/fpumath.h>

namespace Scene 
{
   IMPLEMENT_CONOBJECT(BaseComponent);

   BaseComponent::BaseComponent()
      : mOwnerEntity(NULL)
   {
      mTypeString = "Base";
      mBoundingBox.minExtents.set(0, 0, 0);
      mBoundingBox.maxExtents.set(0, 0, 0);
      mScale.set(1.0f, 1.0f, 1.0f);
      mPosition.set(0.0f, 0.0f, 0.0f);
      mRotation.set(0.0f, 0.0f, 0.0f);
   }

   void BaseComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField( "Position", TypePoint3F, Offset(mPosition, BaseComponent), "Position within the entity's local space." );
      addField( "Rotation", TypePoint3F, Offset(mRotation, BaseComponent), "Position within the entity's local space." );
      addField( "Scale", TypePoint3F, Offset(mScale, BaseComponent), "Position within the entity's local space." );
   }

   void BaseComponent::refresh()
   {
      if ( !mOwnerEntity ) return;

      // Build Transformation Matrix
      F32 mtxLocalScale[16];
      bx::mtxScale(mtxLocalScale, mScale.x, mScale.y, mScale.z);

      F32 mtxLocalRotate[16];
      bx::mtxRotateXYZ(mtxLocalRotate, mRotation.x, mRotation.y, mRotation.z);

      F32 mtxLocal[16];
      bx::mtxMul(mtxLocal, mtxLocalScale, mtxLocalRotate );
	   mtxLocal[12] = mPosition.x;
	   mtxLocal[13] = mPosition.y;
	   mtxLocal[14] = mPosition.z;

      F32 mtxWorldScale[16];
      bx::mtxScale(mtxWorldScale, mOwnerEntity->mScale.x, mOwnerEntity->mScale.y, mOwnerEntity->mScale.z);

      F32 mtxWorldRotate[16];
      bx::mtxRotateXYZ(mtxWorldRotate, mOwnerEntity->mRotation.x, mOwnerEntity->mRotation.y, mOwnerEntity->mRotation.z);

      F32 mtxWorld[16];
      bx::mtxMul(mtxWorld, mtxWorldScale, mtxWorldRotate );
	   mtxWorld[12] = mOwnerEntity->mPosition.x;
	   mtxWorld[13] = mOwnerEntity->mPosition.y;
	   mtxWorld[14] = mOwnerEntity->mPosition.z;

      // Combine local and world.
      bx::mtxIdentity(mTransformMatrix);
      bx::mtxMul(mTransformMatrix, mtxLocal, mtxWorld );

      // Set world position.
      mWorldPosition.set(mTransformMatrix[12], mTransformMatrix[13], mTransformMatrix[14]);
   }

   void BaseComponent::setUniformFloat(const char* name, F32 value)
   {
      bgfx::UniformHandle handle = Graphics::Shader::getUniform(name, bgfx::UniformType::Uniform1f);

      if ( !mUniforms.isEmpty() )
      {
         for ( S32 n = 0; n < mUniforms.uniforms->size(); n++ )
         {
            Rendering::UniformData* uni = &mUniforms.uniforms->at(n);
            if ( uni->uniform.idx == handle.idx )
            {
               dMemcpy(uni->data, &value, sizeof(value));
               refresh();
               return;
            }
         }
      }

      Rendering::UniformData* uni = mUniforms.addUniform();
      uni->uniform = handle;
      uni->count = 1;
      uni->data = new F32(value);
      refresh();
   }

   void BaseComponent::setUniformVec3(const char* name, Point3F value)
   {
      bgfx::UniformHandle handle = Graphics::Shader::getUniformVec3(name);

      if ( !mUniforms.isEmpty() )
      {
         for ( S32 n = 0; n < mUniforms.uniforms->size(); n++ )
         {
            Rendering::UniformData* uni = &mUniforms.uniforms->at(n);
            if ( uni->uniform.idx == handle.idx )
            {
               dMemcpy(uni->data, value, sizeof(value));
               refresh();
               return;
            }
         }
      }

      Rendering::UniformData* uni = mUniforms.addUniform();
      uni->uniform = handle;
      uni->count = 1;
      uni->data = new Point3F(value);
      refresh();
   }


}