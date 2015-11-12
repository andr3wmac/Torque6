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
#include "3d/scene/object/objectTemplate.h"
#endif

#ifndef _UTILITY_H_
#include "2d/core/Utility.h"
#endif

#ifndef _SCENE_CORE_H_
#include "scene.h"
#endif

#include <3d/material/materialAsset.h>
#include "c-interface/c-interface.h"

ConsoleNamespaceFunction( Scene, clear, ConsoleVoid, 1, 1, (""))
{
   Scene::clear();
}

ConsoleNamespaceFunction( Scene, load, ConsoleVoid, 2, 2, (""))
{
   Scene::load(argv[1]);
}

ConsoleNamespaceFunction( Scene, save, ConsoleVoid, 2, 2, (""))
{
   Scene::save(argv[1]);
}

ConsoleNamespaceFunction( Scene, refresh, ConsoleVoid, 1, 1, (""))
{
   Scene::refresh();
}

ConsoleNamespaceFunction( Scene, addObject, ConsoleVoid, 2, 3, (""))
{
   SimObject *obj = Sim::findObject(argv[1]);
   if ( !obj ) return;
   Scene::SceneObject* entity = dynamic_cast<Scene::SceneObject*>(obj);
   if ( !entity ) return;
   if ( argc > 2 )
      Scene::addObject(entity, argv[2]);
   else
      Scene::addObject(entity);
}

ConsoleNamespaceFunction( Scene, removeObject, ConsoleVoid, 2, 2, (""))
{
   SimObject *obj = Sim::findObject(argv[1]);
   if ( !obj ) return;
   
   Scene::SceneObject* entity = dynamic_cast<Scene::SceneObject*>(obj);
   if ( !entity ) return;

   Scene::removeObject(entity);
}

ConsoleNamespaceFunction(Scene, addFeature, ConsoleVoid, 2, 2, (""))
{
   SimObject *obj = Sim::findObject(argv[1]);
   if (!obj) return;
   Scene::SceneFeature* feature = dynamic_cast<Scene::SceneFeature*>(obj);
   if (!feature) return;

   Scene::addFeature(feature);
}

ConsoleNamespaceFunction(Scene, removeFeature, ConsoleVoid, 2, 2, (""))
{
   SimObject *obj = Sim::findObject(argv[1]);
   if (!obj) return;
   Scene::SceneFeature* feature = dynamic_cast<Scene::SceneFeature*>(obj);
   if (!feature) return;

   Scene::removeFeature(feature);
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

namespace Scene{
   extern "C" {
      DLL_PUBLIC void Scene_Clear()
      {
         Scene::clear();
      }

      DLL_PUBLIC void Scene_Load(const char* filename)
      {
         Scene::load(filename);
      }

      DLL_PUBLIC void Scene_Save(const char* filename)
      {
         Scene::save(filename);
      }

      DLL_PUBLIC void Scene_Refresh()
      {
         Scene::refresh();
      }

      DLL_PUBLIC void Scene_AddObject(SceneObject* entity, const char* name)
      {
         if (name != NULL)
            Scene::addObject(entity, name);
         Scene::addObject(entity);
      }

      DLL_PUBLIC void Scene_RemoveObject(SceneObject* entity, const char* name)
      {
         Scene::addObject(entity);
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
   }
}