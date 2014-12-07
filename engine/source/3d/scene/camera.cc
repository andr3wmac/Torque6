//-----------------------------------------------------------------------------
// Copyright (c) 2014 Andrew Mac
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
#include "sceneCore.h"

#include "console/consoleInternal.h"
#include "graphics/shaders.h"
#include "graphics/vertexLayouts.h"
#include "rendering/forwardRendering.h"
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
	   setProcessTicks(true);
   }

   void SceneCamera::lookAt(Point3F look_at_position)
   {
	   bx::mtxLookAt(viewMatrix, mPosition, look_at_position);
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
      bx::mtxMul(viewMatrix, viewMatrix, rotMatrix);
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
      Point3F direction(mCos(mVerticalAngle) * mSin(mHorizontalAngle), 
         mSin(mVerticalAngle), 
         mCos(mVerticalAngle) * mCos(mHorizontalAngle));

		Point3F right(mSin(mHorizontalAngle - bx::piHalf),
			0,
			mCos(mHorizontalAngle - bx::piHalf));

      Point3F cam_at = mPosition + direction;
      Point3F cam_up = mCross(right, direction);
      bx::mtxLookAt(viewMatrix, mPosition, cam_at, cam_up);
   }

   void SceneCamera::onMouseMoveEvent(Point2I mouse_pos)
   {
      if ( !mBindMouse ) return;
      Point2I center(Canvas->getWidth() / 2, Canvas->getHeight() / 2);
      Point2I delta = center - mouse_pos;
      if ( delta.isZero() ) return;

      //Con::printf("Mouse Moved: %d %d", delta.x, delta.y);
      Canvas->setCursorPos(center);

      mHorizontalAngle -= delta.x * 0.01f;
      mVerticalAngle += delta.y * 0.01f;
      refresh();
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
      //Con::printf("Animation Component advanceTime");
	  pan(mPanVelocity * timeDelta * 100.0f);
   }

}