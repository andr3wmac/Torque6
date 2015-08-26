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
#include "core.h"
#endif

#include <3d/material/materialAsset.h>
#include "c-interface/c-interface.h"

ConsoleNamespaceFunction( Scene, clear, ConsoleVoid, 1, 1, (""))
{
   Scene::clear();
}

ConsoleNamespaceFunction( Scene, load, ConsoleVoid, 1, 1, (""))
{
   Scene::load();
}

ConsoleNamespaceFunction( Scene, save, ConsoleVoid, 1, 1, (""))
{
   Scene::save();
}

ConsoleNamespaceFunction( Scene, refresh, ConsoleVoid, 1, 1, (""))
{
   Scene::refresh();
}

ConsoleNamespaceFunction( Scene, addEntity, ConsoleVoid, 2, 3, (""))
{
   SimObject *obj = Sim::findObject(argv[1]);
   if ( !obj ) return;
   Scene::SceneEntity* entity = dynamic_cast<Scene::SceneEntity*>(obj);
   if ( !entity ) return;
   if ( argc > 2 )
      Scene::addEntity(entity, argv[2]);
   else
      Scene::addEntity(entity);
}

ConsoleNamespaceFunction( Scene, removeEntity, ConsoleVoid, 2, 2, (""))
{
   SimObject *obj = Sim::findObject(argv[1]);
   if ( !obj ) return;
   
   Scene::SceneEntity* entity = dynamic_cast<Scene::SceneEntity*>(obj);
   if ( !entity ) return;

   Scene::removeEntity(entity);
}

ConsoleNamespaceFunction( Scene, getActiveCamera, ConsoleInt, 1, 1, (""))
{
   Scene::SceneCamera* cam = Scene::getActiveCamera();
   if ( !cam ) return -1;
   return cam->getId();
}

ConsoleNamespaceFunction( Scene, pushActiveCamera, ConsoleVoid, 2, 2, (""))
{
   Scene::pushActiveCamera(argv[1]);
}

ConsoleNamespaceFunction( Scene, popActiveCamera, ConsoleVoid, 1, 1, (""))
{
   Scene::popActiveCamera();
}

ConsoleNamespaceFunction( Scene, getCamera, ConsoleInt, 2, 2, (""))
{
   Scene::SceneCamera* cam = Scene::getCamera(argv[1]);
   if ( !cam ) return -1;
   return cam->getId();
}

ConsoleNamespaceFunction( Scene, setDirectionalLight, ConsoleVoid, 4, 4, (""))
{
   Point3F direction;
   Con::setData(TypePoint3F, direction, 0, 1, &argv[1]);

   ColorF color;
   Con::setData(TypeColorF, &color, 0, 1, &argv[2]);

   ColorF ambient;
   Con::setData(TypeColorF, &ambient, 0, 1, &argv[3]);

   Scene::setDirectionalLight(direction, color, ambient);
}

namespace Scene{
   extern "C" {
      DLL_PUBLIC void Scene_Clear()
      {
         Scene::clear();
      }

      DLL_PUBLIC void Scene_Load()
      {
         Scene::load();
      }

      DLL_PUBLIC void Scene_Save()
      {
         Scene::save();
      }

      DLL_PUBLIC void Scene_Refresh()
      {
         Scene::refresh();
      }

      DLL_PUBLIC void Scene_AddEntity(SceneEntity* entity, const char* name)
      {
         if (name != NULL)
            Scene::addEntity(entity, name);
         Scene::addEntity(entity);
      }

      DLL_PUBLIC void Scene_RemoveEntity(SceneEntity* entity, const char* name)
      {
         Scene::addEntity(entity);
      }

      DLL_PUBLIC SceneCamera* Scene_GetActiveCamera()
      {
         return Scene::getActiveCamera();
      }

      DLL_PUBLIC void Scene_PushActiveCamera(const char* name)
      {
         Scene::pushActiveCamera(name);
      }

      DLL_PUBLIC void Scene_PopActiveCamera()
      {
         Scene::popActiveCamera();
      }

      DLL_PUBLIC SceneCamera* Scene_GetCamera(const char* name)
      {
         return Scene::getCamera(name);
      }

      DLL_PUBLIC void Scene_SetDirectionalLight(CInterface::Point3FParam direction, CInterface::ColorParam color, CInterface::ColorParam ambient)
      {
         Scene::setDirectionalLight(direction, color, ambient);
      }
   }
}