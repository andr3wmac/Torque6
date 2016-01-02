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

#include "scene.h"
#include "camera.h"

#include "console/consoleInternal.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "rendering/rendering.h"
#include "rendering/renderable.h"
#include "scene/object/object.h"

#include <bgfx/bgfx.h>
#include <bx/fpumath.h>

#include "scene_Binding.h"

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
         SceneObject* obj = dynamic_cast<SceneObject*>(gSceneGroup[0]);
         if (obj)
         {
            removeObject(obj);
            obj->deleteObject();
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
      Vector<SceneObject*> forRemoval;
      for(S32 n = 0; n < gSceneGroup.size(); ++n)
      {
         SceneObject* obj = dynamic_cast<SceneObject*>(gSceneGroup.at(n));
         if (obj && obj->mGhosted )
            forRemoval.push_back(obj);
      }

      for(S32 n = 0; n < forRemoval.size(); ++n)
      {
         SceneObject* obj = forRemoval[n];
         //SceneObjectGroup.removeObject(entity);
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

   SimGroup* getSceneGroup()
   {
      return &gSceneGroup;
   }

   Box3F getSceneBounds()
   {
      // TODO: Cache result instead of calculating per call?

      // Calculate bounding box based on SceneObject bounding boxes.
      Box3F sceneBounds;
      sceneBounds.set(Point3F(0, 0, 0));

      for (S32 n = 0; n < gSceneGroup.size(); ++n)
      {
         // Only refresh objects for now.
         SceneObject* obj = dynamic_cast<SceneObject*>(gSceneGroup.at(n));
         if (!obj) 
            continue;
         
         if (n == 0)
            sceneBounds = obj->mBoundingBox;
         else
            sceneBounds.intersect(obj->mBoundingBox);
      }

      return sceneBounds;
   }

   void addObject(SceneObject* obj, const char* name)
   {
      obj->assignUniqueName(name);
      Scene::gSceneGroup.addObject(obj);
   }

   void deleteObject(SceneObject* obj)
   {
      removeObject(obj);
      obj->deleteObject();
   }

   void removeObject(SceneObject* obj)
   {
      Scene::gSceneGroup.removeObject(obj);
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
         // Refresh Objects
         SceneObject* obj = dynamic_cast<SceneObject*>(gSceneGroup.at(n));
         if (obj)
         {
            obj->refresh();
            continue;
         }

         // Refresh Features
         SceneFeature* feature = dynamic_cast<SceneFeature*>(gSceneGroup.at(n));
         if (feature)
         {
            feature->refresh();
            continue;
         }
      }

      getActiveCamera()->refresh();
   }

   SceneObject* raycast(const Point3F& start, const Point3F& end)
   {
      SceneObject* result = NULL;
      Point3F hitPoint = end;
      F32 lastHitDistance = Point3F(hitPoint - start).len();

      for(S32 n = 0; n < gSceneGroup.size(); ++n)
      {
         SceneObject* obj = dynamic_cast<SceneObject*>(gSceneGroup.at(n));
         if ( !obj)
            continue;

         if (obj->raycast(start, end, hitPoint))
         {
            F32 hitDistance = Point3F(hitPoint - start).len();
            if (hitDistance < lastHitDistance)
            {
               lastHitDistance = hitDistance;
               result = obj;
            }
         }
      }

      return result;
   }

   void onCameraScopeQuery(NetConnection *cr, CameraScopeQuery *camInfo)
   {
      for(S32 n = 0; n < gSceneGroup.size(); ++n)
      {
         SceneObject* obj = dynamic_cast<SceneObject*>(gSceneGroup.at(n));
         if (obj != NULL && obj->isGhostable() && obj->mGhosted )
            cr->objectInScope(obj);
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
}
