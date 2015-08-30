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

#include "camera.h"
#include "core.h"

#include "console/consoleInternal.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "3d/rendering/common.h"
#include "platform/event.h"
#include "gui/guiCanvas.h"

#include <bgfx.h>
#include <bx/fpumath.h>

#include "camera_Binding.h"

namespace Scene
{
   IMPLEMENT_CONOBJECT(SceneCamera);

   SceneCamera::SceneCamera()
      :  mDirection(Point3F::Zero),
         mLookAt(Point3F::Zero),
         mUp(Point3F::Zero),
         mPanVelocity(Point3F::Zero),
         mActive(false),
         mBindMouse(false),
         mBindMouseLeftBtn(false),
         mBindMouseRightBtn(false)
   {
      setProcessTicks(false);
   }

   SceneCamera::~SceneCamera()
   {
      // 
   }

   void SceneCamera::copy(SceneCamera* cam)
   {
      mCurrent = cam->mCurrent;
      mTarget = cam->mTarget;
      mDirection = cam->mDirection;
      mLookAt = cam->mLookAt;
      mUp = cam->mUp;
      mPanVelocity = cam->mPanVelocity;
      refreshAngles();
   }

   void SceneCamera::initPersistFields()
   {
       // Call parent.
       Parent::initPersistFields();

       addField("Position",         TypePoint3F,   Offset(mCurrent.position, SceneCamera), "");
       addField("HorizontalAngle",  TypeF32,       Offset(mCurrent.horizontalAngle, SceneCamera), "");
       addField("VerticalAngle",    TypeF32,       Offset(mCurrent.verticalAngle, SceneCamera), "");
   }

   // -----------------------------------------------------

   void SceneCamera::setActive(bool val)
   {
      mActive = val;
      setProcessTicks(mActive);
   }

   // -----------------------------------------------------

   void SceneCamera::lookAt(Point3F look_at_position)
   {
      mLookAt = look_at_position;
      mUp.set(0, 1, 0);
      refresh();
   }

   void SceneCamera::translate(Point3F translation)
   {
      mTarget.position -= translation;
   }

   void SceneCamera::rotate(Point3F translation)
   {
      F32 rotMatrix[16];
      bx::mtxRotateXYZ(rotMatrix, translation.x, translation.y, translation.z);
      bx::mtxMul(Rendering::viewMatrix, Rendering::viewMatrix, rotMatrix);
   }

   void SceneCamera::setPosition(Point3F position)
   {
      mCurrent.position = position;
      mTarget.position  = position;
      refresh();
   }

   void SceneCamera::pan(Point3F panDirection)
   {
      Point3F direction(mCos(mCurrent.verticalAngle) * mSin(mCurrent.horizontalAngle),
         mSin(mCurrent.verticalAngle),
         mCos(mCurrent.verticalAngle) * mCos(mCurrent.horizontalAngle));

		Point3F right(mSin(mCurrent.horizontalAngle - bx::piHalf),
			0,
			mCos(mCurrent.horizontalAngle - bx::piHalf));

      mTarget.position -= (direction * panDirection.z) * 0.1f;
      mTarget.position += (right * panDirection.x) * 0.1f;
   }

   // -----------------------------------------------------

   void SceneCamera::mouseMove(Point2I center, Point2I mousePos)
   {
      Point2I delta = center - mousePos;
      if ( delta.isZero() ) return;

      Canvas->setCursorPos(center);

      mTarget.horizontalAngle  -= delta.x * 0.01f;
      mTarget.verticalAngle    += delta.y * 0.01f;
      mTarget.verticalAngle    = mClampF(mTarget.verticalAngle, 4.71f, 7.85f);
   }

   void SceneCamera::onMouseMoveEvent(const GuiEvent &event)
   {
      if ( !mBindMouse || mBindMouseLeftBtn || mBindMouseRightBtn ) return;

      Point2I center(Canvas->getWidth() / 2, Canvas->getHeight() / 2);
      mouseMove(center, event.mousePoint);
   }

   void SceneCamera::onMouseDownEvent(const GuiEvent &event)
   {
      mMouseStartPosition.set(event.mousePoint.x, event.mousePoint.y);
   }

   void SceneCamera::onMouseDraggedEvent(const GuiEvent &event)
   {
      if ( !mBindMouse || !mBindMouseLeftBtn ) return;

      mouseMove(mMouseStartPosition, event.mousePoint);
   }

   void SceneCamera::onRightMouseDownEvent(const GuiEvent &event)
   {
      mMouseStartPosition.set(event.mousePoint.x, event.mousePoint.y);
   }

   void SceneCamera::onRightMouseDraggedEvent(const GuiEvent &event)
   {
      if ( !mBindMouse || !mBindMouseRightBtn ) return;

      mouseMove(mMouseStartPosition, event.mousePoint);
   }

   // -----------------------------------------------------

   void SceneCamera::refresh()
   {
      bx::mtxLookAt(Rendering::viewMatrix, mCurrent.position, mLookAt, mUp);
   }

   void SceneCamera::refreshAngles()
   {
      mCurrent.verticalAngle = mClampF(mCurrent.verticalAngle, 4.71f, 7.85f);

      Point3F direction(mCos(mCurrent.verticalAngle) * mSin(mCurrent.horizontalAngle),
         mSin(mCurrent.verticalAngle),
         mCos(mCurrent.verticalAngle) * mCos(mCurrent.horizontalAngle));
      mDirection = direction;

      Point3F right(mSin(mCurrent.horizontalAngle - bx::piHalf),
         0,
         mCos(mCurrent.horizontalAngle - bx::piHalf));

      mLookAt = mCurrent.position + direction;
      mUp = mCross(right, direction);
      refresh();
   }

   // -----------------------------------------------------

   void SceneCamera::interpolateTick( F32 delta )
   {  

   }

   void SceneCamera::processTick()
   {  
      //
   }

   void SceneCamera::advanceTime( F32 timeDelta )
   {  
      if ( mPanVelocity.len() > 0 )
	      pan(mPanVelocity * timeDelta * 100.0f);

      Point3F positionDiff = mTarget.position - mCurrent.position;
      F32 horizontalDiff   = mTarget.horizontalAngle - mCurrent.horizontalAngle;
      F32 verticalDiff     = mTarget.verticalAngle - mCurrent.verticalAngle;

      if (positionDiff.len() > 0.01f || mFabs(horizontalDiff) > 0.01f || mFabs(verticalDiff) > 0.01f)
      {
         mCurrent.horizontalAngle  += horizontalDiff * timeDelta * 10.0f;
         mCurrent.verticalAngle    += verticalDiff * timeDelta * 10.0f;
         mCurrent.position         += positionDiff * timeDelta * 10.0f;

         refreshAngles();
      }
   }
}
