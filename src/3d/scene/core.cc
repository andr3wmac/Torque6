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

#include "core_Binding.h"

namespace Scene
{
   typedef HashMap< const char*, SimObjectPtr<SceneCamera> > SceneCameraMap;

   static SimGroup               s_sceneEntityGroup;
   static SimGroup               s_sceneFeatureGroup;
   static Vector<SceneCamera*>   s_activeCameraList;
   static SceneCameraMap         s_cameraMap;

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
      s_cameraMap.clear();
      s_activeCameraList.clear();
   }

   void clear()
   {
      s_sceneEntityGroup.clear();
      s_sceneFeatureGroup.clear();
   }

   void clearGhosted()
   {
      Vector<SceneEntity*> forRemoval;
      for(S32 n = 0; n < s_sceneEntityGroup.size(); ++n)
      {
         SceneEntity* entity = dynamic_cast<SceneEntity*>(s_sceneEntityGroup.at(n));
         if ( entity && entity->mGhosted )
            forRemoval.push_back(entity);
      }

      for(S32 n = 0; n < forRemoval.size(); ++n)
      {
         SceneEntity* entity = forRemoval[n];
         //sceneEntityGroup.removeObject(entity);
      }
   }

   void load()
   {
      // Clear old scene.
      clear();

      // Load new scene.
      Taml tamlReader;
      SimGroup* group = tamlReader.read<SimGroup>("testScene.taml");
      while(group->size() > 0)
         s_sceneEntityGroup.addObject(group->at(0));
   }

   void save()
   {
      Taml tamlWriter;
      tamlWriter.write(&s_sceneEntityGroup, "testScene.taml");
   }

   SimGroup* getEntityGroup()
   {
      return &s_sceneEntityGroup;
   }

   void addEntity(SceneEntity* entity, const char* name)
   {
      Scene::s_sceneEntityGroup.addObject(entity);
   }

   void removeEntity(SceneEntity* entity)
   {
      Scene::s_sceneEntityGroup.removeObject(entity);
   }

   SceneCamera* getActiveCamera()
   {
      if (s_activeCameraList.size() < 1 )
         pushActiveCamera("Default");

      return s_activeCameraList[0];
   }

   void pushActiveCamera(const char* name)
   { 
      SceneCamera* camera = getCamera(name);

      if (s_activeCameraList.size() > 0 )
      {
         if (s_activeCameraList[0] == camera )
            return;
         s_activeCameraList[0]->setActive(false);
      }

      s_activeCameraList.push_front(camera);
      s_activeCameraList[0]->setActive(true);
   }

   void popActiveCamera()
   {
      if (s_activeCameraList.size() > 0 )
      {
         s_activeCameraList[0]->setActive(false);
         s_activeCameraList.pop_front();

         if (s_activeCameraList.size() > 0 )
            s_activeCameraList[0]->setActive(true);
      }
   }

   void addCamera(const char* name, SceneCamera* cam)
   {
      if (s_cameraMap.find(name) != s_cameraMap.end() )
         return;

      s_cameraMap.insert(name, cam);
      cam->registerObject();
   }

   SceneCamera* getCamera(const char* name)
   {
      if (s_cameraMap.find(name) != s_cameraMap.end() )
         return s_cameraMap[name];

      // Create new camera.
      SceneCamera* cam = new SceneCamera();
      addCamera(name, cam);
      return cam;
   }

   void refresh()
   {
      for(S32 n = 0; n < s_sceneEntityGroup.size(); ++n)
      {
         SceneEntity* entity = dynamic_cast<SceneEntity*>(s_sceneEntityGroup.at(n));
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

      for(S32 n = 0; n < s_sceneEntityGroup.size(); ++n)
      {
         SceneEntity* entity = dynamic_cast<SceneEntity*>(s_sceneEntityGroup.at(n));
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
      for(S32 n = 0; n < s_sceneEntityGroup.size(); ++n)
      {
         SceneEntity* entity = dynamic_cast<SceneEntity*>(s_sceneEntityGroup.at(n));
         if ( entity->isGhostable() && entity->mGhosted )
            cr->objectInScope(entity);
      }
   }

   // -------------------------------------------------------------------------------

   void addFeature(SceneFeature* feature)
   {
      Scene::s_sceneFeatureGroup.addObject(feature);
   }

   void removeFeature(SceneFeature* feature)
   {
      Scene::s_sceneFeatureGroup.removeObject(feature);
   }
}
