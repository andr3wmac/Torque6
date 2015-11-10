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

   static SimGroup               gSceneGroup;
   static Vector<SceneCamera*>   gActiveCameraList;
   static SceneCameraMap         gCameraMap;

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
      while (gSceneGroup.size() > 0)
      {
         SceneEntity* entity = dynamic_cast<SceneEntity*>(gSceneGroup[0]);
         if (entity)
         {
            removeEntity(entity);
            entity->deleteObject();
            continue;
         }

         SceneFeature* feature = dynamic_cast<SceneFeature*>(gSceneGroup[0]);
         if (feature)
         {
            removeFeature(feature);
            feature->deleteObject();
            continue;
         }    

         // Still here? Failsafe.
         gSceneGroup.removeObject(gSceneGroup[0]);
      }
   }

   void clearGhosted()
   {
      Vector<SceneEntity*> forRemoval;
      for(S32 n = 0; n < gSceneGroup.size(); ++n)
      {
         SceneEntity* entity = dynamic_cast<SceneEntity*>(gSceneGroup.at(n));
         if ( entity && entity->mGhosted )
            forRemoval.push_back(entity);
      }

      for(S32 n = 0; n < forRemoval.size(); ++n)
      {
         SceneEntity* entity = forRemoval[n];
         //sceneEntityGroup.removeObject(entity);
      }
   }

   void load(const char* filename)
   {
      // Clear old scene.
      clear();

      // Load new scene.
      Taml tamlReader;
      SimGroup* group = tamlReader.read<SimGroup>(filename);
      if (group)
      {
         while (group->size() > 0)
            gSceneGroup.addObject(group->at(0));
      }
      refresh();
   }

   void save(const char* filename)
   {
      Taml tamlWriter;
      tamlWriter.write(&gSceneGroup, filename);
   }

   SimGroup* getEntityGroup()
   {
      return &gSceneGroup;
   }

   void addEntity(SceneEntity* entity, const char* name)
   {
      entity->assignUniqueName(name);
      Scene::gSceneGroup.addObject(entity);
   }

   void deleteEntity(SceneEntity* entity)
   {
      removeEntity(entity);
      entity->deleteObject();
   }

   void removeEntity(SceneEntity* entity)
   {
      Scene::gSceneGroup.removeObject(entity);
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

      SimObjectPtr<SceneCamera> camPtr(cam);
      gCameraMap.insert(name, camPtr);
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
      for(S32 n = 0; n < gSceneGroup.size(); ++n)
      {
         // Only refresh entitys for now.
         SceneEntity* entity = dynamic_cast<SceneEntity*>(gSceneGroup.at(n));
         if ( entity )
            entity->refresh();
      }

      getActiveCamera()->refresh();
   }

   SceneEntity* raycast(const Point3F& start, const Point3F& end)
   {
      SceneEntity* result = NULL;
      Point3F hitPoint = end;
      F32 lastHitDistance = Point3F(hitPoint - start).len();

      for(S32 n = 0; n < gSceneGroup.size(); ++n)
      {
         SceneEntity* entity = dynamic_cast<SceneEntity*>(gSceneGroup.at(n));
         if ( !entity )
            continue;

         if (entity->raycast(start, end, hitPoint))
         {
            F32 hitDistance = Point3F(hitPoint - start).len();
            if (hitDistance < lastHitDistance)
            {
               lastHitDistance = hitDistance;
               result = entity;
            }
         }
      }

      return result;
   }

   void onCameraScopeQuery(NetConnection *cr, CameraScopeQuery *camInfo)
   {
      for(S32 n = 0; n < gSceneGroup.size(); ++n)
      {
         SceneEntity* entity = dynamic_cast<SceneEntity*>(gSceneGroup.at(n));
         if (entity != NULL && entity->isGhostable() && entity->mGhosted )
            cr->objectInScope(entity);
      }
   }

   // -------------------------------------------------------------------------------

   void addFeature(SceneFeature* feature)
   {
      Scene::gSceneGroup.addObject(feature);
   }

   void deleteFeature(SceneFeature* feature)
   {
      removeFeature(feature);
      feature->deleteObject();
   }

   void removeFeature(SceneFeature* feature)
   {
      Scene::gSceneGroup.removeObject(feature);
      feature->setActive(false);
   }

   SimGroup* getFeatureGroup()
   {
      return &gSceneGroup;
   }
}
