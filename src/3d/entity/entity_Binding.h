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

#ifndef _SCENECORE_H_
#include <3d/scene/core.h>
#endif

#include "c-interface/c-interface.h"

namespace Scene
{

   ConsoleMethod(SceneEntity, findComponent, ConsoleInt, 3, 3, (""))
   {
      StringTableEntry name = StringTable->insert(argv[2]);
      SimObject* result = object->findComponent(name);
      if ( result )
         return result->getId();

      return -1;
   }

   ConsoleMethod(SceneEntity, findComponentByType, ConsoleInt, 3, 3, (""))
   {
      SimObject* result = object->findComponentByType(argv[2]);
      if ( result )
         return result->getId();

      return -1;
   }

   ConsoleMethod(SceneEntity, getPosition, ConsoleString, 2, 2, (""))
   {
      return Con::getData(TypePoint3F, object->mPosition, 0);
   }

   ConsoleMethod(SceneEntity, setPosition, ConsoleVoid, 3, 3, (""))
   {
      Point3F position;
      Con::setData(TypePoint3F, position, 0, 1, &argv[2]);

      object->mPosition = position;
      object->refresh();
   }

   extern "C" {
      DLL_PUBLIC SceneEntity* SceneEntityCreateInstance()
      {
         return new SceneEntity();
      }

      DLL_PUBLIC void SceneEntitySetTemplate(SceneEntity* sceneEntity, const char* templatePath)
      {
         sceneEntity->setTemplateAsset(templatePath);
      }

      DLL_PUBLIC const char* SceneEntityGetTemplate(SceneEntity* sceneEntity)
      {
         return CInterface::GetMarshallableString(sceneEntity->mTemplateAssetID);
      }

      DLL_PUBLIC void SceneEntitySetPosition(SceneEntity* sceneEntity, CInterface::Point3FParam position)
      {
         sceneEntity->mPosition = position;
         sceneEntity->refresh();
      }

      DLL_PUBLIC void SceneEntityGetPosition(SceneEntity* sceneEntity, CInterface::Point3FParam* outPoint)
      {
         *outPoint = CInterface::Point3FParam(sceneEntity->mPosition);
      }

      DLL_PUBLIC void SceneEntitySetRotation(SceneEntity* sceneEntity, CInterface::Point3FParam rotation)
      {
         sceneEntity->mRotation = rotation;
      }

      DLL_PUBLIC void SceneEntityGetRotation(SceneEntity* sceneEntity, CInterface::Point3FParam* outRotation)
      {
         *outRotation = CInterface::Point3FParam(sceneEntity->mRotation);
      }

      DLL_PUBLIC void SceneEntitySetScale(SceneEntity* sceneEntity, CInterface::Point3FParam scale)
      {
         sceneEntity->mScale = scale;
      }

      DLL_PUBLIC void SceneEntityGetScale(SceneEntity* sceneEntity, CInterface::Point3FParam* outScale)
      {
         *outScale = CInterface::Point3FParam(sceneEntity->mScale);
      }

      DLL_PUBLIC SimObject* SceneEntityFindComponent(SceneEntity* sceneEntity, const char* name)
      {
         return sceneEntity->findComponent(StringTable->insert(name));
      }

      DLL_PUBLIC SimObject* SceneEntityFindComponentByType(SceneEntity* sceneEntity, const char* name)
      {
         return sceneEntity->findComponentByType(StringTable->insert(name));
      }
   }
}