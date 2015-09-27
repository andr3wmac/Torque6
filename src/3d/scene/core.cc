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

#include <bgfx/bgfx.h>
#include <bx/fpumath.h>

#include "core_Binding.h"

namespace Scene
{
   typedef HashMap< const char*, SimObjectPtr<SceneCamera> > SceneCameraMap;

   static SimGroup               gSceneEntityGroup;
   static SimGroup               gSceneFeatureGroup;
   static Vector<SceneCamera*>   gActiveCameraList;
   static SceneCameraMap         gCameraMap;

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
      gCameraMap.clear();
      gActiveCameraList.clear();
   }

   void clear()
   {
      gSceneEntityGroup.clear();
      gSceneFeatureGroup.clear();
   }

   void clearGhosted()
   {
      Vector<SceneEntity*> forRemoval;
      for(S32 n = 0; n < gSceneEntityGroup.size(); ++n)
      {
         SceneEntity* entity = dynamic_cast<SceneEntity*>(gSceneEntityGroup.at(n));
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
         gSceneEntityGroup.addObject(group->at(0));
   }

   void save()
   {
      Taml tamlWriter;
      tamlWriter.write(&gSceneEntityGroup, "testScene.taml");
   }

   SimGroup* getEntityGroup()
   {
      return &gSceneEntityGroup;
   }

   void addEntity(SceneEntity* entity, const char* name)
   {
      entity->assignName(name);
      Scene::gSceneEntityGroup.addObject(entity);
   }

   void removeEntity(SceneEntity* entity)
   {
      Scene::gSceneEntityGroup.removeObject(entity);
   }

   SceneCamera* getActiveCamera()
   {
      if (gActiveCameraList.size() < 1 )
         pushActiveCamera("Default");

      return gActiveCameraList[0];
   }

   void pushActiveCamera(const char* name)
   { 
      SceneCamera* camera = getCamera(name);

      if (gActiveCameraList.size() > 0 )
      {
         if (gActiveCameraList[0] == camera )
            return;
         gActiveCameraList[0]->setActive(false);
      }

      gActiveCameraList.push_front(camera);
      gActiveCameraList[0]->setActive(true);
   }

   void popActiveCamera()
   {
      if (gActiveCameraList.size() > 0 )
      {
         gActiveCameraList[0]->setActive(false);
         gActiveCameraList.pop_front();

         if (gActiveCameraList.size() > 0 )
            gActiveCameraList[0]->setActive(true);
      }
   }

   void addCamera(const char* name, SceneCamera* cam)
   {
      if (gCameraMap.find(name) != gCameraMap.end() )
         return;

      gCameraMap.insert(name, cam);
      cam->registerObject();
   }

   SceneCamera* getCamera(const char* name)
   {
      if (gCameraMap.find(name) != gCameraMap.end() )
         return gCameraMap[name];

      // Create new camera.
      SceneCamera* cam = new SceneCamera();
      addCamera(name, cam);
      return cam;
   }

   void refresh()
   {
      for(S32 n = 0; n < gSceneEntityGroup.size(); ++n)
      {
         SceneEntity* entity = dynamic_cast<SceneEntity*>(gSceneEntityGroup.at(n));
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

      for(S32 n = 0; n < gSceneEntityGroup.size(); ++n)
      {
         SceneEntity* entity = dynamic_cast<SceneEntity*>(gSceneEntityGroup.at(n));
         if ( !entity )
            continue;

         F32 collidePoint;
         Point3F collideNormal; 

         F32 distToStart = entity->mBoundingBox.getDistanceToPoint(start);
         if (distToStart != 0.0f && entity->mBoundingBox.collideLine(start, end, &collidePoint, &collideNormal) )
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
      for(S32 n = 0; n < gSceneEntityGroup.size(); ++n)
      {
         SceneEntity* entity = dynamic_cast<SceneEntity*>(gSceneEntityGroup.at(n));
         if ( entity->isGhostable() && entity->mGhosted )
            cr->objectInScope(entity);
      }
   }

   // -------------------------------------------------------------------------------

   void addFeature(SceneFeature* feature)
   {
      Scene::gSceneFeatureGroup.addObject(feature);
   }

   void removeFeature(SceneFeature* feature)
   {
      Scene::gSceneFeatureGroup.removeObject(feature);
   }

   SimGroup* getFeatureGroup()
   {
      return &gSceneFeatureGroup;
   }
}
