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
#include "cameraComponent.h"
#include "graphics/core.h"
#include "rendering/rendering.h"
#include "scene/scene.h"
#include "gui/guiCanvas.h"

// Script bindings.
#include "cameraComponent_Binding.h"

// Debug Profiling.
#include "debug/profiler.h"

// bgfx/bx
#include <bgfx/bgfx.h>
#include <bx/fpumath.h>

// Assimp - Asset Import Library
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

namespace Scene
{
   IMPLEMENT_CONOBJECT(CameraComponent);

   CameraComponent::CameraComponent()
    : mDirection(Point3F::Zero),
      mLookAt(Point3F::Zero),
      mUp(Point3F::Zero),
      mPanVelocity(Point3F::Zero),
      mActive(false),
      mBindMouse(false),
      mBindMouseLeftBtn(false),
      mBindMouseRightBtn(false)
   {
      setProcessTicks(false);
      mRender        = false;
      mRenderCamera  = NULL;

      setBindMouse(true);
   }

   CameraComponent::~CameraComponent()
   {
      if (mRenderCamera != NULL)
         SAFE_DELETE(mRenderCamera);
   }

   void CameraComponent::copy(CameraComponent* cam)
   {
      mCurrent = cam->mCurrent;
      mTarget = cam->mTarget;
      mDirection = cam->mDirection;
      mLookAt = cam->mLookAt;
      mUp = cam->mUp;
      mPanVelocity = cam->mPanVelocity;
      refreshAngles();
   }

   void CameraComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addGroup("CameraComponent");

         addProtectedField("Render", TypeBool, Offset(mRender, CameraComponent), &CameraComponent::setRender, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
         addField("Position", TypePoint3F, Offset(mCurrent.position, CameraComponent), "");
         addField("HorizontalAngle", TypeF32, Offset(mCurrent.horizontalAngle, CameraComponent), "");
         addField("VerticalAngle", TypeF32, Offset(mCurrent.verticalAngle, CameraComponent), "");

      endGroup("CameraComponent");
   }

   void CameraComponent::onAddToScene()
   {  
      refresh();
   }

   void CameraComponent::onRemoveFromScene()
   {
      //
   }

   void CameraComponent::setRender(bool value)
   {
      if (mRender == value)
         return;

      mRender = value;

      // Destroy render camera
      if (mRenderCamera != NULL)
         SAFE_DELETE(mRenderCamera);

      // Activate/deactive input listening
      setProcessTicks(value);
      setListening(value);

      // Activating rendering
      if (mRender)
      {
         mRenderCamera = new Rendering::RenderCamera();
         Rendering::addRenderCamera(mRenderCamera);
      }
   }

   Rendering::RenderCamera* CameraComponent::getRenderCamera()
   {
      return mRenderCamera;
   }

   bool CameraComponent::processInputEvent(const InputEvent* event)
   {
      return false;
   }

   bool CameraComponent::processMouseMoveEvent(const MouseMoveEvent *event)
   {
      if (!mBindMouse || mBindMouseLeftBtn || mBindMouseRightBtn) return false;

      Point2I center(Canvas->getWidth() / 2, Canvas->getHeight() / 2);
      mouseMove(center, Point2I(event->xPos, event->yPos));
      return true;
   }

   bool CameraComponent::processScreenTouchEvent(const ScreenTouchEvent *event)
   {
      return false;
   }

   // -----------------------------------------------------

   void CameraComponent::lookAt(Point3F look_at_position)
   {
      mLookAt = look_at_position;
      mUp.set(0, 0, 1);
      refresh();
   }

   void CameraComponent::translate(Point3F translation)
   {
      mTarget.position -= translation;
   }

   void CameraComponent::rotate(Point3F translation)
   {
      Rendering::RenderCamera* camera = Rendering::getActiveCamera();
      if (camera)
      {
         F32 rotMatrix[16];
         bx::mtxRotateXYZ(rotMatrix, translation.x, translation.y, translation.z);
         bx::mtxMul(camera->viewMatrix, camera->viewMatrix, rotMatrix);
      }
   }

   void CameraComponent::setPosition(Point3F position)
   {
      mCurrent.position = position;
      mTarget.position = position;
      refresh();
   }

   void CameraComponent::pan(Point3F panDirection)
   {
      Point3F direction(mCos(mCurrent.verticalAngle) * mSin(mCurrent.horizontalAngle),
         mCos(mCurrent.verticalAngle) * mCos(mCurrent.horizontalAngle),
         mSin(mCurrent.verticalAngle));

      Point3F right(mSin(mCurrent.horizontalAngle - bx::piHalf),
         mCos(mCurrent.horizontalAngle - bx::piHalf),
         0);

      mTarget.position -= (direction * panDirection.y) * 0.1f;
      mTarget.position += (right * panDirection.x) * 0.1f;
   }

   // -----------------------------------------------------

   void CameraComponent::mouseMove(Point2I center, Point2I mousePos)
   {
      Point2I delta = center - mousePos;
      if (delta.isZero()) return;

      Canvas->setCursorPos(center);

      mTarget.horizontalAngle += delta.x * 0.01f;
      mTarget.verticalAngle -= delta.y * 0.01f;
      mTarget.verticalAngle = mClampF(mTarget.verticalAngle, -4.7f, -1.7f);
   }

   void CameraComponent::onMouseMoveEvent(const GuiEvent &event)
   {
      if (!mBindMouse || mBindMouseLeftBtn || mBindMouseRightBtn) return;

      Point2I center(Canvas->getWidth() / 2, Canvas->getHeight() / 2);
      mouseMove(center, event.mousePoint);
   }

   void CameraComponent::onMouseDownEvent(const GuiEvent &event)
   {
      mMouseStartPosition.set(event.mousePoint.x, event.mousePoint.y);
   }

   void CameraComponent::onMouseDraggedEvent(const GuiEvent &event)
   {
      if (!mBindMouse || !mBindMouseLeftBtn) return;

      mouseMove(mMouseStartPosition, event.mousePoint);
   }

   void CameraComponent::onRightMouseDownEvent(const GuiEvent &event)
   {
      mMouseStartPosition.set(event.mousePoint.x, event.mousePoint.y);
   }

   void CameraComponent::onRightMouseDraggedEvent(const GuiEvent &event)
   {
      if (!mBindMouse || !mBindMouseRightBtn) return;

      mouseMove(mMouseStartPosition, event.mousePoint);
   }

   // -----------------------------------------------------

   void CameraComponent::refresh()
   {
      Rendering::RenderCamera* camera = Rendering::getActiveCamera();
      if (camera)
         bx::mtxLookAt(camera->viewMatrix, mCurrent.position, mLookAt, mUp);
   }

   void CameraComponent::refreshAngles()
   {
      mCurrent.verticalAngle = mClampF(mCurrent.verticalAngle, -4.7f, -1.7f);

      Point3F direction(mCos(mCurrent.verticalAngle) * mSin(mCurrent.horizontalAngle),
         mCos(mCurrent.verticalAngle) * mCos(mCurrent.horizontalAngle),
         mSin(mCurrent.verticalAngle));
      mDirection = direction;

      Point3F right(mSin(mCurrent.horizontalAngle - bx::piHalf),
         mCos(mCurrent.horizontalAngle - bx::piHalf),
         0);

      mLookAt = mCurrent.position + direction;
      mUp = mCross(right, direction);
      refresh();
   }

   // -----------------------------------------------------

   void CameraComponent::interpolateTick(F32 delta)
   {
      //
   }

   void CameraComponent::processTick()
   {
      if (mPanVelocity.len() > 0.0f)
         pan(mPanVelocity * 10.0f);
   }

   void CameraComponent::advanceTime(F32 timeDelta)
   {
      Point3F positionDiff = mTarget.position - mCurrent.position;
      F32 horizontalDiff = mTarget.horizontalAngle - mCurrent.horizontalAngle;
      F32 verticalDiff = mTarget.verticalAngle - mCurrent.verticalAngle;

      if (positionDiff.len() > 0.01f || mFabs(horizontalDiff) > 0.01f || mFabs(verticalDiff) > 0.01f)
      {
         mCurrent.horizontalAngle += horizontalDiff * timeDelta * 10.0f;
         mCurrent.verticalAngle += verticalDiff * timeDelta * 10.0f;
         mCurrent.position += positionDiff * timeDelta * 10.0f;

         refreshAngles();
      }
   }
}
