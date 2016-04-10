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

#ifndef _CONTROLLER_COMPONENT_H_
#define _CONTROLLER_COMPONENT_H_

#ifndef _BASE_COMPONENT_H_
#include "baseComponent.h"
#endif

#ifndef _INPUTLISTENER_H_
#include "input/inputListener.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

#ifndef _PHYSICS_CHARACTER_COMPONENT_H_
#include "scene/components/physics/physicsCharacterComponent.h"
#endif

namespace Scene 
{
   class DLL_PUBLIC ControllerComponent : public BaseComponent, public InputListener, public virtual Tickable
   {
      private:
         typedef BaseComponent Parent;

      protected:
         struct DLL_PUBLIC ControllerState
         {
            Point3F position;
            F32     horizontalAngle;
            F32     verticalAngle;

            ControllerState()
               : position(Point3F::Zero),
               horizontalAngle(0.0f),
               verticalAngle(0.0f)
            { }
         };

         Point3F mLinearVelocity;
         Point3F mForwardVelocity;
         bool    mCaptureMouse;
         bool    mCaptureMouseLeftBtn;
         bool    mCaptureMouseRightBtn;
         bool    mDirty;

         PhysicsCharacterComponent* mPhysicsCharacter;

         // State: used for interpolation
         ControllerState mCurrent;
         ControllerState mTarget;

         // Mouse Tracking
         Point2I mMouseStartPosition;

      public:
         ControllerComponent();

         void onAddToScene();
         void onRemoveFromScene();
         void refresh();

         virtual bool processInputEvent(const InputEvent *event);
         virtual bool processMouseMoveEvent(const MouseMoveEvent *event);
         virtual bool processScreenTouchEvent(const ScreenTouchEvent *event);

         void pan(Point3F direction);
         void setLinearVelocity(Point3F velocity);
         void setForwardVelocity(Point3F velocity);
         void setCaptureMouse(bool value, bool left = false, bool right = false);

         virtual void interpolateTick(F32 delta);
         virtual void processTick();
         virtual void advanceTime(F32 timeDelta);

         static void initPersistFields();
         static bool setCaptureMouseFn(void* obj, const char* data) { static_cast<ControllerComponent*>(obj)->setCaptureMouse(dAtob(data)); return false; }

         DECLARE_CONOBJECT(ControllerComponent);
   };
}

#endif // _CONTROLLER_COMPONENT_H_