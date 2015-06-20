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

#include "core.h"
#include "camera.h"

#include "console/consoleInternal.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "3d/rendering/common.h"
#include "3d/rendering/renderable.h"
#include "3d/entity/entity.h"

#include <bgfx.h>
#include <bx/fpumath.h>

#include "core_ScriptBinding.h"

namespace Scene
{
   SimGroup sceneEntityGroup;

   Vector<SceneCamera*> activeCameraList;
   HashMap< const char*, SimObjectPtr<SceneCamera> > cameraList;

   Point3F directionalLightDir;
   ColorF  directionalLightColor;
   ColorF  directionalLightAmbient;

   // Init/Destroy
   void init()
   {
      //
   }

   void destroy()
   {
      clear();
      cameraList.clear();
      activeCameraList.clear();
   }

   void clear()
   {
      sceneEntityGroup.clear();
   }

   void load()
   {
      // Clear old scene.
      clear();

      // Load new scene.
      Taml tamlReader;
      SimGroup* group = tamlReader.read<SimGroup>("testScene.taml");
      while(group->size() > 0)
         sceneEntityGroup.addObject(group->at(0));
   }

   void save()
   {
      Taml tamlWriter;
      tamlWriter.write(&sceneEntityGroup, "testScene.taml");
   }

   SimGroup* getEntityGroup()
   {
      return &sceneEntityGroup;
   }

   void addEntity(SceneEntity* entity, const char* name)
   {
      Scene::sceneEntityGroup.addObject(entity);
   }

   void removeEntity(SceneEntity* entity)
   {
      Scene::sceneEntityGroup.removeObject(entity);
   }

   SceneCamera* getActiveCamera()
   {
      if ( activeCameraList.size() < 1 )
         pushActiveCamera("Default");

      return activeCameraList[0];
   }

   void pushActiveCamera(const char* name)
   { 
      SceneCamera* camera = getCamera(name);

      if ( activeCameraList.size() > 0 )
      {
         if ( activeCameraList[0] == camera )
            return;
         activeCameraList[0]->setActive(false);
      }

      activeCameraList.push_front(camera);
      activeCameraList[0]->setActive(true);
   }

   void popActiveCamera()
   {
      if ( activeCameraList.size() > 0 )
      {
         activeCameraList[0]->setActive(false);
         activeCameraList.pop_front();

         if ( activeCameraList.size() > 0 )
            activeCameraList[0]->setActive(true);
      }
   }

   void addCamera(const char* name, SceneCamera* cam)
   {
      if ( cameraList.find(name) != cameraList.end() )
         return;

      cameraList.insert(name, cam);
      cam->registerObject();
   }

   SceneCamera* getCamera(const char* name)
   {
      if ( cameraList.find(name) != cameraList.end() )
         return cameraList[name];

      // Create new camera.
      SceneCamera* cam = new SceneCamera();
      addCamera(name, cam);
      return cam;
   }

   void refresh()
   {
      for(S32 n = 0; n < sceneEntityGroup.size(); ++n)
      {
         SceneEntity* entity = dynamic_cast<SceneEntity*>(sceneEntityGroup.at(n));
         if ( entity )
            entity->refresh();
      }

      getActiveCamera()->refresh();
   }

   // Directional Light
   void setDirectionalLight(Point3F direction, ColorF color, ColorF ambient)
   {
      directionalLightDir = direction;
      directionalLightColor = color;
      directionalLightAmbient = ambient;
      refresh();
   }

   SceneEntity* raycast(Point3F start, Point3F end)
   {
      SceneEntity* result = NULL;
      F32 resultPoint = 1.0;

      for(S32 n = 0; n < sceneEntityGroup.size(); ++n)
      {
         SceneEntity* entity = dynamic_cast<SceneEntity*>(sceneEntityGroup.at(n));
         if ( !entity )
            continue;

         F32 collidePoint;
         Point3F collideNormal; 

         if ( entity->mBoundingBox.collideLine(start, end, &collidePoint, &collideNormal) )
         {
            if ( collidePoint < resultPoint )
            {
               result = entity;
               resultPoint = collidePoint;
            }
         }
      }

      return result;
   }

   void onCameraScopeQuery(NetConnection *cr, CameraScopeQuery *camInfo)
   {
      for(S32 n = 0; n < sceneEntityGroup.size(); ++n)
      {
         SceneEntity* entity = dynamic_cast<SceneEntity*>(sceneEntityGroup.at(n));
         if ( entity->isGhostable() )
            cr->objectInScope(entity);
      }
   }
}
