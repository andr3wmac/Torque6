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

#ifndef _OBJECT_TEMPLATE_H_
#include "scene/objectTemplate.h"
#endif

#ifndef _UTILITY_H_
#include "2d/core/Utility.h"
#endif

#ifndef _SCENE_CORE_H_
#include <scene/scene.h>
#endif

#include "c-interface/c-interface.h"

namespace Scene
{

   ConsoleMethod(SceneObject, findComponent, ConsoleInt, 3, 3, (""))
   {
      StringTableEntry name = StringTable->insert(argv[2]);
      SimObject* result = object->findComponent(name);
      if ( result )
         return result->getId();

      return -1;
   }

   ConsoleMethod(SceneObject, findComponentByType, ConsoleInt, 3, 3, (""))
   {
      SimObject* result = object->findComponentByType(argv[2]);
      if ( result )
         return result->getId();

      return -1;
   }

   ConsoleMethod(SceneObject, getPosition, ConsoleString, 2, 2, (""))
   {
      return Con::getData(TypePoint3F, object->mPosition, 0);
   }

   ConsoleMethod(SceneObject, setPosition, ConsoleVoid, 3, 3, (""))
   {
      Point3F position;
      Con::setData(TypePoint3F, position, 0, 1, &argv[2]);

      object->mPosition = position;
      object->refresh();
   }

   extern "C" {
      DLL_PUBLIC SceneObject* SceneObjectCreateInstance()
      {
         return new SceneObject();
      }

      DLL_PUBLIC void SceneObjectSetTemplate(SceneObject* SceneObject, const char* templatePath)
      {
         SceneObject->setTemplateAsset(templatePath);
      }

      DLL_PUBLIC const char* SceneObjectGetTemplate(SceneObject* SceneObject)
      {
         return CInterface::GetMarshallableString(SceneObject->mTemplateAssetID);
      }

      DLL_PUBLIC void SceneObjectSetPosition(SceneObject* SceneObject, CInterface::Point3FParam position)
      {
         SceneObject->mPosition = position;
         SceneObject->refresh();
      }

      DLL_PUBLIC void SceneObjectGetPosition(SceneObject* SceneObject, CInterface::Point3FParam* outPoint)
      {
         *outPoint = CInterface::Point3FParam(SceneObject->mPosition);
      }

      DLL_PUBLIC void SceneObjectSetRotation(SceneObject* SceneObject, CInterface::Point3FParam rotation)
      {
         SceneObject->mRotation = rotation;
      }

      DLL_PUBLIC void SceneObjectGetRotation(SceneObject* SceneObject, CInterface::Point3FParam* outRotation)
      {
         *outRotation = CInterface::Point3FParam(SceneObject->mRotation);
      }

      DLL_PUBLIC void SceneObjectSetScale(SceneObject* SceneObject, CInterface::Point3FParam scale)
      {
         SceneObject->mScale = scale;
      }

      DLL_PUBLIC void SceneObjectGetScale(SceneObject* SceneObject, CInterface::Point3FParam* outScale)
      {
         *outScale = CInterface::Point3FParam(SceneObject->mScale);
      }

      DLL_PUBLIC SimObject* SceneObjectFindComponent(SceneObject* SceneObject, const char* name)
      {
         return SceneObject->findComponent(StringTable->insert(name));
      }

      DLL_PUBLIC SimObject* SceneObjectFindComponentByType(SceneObject* SceneObject, const char* name)
      {
         return SceneObject->findComponentByType(StringTable->insert(name));
      }
   }
}