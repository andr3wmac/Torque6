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

#ifndef _SCENECAMERA_H_
#define _SCENECAMERA_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _SCENEENTITY_H_
#include "sceneEntity.h"
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

namespace Scene
{
   class SceneCamera : public SimObject, public virtual Tickable
   {
      private:
         typedef SimObject Parent;

      protected:
         Point3F mPosition;
         F32     mHorizontalAngle;
         F32     mVerticalAngle;
         bool    mBindMouse;

      public :
         void lookAt(Point3F look_at_position);
         void translate(Point3F translation);
         void rotate(Point3F rotation);
         void setPosition(Point3F position);
         void pan(Point3F direction);
         void refresh();
         void setBindMouse(bool value) { mBindMouse = value; }

         void onMouseMoveEvent(Point2I mouse_pos);

         virtual void interpolateTick( F32 delta );
         virtual void processTick();
         virtual void advanceTime( F32 timeDelta );

         DECLARE_CONOBJECT(SceneCamera);
   };
}

#endif