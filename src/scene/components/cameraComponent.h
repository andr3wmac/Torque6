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

#ifndef _CAMERA_COMPONENT_H_
#define _CAMERA_COMPONENT_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _VERTEXLAYOUTS_H_
#include "graphics/core.h"
#endif

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

#ifndef _SHADERS_H_
#include "graphics/shaders.h"
#endif

#ifndef _MESH_ASSET_H_
#include "mesh/meshAsset.h"
#endif

#ifndef _RENDERING_H_
#include "rendering/rendering.h"
#endif

#ifndef _BASE_COMPONENT_H_
#include "baseComponent.h"
#endif

#ifndef _RENDER_CAMERA_H_
#include "rendering/renderCamera.h"
#endif

#ifndef NANOVG_H
#include <../common/nanovg/nanovg.h>
#endif

#ifndef _INPUTLISTENER_H_
#include "input/inputListener.h"
#endif

#ifndef _GUICANVAS_H_
#include "gui/guiCanvas.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/tickable.h"
#endif

namespace Scene 
{
   class DLL_PUBLIC CameraComponent : public BaseComponent, public InputListener, public virtual Tickable
   {
      struct DLL_PUBLIC CameraState
      {
         Point3F position;
         F32     horizontalAngle;
         F32     verticalAngle;

         CameraState()
            : position(Point3F::Zero),
              horizontalAngle(0.0f),
              verticalAngle(0.0f)
         { }
      };

      protected:
         typedef BaseComponent Parent;

         bool                       mRender;
         Rendering::RenderCamera*   mRenderCamera;

         // Settings
         bool    mActive;
         Point3F mDirection;
         Point3F mLookAt;
         Point3F mUp;
         Point3F mPanVelocity;
         bool    mBindMouse;
         bool    mBindMouseLeftBtn;
         bool    mBindMouseRightBtn;

         // State: used for interpolation
         CameraState mCurrent;
         CameraState mTarget;

         // Mouse Tracking
         Point2I mMouseStartPosition;

      public:
         CameraComponent();
         ~CameraComponent();

         virtual void onAddToScene();
         virtual void onRemoveFromScene();
         
         void setRender(bool value);
         Rendering::RenderCamera* getRenderCamera();

         virtual bool processInputEvent(const InputEvent *event);
         virtual bool processMouseMoveEvent(const MouseMoveEvent *event);
         virtual bool processScreenTouchEvent(const ScreenTouchEvent *event);

         static void initPersistFields();
         static bool setRender(void* obj, const char* data) { static_cast<CameraComponent*>(obj)->setRender(dAtob(data)); return false; }

         Point3F  getPosition() { return mCurrent.position; }
         F32      getHorizontalAngle() { return mCurrent.horizontalAngle; }
         F32      getVerticalAngle() { return mCurrent.verticalAngle; }
         Point3F  getDirection() { return mDirection; }

         void setHorizontalAngle(F32 angle)
         {
            mCurrent.horizontalAngle = angle;
            mTarget.horizontalAngle = angle;
         }
         void setVerticalAngle(F32 angle)
         {
            mCurrent.verticalAngle = angle;
            mTarget.verticalAngle = angle;
         }

         void setActive(bool val);
         void setPosition(Point3F position);
         void lookAt(Point3F look_at_position);
         void translate(Point3F translation);
         void rotate(Point3F rotation);
         void pan(Point3F direction);
         void setPanVelocity(Point3F velocity) { mPanVelocity = velocity; }

         void refresh();
         void refreshAngles();

         void mouseMove(Point2I center, Point2I mousePos);
         void setBindMouse(bool value, bool left = false, bool right = false)
         {
            mBindMouse = value;
            mBindMouseLeftBtn = left;
            mBindMouseRightBtn = right;
         }

         virtual void onMouseDownEvent(const GuiEvent &event);
         virtual void onMouseMoveEvent(const GuiEvent &event);
         virtual void onMouseDraggedEvent(const GuiEvent &event);
         virtual void onRightMouseDownEvent(const GuiEvent &event);
         virtual void onRightMouseDraggedEvent(const GuiEvent &event);

         virtual void interpolateTick(F32 delta);
         virtual void processTick();
         virtual void advanceTime(F32 timeDelta);

         virtual void copy(CameraComponent* cam);

         DECLARE_CONOBJECT(CameraComponent);
   };
}

#endif // _CAMERA_COMPONENT_H_