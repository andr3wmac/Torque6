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

#ifndef _CONSOLE_H_
#include "console/console.h"
#endif

#ifndef _ENTITY_TEMPLATE_H_
#include "3d/entity/entityTemplate.h"
#endif

#ifndef _UTILITY_H_
#include "2d/core/Utility.h"
#endif

#include "c-interface/c-interface.h"

namespace Scene
{

   ConsoleMethod(SceneCamera, lookAt, ConsoleVoid, 3, 3, (""))
   {
      Point3F look_at = Utility::mConvertStringToPoint3F(argv[2]);
      object->lookAt(look_at);
   }

   ConsoleMethod(SceneCamera, translate, ConsoleVoid, 3, 3, (""))
   {
      Point3F translation = Utility::mConvertStringToPoint3F(argv[2]);
      object->translate(translation);
   }

   ConsoleMethod(SceneCamera, pan, ConsoleVoid, 3, 3, (""))
   {
      Point3F pan = Utility::mConvertStringToPoint3F(argv[2]);
      object->pan(pan);
   }

   ConsoleMethod(SceneCamera, setPanVelocity, ConsoleVoid, 3, 3, (""))
   {
      Point3F panVel = Utility::mConvertStringToPoint3F(argv[2]);
      object->setPanVelocity(panVel);
   }

   ConsoleMethod(SceneCamera, setPosition, ConsoleVoid, 3, 3, (""))
   {
      Point3F position = Utility::mConvertStringToPoint3F(argv[2]);
      object->setPosition(position);
   }

   ConsoleMethod(SceneCamera, rotate, ConsoleVoid, 3, 3, (""))
   {
      Point3F rotation = Utility::mConvertStringToPoint3F(argv[2]);
      object->rotate(rotation);
   }

   ConsoleMethod(SceneCamera, bindMouse, ConsoleVoid, 3, 5, (""))
   {
      switch (argc)
      {
      case 3:
         object->setBindMouse(dAtob(argv[2]));
         break;

      case 4:
         object->setBindMouse(dAtob(argv[2]), dAtob(argv[3]));
         break;

      case 5:
         object->setBindMouse(dAtob(argv[2]), dAtob(argv[3]), dAtob(argv[4]));
         break;
      }
   }

   extern "C" {
      DLL_PUBLIC SceneCamera* SceneCameraCreateInstance()
      {
         return new SceneCamera();
      }

      DLL_PUBLIC void SceneCameraLookAt(SceneCamera* camera, CInterface::Point3FParam look_at)
      {
         camera->lookAt(look_at);
      }

      DLL_PUBLIC void SceneCameraTranslate(SceneCamera* camera, CInterface::Point3FParam translation)
      {
         camera->translate(translation);
      }

      DLL_PUBLIC void SceneCameraPan(SceneCamera* camera, CInterface::Point3FParam pan)
      {
         camera->pan(pan);
      }

      DLL_PUBLIC void SceneCameraSetPanVelocity(SceneCamera* camera, CInterface::Point3FParam panVel)
      {
         camera->setPanVelocity(panVel);
      }

      DLL_PUBLIC void SceneCameraRotate(SceneCamera* camera, CInterface::Point3FParam rotation)
      {
         camera->rotate(rotation);
      }

      DLL_PUBLIC void SceneCameraBindMouse(SceneCamera* camera, bool value, bool left, bool right)
      {
         camera->setBindMouse(value, left, right);
      }

      DLL_PUBLIC void SceneCameraGetPosition(SceneCamera* camera, CInterface::Point3FParam* position)
      {
         *position = camera->getPosition();
      }

      DLL_PUBLIC void SceneCameraSetPosition(SceneCamera* camera, CInterface::Point3FParam position)
      {
         camera->setPosition(position);
      }

      DLL_PUBLIC F32 SceneCameraGetHorizontalAngle(SceneCamera* camera)
      {
         return camera->getHorizontalAngle();
      }

      DLL_PUBLIC void SceneCameraSetHorizontalAngle(SceneCamera* camera, F32 angle)
      {
         camera->setHorizontalAngle(angle);
      }

      DLL_PUBLIC F32 SceneCameraGetVerticalAngle(SceneCamera* camera)
      {
         return camera->getVerticalAngle();
      }

      DLL_PUBLIC void SceneCameraSetVerticalAngle(SceneCamera* camera, F32 angle)
      {
         camera->setVerticalAngle(angle);
      }
   }
}