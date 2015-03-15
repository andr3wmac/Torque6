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

#ifndef _SCENEENTITY_H_
#include "entity.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _MESH_ASSET_H_
#include "3d/assets/meshAsset.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

#ifndef _GUITYPES_H_
#include <gui/guiTypes.h>
#endif

namespace Scene
{
   class SceneCamera : public SimObject, public virtual Tickable
   {
      private:
         typedef SimObject Parent;

      protected:
         bool    mActive;
         Point3F mDirection;
         Point3F mLookAt;
         Point3F mUp;
         Point3F mPanVelocity;
         Point3F mPosition;
         F32     mHorizontalAngle;
         F32     mVerticalAngle;
         bool    mBindMouse;
         bool    mBindMouseLeftBtn;
         bool    mBindMouseRightBtn;

         Point2I mMouseStartPosition;

      public :
         SceneCamera();
         ~SceneCamera();
		  
         void setActive(bool val);
         Point3F getDirection() { return mDirection; }
         void lookAt(Point3F look_at_position);
         void translate(Point3F translation);
         void rotate(Point3F rotation);
         void setPosition(Point3F position);
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

         void onMouseDownEvent(const GuiEvent &event);
         void onMouseMoveEvent(const GuiEvent &event);
         void onMouseDraggedEvent(const GuiEvent &event);
         void onRightMouseDownEvent(const GuiEvent &event);
         void onRightMouseDraggedEvent(const GuiEvent &event);

         virtual void interpolateTick( F32 delta );
         virtual void processTick();
         virtual void advanceTime( F32 timeDelta );

         DECLARE_CONOBJECT(SceneCamera);
   };
}

#endif