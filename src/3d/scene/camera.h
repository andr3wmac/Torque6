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

#ifndef _SCENECAMERA_H_
#define _SCENECAMERA_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _MESH_ASSET_H_
#include "3d/entity/meshAsset.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

#ifndef _GUITYPES_H_
#include <gui/guiTypes.h>
#endif

namespace Scene
{
   class DLL_PUBLIC SceneCamera : public SimObject, public virtual Tickable
   {
      struct DLL_PUBLIC SceneCameraState
      {
         Point3F position;
         F32     horizontalAngle;
         F32     verticalAngle;

         SceneCameraState()
            :  position(Point3F::Zero),
               horizontalAngle(0.0f),
               verticalAngle(0.0f)
         { }
      };

      private:
         typedef SimObject Parent;

      protected:
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
         SceneCameraState mCurrent;
         SceneCameraState mTarget;

         // Mouse Tracking
         Point2I mMouseStartPosition;

      public :
         SceneCamera();
         ~SceneCamera();
		  
         Point3F  getPosition()        { return mCurrent.position; }
         F32      getHorizontalAngle() { return mCurrent.horizontalAngle; }
         F32      getVerticalAngle()   { return mCurrent.verticalAngle; }
         Point3F  getDirection()       { return mDirection; }

         void setHorizontalAngle(F32 angle) 
         { 
            mCurrent.horizontalAngle   = angle;
            mTarget.horizontalAngle    = angle;
         }
         void setVerticalAngle(F32 angle) 
         { 
            mCurrent.verticalAngle  = angle;
            mTarget.verticalAngle   = angle;
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

         virtual void interpolateTick( F32 delta );
         virtual void processTick();
         virtual void advanceTime( F32 timeDelta );

         virtual void copy(SceneCamera* cam);

         static void initPersistFields();

         DECLARE_CONOBJECT(SceneCamera);
   };
}

#endif