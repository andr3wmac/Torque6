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
#include "graphics/utilities.h"
#include "3d/rendering/forwardRendering.h"
#include "platform/event.h"
#include "gui/guiCanvas.h"

#include <bgfx.h>
#include <bx/fpumath.h>

#include "camera_ScriptBinding.h"

namespace Scene
{
   IMPLEMENT_CONOBJECT(SceneCamera);

   SceneCamera::SceneCamera()
   {
      mActive = false;
	   setProcessTicks(false);
      mBindMouse = false;
      mBindMouseLeftBtn = false;
      mBindMouseRightBtn = false;
      Con::printf("Scene Camera Created!");
   }

   SceneCamera::~SceneCamera()
   {
      Con::printf("Scene Camera Destroyed!");
   }

   void SceneCamera::setActive(bool val)
   {
      mActive = val;
      setProcessTicks(mActive);
   }

   void SceneCamera::lookAt(Point3F look_at_position)
   {
	   bx::mtxLookAt(Rendering::viewMatrix, mPosition, look_at_position);
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

      mPosition -= (direction * panDirection.z);
      mPosition += (right * panDirection.x);
      refresh();
   }

   void SceneCamera::refresh()
   {
      if ( !mActive ) return;

      mVerticalAngle = mClampF(mVerticalAngle, 4.71f, 7.85f);

      Point3F direction(mCos(mVerticalAngle) * mSin(mHorizontalAngle), 
         mSin(mVerticalAngle), 
         mCos(mVerticalAngle) * mCos(mHorizontalAngle));
      mDirection = direction;

		Point3F right(mSin(mHorizontalAngle - bx::piHalf),
			0,
			mCos(mHorizontalAngle - bx::piHalf));
      
      Point3F cam_at = mPosition + direction;
      Point3F cam_up = mCross(right, direction);
      bx::mtxLookAt(Rendering::viewMatrix, mPosition, cam_at, cam_up);
   }

   void SceneCamera::mouseMove(Point2I center, Point2I mousePos)
   {
      Point2I delta = center - mousePos;
      if ( delta.isZero() ) return;

      Canvas->setCursorPos(center);

      mHorizontalAngle -= delta.x * 0.01f;
      mVerticalAngle += delta.y * 0.01f;
      refresh();
   }

   void SceneCamera::onMouseMoveEvent(const GuiEvent &event)
   {
      if ( !mBindMouse || mBindMouseLeftBtn || mBindMouseRightBtn ) return;

      Point2I center(Canvas->getWidth() / 2, Canvas->getHeight() / 2);
      mouseMove(center, event.mousePoint);
   }

   void SceneCamera::onMouseDownEvent(const GuiEvent &event)
   {
      F32 x = (2.0f * event.mousePoint.x) / Rendering::canvasWidth - 1.0f;
      F32 y = 1.0f - (2.0f * event.mousePoint.y) / Rendering::canvasHeight;
      F32 z = -1.0f;
      Point4F ray_clip(x, y, z, 1.0);

      F32 invProjMtx[16];
      bx::mtxInverse(invProjMtx, Rendering::projectionMatrix);

      Point4F ray_eye;
      bx::vec4MulMtx(ray_eye, ray_clip, invProjMtx);

      F32 invViewMtx[16];
      bx::mtxInverse(invViewMtx, Rendering::viewMatrix);

      Point4F ray_wor;
      bx::vec4MulMtx(ray_wor, ray_eye, invViewMtx);
      Point3F ray_final(ray_wor.x, ray_wor.y, ray_wor.z);
      ray_final.normalize();

      Con::printf("Ray Vector: %f %f %f", ray_final.x, ray_final.y, ray_final.z);

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
	   pan(mPanVelocity * timeDelta * 100.0f);
   }

}