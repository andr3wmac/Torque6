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
   {
      mHorizontalAngle = 0.0f;
      mVerticalAngle = 0.0f;

      mDirection = Point3F::Zero;
      mLookAt = Point3F::Zero;
      mUp = Point3F::Zero;
      mPosition = Point3F::Zero;
      mPanVelocity = Point3F::Zero;

      mActive = false;
      setProcessTicks(false);
      mBindMouse = false;
      mBindMouseLeftBtn = false;
      mBindMouseRightBtn = false;
      //Con::printf("Scene Camera Created!");
   }

   SceneCamera::~SceneCamera()
   {
      //Con::printf("Scene Camera Destroyed!");
   }

   void SceneCamera::initPersistFields()
   {
       // Call parent.
       Parent::initPersistFields();

       addField("Position",         TypePoint3F,   Offset(mPosition, SceneCamera), "");
       addField("HorizontalAngle",  TypeF32,       Offset(mHorizontalAngle, SceneCamera), "");
       addField("VerticalAngle",    TypeF32,       Offset(mVerticalAngle, SceneCamera), "");
   }

   void SceneCamera::setActive(bool val)
   {
      mActive = val;
      setProcessTicks(mActive);
   }

   void SceneCamera::lookAt(Point3F look_at_position)
   {
      mLookAt = look_at_position;
      mUp.set(0, 1, 0);
      refresh();
   }

   void SceneCamera::translate(Point3F translation)
   {
      mPosition -= translation;
      refresh();
   }

   void SceneCamera::rotate(Point3F translation)
   {
      F32 rotMatrix[16];
      bx::mtxRotateXYZ(rotMatrix, translation.x, translation.y, translation.z);
      bx::mtxMul(Rendering::viewMatrix, Rendering::viewMatrix, rotMatrix);
   }

   void SceneCamera::setPosition(Point3F position)
   {
      mPosition = position;
      refresh();
   }

   void SceneCamera::pan(Point3F panDirection)
   {
      Point3F direction(mCos(mVerticalAngle) * mSin(mHorizontalAngle), 
         mSin(mVerticalAngle), 
         mCos(mVerticalAngle) * mCos(mHorizontalAngle));

		Point3F right(mSin(mHorizontalAngle - bx::piHalf),
			0,
			mCos(mHorizontalAngle - bx::piHalf));

      mPosition -= (direction * panDirection.z) * 0.1f;
      mPosition += (right * panDirection.x) * 0.1f;
      refreshAngles();
   }

   void SceneCamera::refresh()
   {
      //if ( !mActive ) return;

      bx::mtxLookAt(Rendering::viewMatrix, mPosition, mLookAt, mUp);
   }

   void SceneCamera::refreshAngles()
   {
      mVerticalAngle = mClampF(mVerticalAngle, 4.71f, 7.85f);

      Point3F direction(mCos(mVerticalAngle) * mSin(mHorizontalAngle), 
         mSin(mVerticalAngle), 
         mCos(mVerticalAngle) * mCos(mHorizontalAngle));
      mDirection = direction;

		Point3F right(mSin(mHorizontalAngle - bx::piHalf),
			0,
			mCos(mHorizontalAngle - bx::piHalf));
      
      mLookAt = mPosition + direction;
      mUp = mCross(right, direction);
      refresh();
   }

   void SceneCamera::mouseMove(Point2I center, Point2I mousePos)
   {
      Point2I delta = center - mousePos;
      if ( delta.isZero() ) return;

      Canvas->setCursorPos(center);

      mHorizontalAngle -= delta.x * 0.01f;
      mVerticalAngle += delta.y * 0.01f;
      refreshAngles();
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
   }

   void SceneCamera::copy(SceneCamera* cam)
   {
      mHorizontalAngle = cam->mHorizontalAngle;
      mVerticalAngle = cam->mVerticalAngle;
      mDirection = cam->mDirection;
      mLookAt = cam->mLookAt;
      mUp = cam->mUp;
      mPosition = cam->mPosition;
      mPanVelocity = cam->mPanVelocity;
      refreshAngles();
   }

}
