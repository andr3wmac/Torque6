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
#include "controllerComponent.h"
#include "graphics/core.h"
#include "scene/object.h"
#include "scene/components/meshComponent.h"

#include "scene/scene.h"
#include "gui/guiCanvas.h"

// Script bindings.
#include "controllerComponent_Binding.h"

// bgfx/bx
#include <bgfx/bgfx.h>
#include <bx/fpumath.h>

// Client prediction
static F32 sMinWarpTicks = 0.5f;       // Fraction of tick at which instant warp occurs
static S32 sMaxWarpTicks = 3;          // Max warp duration in ticks
static S32 sMaxPredictionTicks = 30;   // Number of ticks to predict

namespace Scene
{
   IMPLEMENT_CONOBJECT(ControllerComponent);

   ControllerComponent::ControllerComponent()
      : mLinearVelocity(Point3F::Zero),
        mForwardVelocity(Point3F::Zero),
        mBindMouse(false),
        mBindMouseLeftBtn(false),
        mBindMouseRightBtn(false),
        mDirty(false),
        mPhysicsCharacter(NULL)
   {
   }

   void ControllerComponent::initPersistFields()
   {
      Parent::initPersistFields();

      addGroup("ControllerComponent");

         addField("BindMouse", TypeBool, Offset(mBindMouse, ControllerComponent), "");

      endGroup("ControllerComponent");
   }

   void ControllerComponent::onAddToScene()
   {  
      mPhysicsCharacter = mOwnerObject->findComponentByType<PhysicsCharacterComponent>();

      setProcessTicks(true);
      setListening(true);
   }

   void ControllerComponent::onRemoveFromScene()
   {
      setProcessTicks(false);
      setListening(false);
   }

   bool ControllerComponent::processInputEvent(const InputEvent* event)
   {
      return false;
   }

   bool ControllerComponent::processMouseMoveEvent(const MouseMoveEvent *event)
   {
      if (!mBindMouse || mBindMouseLeftBtn || mBindMouseRightBtn) return false;

      Point2I center    = Point2I(Canvas->getWidth() / 2, Canvas->getHeight() / 2);
      Point2I mousePos  = Point2I(event->xPos, event->yPos);
      Point2I delta     = center - mousePos;

      if (delta.isZero()) return false;
      Canvas->setCursorPos(center);

      mTarget.horizontalAngle += delta.x * 0.01f;
      mTarget.verticalAngle -= delta.y * 0.01f;
      mTarget.verticalAngle = mClampF(mTarget.verticalAngle, -4.7f, -1.7f);
      return true;
   }

   bool ControllerComponent::processScreenTouchEvent(const ScreenTouchEvent *event)
   {
      return false;
   }


   void ControllerComponent::refresh()
   {
      Parent::refresh();

      if (mDirty)
      {
         mDirty = false;

         mCurrent.verticalAngle     = mClampF(mCurrent.verticalAngle, -4.7f, -1.7f);
         mOwnerObject->mRotation.y  = mCurrent.verticalAngle;
         mOwnerObject->mRotation.z  = mCurrent.horizontalAngle;

         if (mPhysicsCharacter == NULL)
         {
            mOwnerObject->mPosition = mCurrent.position;
            mOwnerObject->refresh();
         }
      }
   }

   // -----------------------------------------------------

   void ControllerComponent::interpolateTick(F32 delta)
   {
      //

   }

   void ControllerComponent::processTick()
   {
      if (mForwardVelocity.len() > 0.01f)
      {
         VectorF up(0.0f, 0.0f, 1.0f);
         VectorF forward(1.0f, 0.0f, 0.0f);
         MatrixF lookMatrix;
         VectorF look;

         bx::vec3MulMtx(look, forward, mTransformMatrix);
         bx::mtxLookAt(lookMatrix, mWorldPosition, look, up);

         Point3F velocity = Point3F(0.0f, 0.0f, 0.0f);
         velocity -= (lookMatrix.getForwardVector() * mForwardVelocity.x);
         velocity -= (lookMatrix.getRightVector() * mForwardVelocity.y);

         setLinearVelocity(velocity);
      }
   }

   void ControllerComponent::advanceTime(F32 timeDelta)
   {
      F32 blend = getMin(10.0f * timeDelta, 1.0f);

      mCurrent.horizontalAngle   = mLerp(mCurrent.horizontalAngle, mTarget.horizontalAngle, blend);
      mCurrent.verticalAngle     = mLerp(mCurrent.verticalAngle, mTarget.verticalAngle, blend);

      if (mPhysicsCharacter == NULL)
         mCurrent.position += mLinearVelocity * timeDelta;

      mDirty = true;
      refresh();
   }

   void ControllerComponent::setLinearVelocity(Point3F velocity)
   {
      mLinearVelocity = velocity;

      if (mPhysicsCharacter != NULL)
         mPhysicsCharacter->setLinearVelocity(mLinearVelocity);
   }

   void ControllerComponent::setForwardVelocity(Point3F velocity)
   {
      mForwardVelocity = velocity;
      if (mForwardVelocity.len() < 0.01f)
      {
         setLinearVelocity(Point3F(0.0f, 0.0f, 0.0f));
      }
   }

   // -----------------------------------------------------

   void ControllerComponent::setBindMouse(bool value, bool left, bool right)
   {
      mBindMouse           = value;
      mBindMouseLeftBtn    = left;
      mBindMouseRightBtn   = right;
   }
}
