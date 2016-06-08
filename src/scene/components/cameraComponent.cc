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

#include "console/consoleTypes.h"
#include "cameraComponent.h"
#include "graphics/core.h"
#include "rendering/rendering.h"
#include "scene/scene.h"

// Script bindings.
#include "cameraComponent_Binding.h"

// bgfx/bx
#include <bgfx/bgfx.h>
#include <bx/fpumath.h>

namespace Scene
{
   IMPLEMENT_CONOBJECT(CameraComponent);

   typedef HashMap<StringTableEntry, CameraComponent*> typeActiveCameraHash;
   typeActiveCameraHash activeCameraMap;

   Vector<CameraComponent*> CameraComponent::smCameraComponents;

   CameraComponent::CameraComponent()
   {
      mActive                 = false;
      mRenderCamera           = NULL;
      mRenderCameraName       = StringTable->insert("MainCamera");
      mRenderCameraPriority   = 0;
      mRenderTextureName      = StringTable->EmptyString;
   }

   CameraComponent::~CameraComponent()
   {
      if (mRenderCamera != NULL)
      {
         Rendering::destroyRenderCamera(mRenderCamera);
         mRenderCamera = NULL;
      }
   }

   void CameraComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addGroup("CameraComponent");

         addProtectedField("Active", TypeBool, Offset(mActive, CameraComponent), &CameraComponent::setActive, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
         addProtectedField("RenderCameraName", TypeString, Offset(mRenderCameraName, CameraComponent), &CameraComponent::setRenderCameraName, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
         addProtectedField("RenderCameraPriority", TypeS32, Offset(mRenderCameraPriority, CameraComponent), &CameraComponent::setRenderCameraPriority, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
         addProtectedField("RenderTextureName", TypeString, Offset(mRenderTextureName, CameraComponent), &CameraComponent::setRenderTextureName, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");

      endGroup("CameraComponent");
   }

   void CameraComponent::onAddToScene()
   {  
      addCameraComponent(this);
      refresh();
   }

   void CameraComponent::onRemoveFromScene()
   {
      removeCameraComponent(this);

      if (mRenderCamera != NULL)
      {
         Rendering::destroyRenderCamera(mRenderCamera);
         mRenderCamera = NULL;
      }
   }

   void CameraComponent::setActive(bool value)
   {
      if (mActive == value)
         return;

      mActive = value;

      if (mActive && mRenderCamera == NULL)
      {
         setRenderCameraName(mRenderCameraName);
      }

      if (mActive && mRenderCamera)
      {
         setActiveCamera(this);
      }
   }

   void CameraComponent::setRenderCameraName(const char* name)
   {
      mRenderCameraName = StringTable->insert(name);
      refreshAllCameras();
   }

   void CameraComponent::setRenderTextureName(const char* name)
   {
      mRenderTextureName = StringTable->insert(name);
      refreshAllCameras();
   }

   void CameraComponent::setRenderCameraPriority(S32 priority)
   {
      mRenderCameraPriority = priority;
      refreshAllCameras();
   }

   Rendering::RenderCamera* CameraComponent::getRenderCamera()
   {
      return mRenderCamera;
   }

   // -----------------------------------------------------

   void CameraComponent::refresh()
   {
      Parent::refresh();

      // Attempt to create camera.
      if (mRenderCamera == NULL)
      {
         if (mRenderCameraName == NULL)
            return;

         if (dStrlen(mRenderCameraName) == 0)
            return;

         mRenderCamera = Rendering::createRenderCamera(StringTable->insert(mRenderCameraName), "DeferredShading");
      }
      
      // If camera is still null then we'll reset to default values.
      if (mRenderCamera == NULL)
      {
         mActive                 = false;
         mRenderCameraPriority   = 0;
         mRenderTextureName      = StringTable->EmptyString;
         return;
      }

      // If it's not an active camera then these values are set by another
      // camera component somewhere.
      mActive = isActiveCamera(this);
      if (!mActive)
      {
         mRenderCameraPriority   = mRenderCamera->getRenderPriority();
         mRenderTextureName      = mRenderCamera->getRenderTextureName();
         return;
      }

      // If we are active, set them ourselves.
      mRenderCamera->setRenderPriority((S16)mRenderCameraPriority);
      mRenderCamera->setRenderTextureName(mRenderTextureName);

      VectorF up(0.0f, 0.0f, 1.0f);
      Point3F look;
      Point3F cameraForward(1.0f, 0.0f, 0.0f);

      bx::vec3MulMtx(look, cameraForward, mTransformMatrix);
      bx::mtxLookAt(mRenderCamera->viewMatrix, mWorldPosition, look, up);

      mRenderCamera->position = mWorldPosition;
   }

   void CameraComponent::addCameraComponent(CameraComponent* cam)
   {
      smCameraComponents.push_back(cam);

      typeActiveCameraHash::iterator itr = activeCameraMap.find(cam->mRenderCameraName);
      if (itr == activeCameraMap.end())
      {
         setActiveCamera(cam);
      }
   }

   bool CameraComponent::removeCameraComponent(CameraComponent* cam)
   {
      for (Vector< CameraComponent* >::iterator itr = smCameraComponents.begin(); itr != smCameraComponents.end(); ++itr)
      {
         if ((*itr) == cam)
         {
            if ( isActiveCamera(cam) )
               activeCameraMap.erase(cam->mRenderCameraName);

            smCameraComponents.erase(itr);
            return true;
         }
      }
      return false;
   }

   bool CameraComponent::isActiveCamera(CameraComponent* cam)
   {
      typeActiveCameraHash::iterator itr = activeCameraMap.find(cam->mRenderCameraName);
      if (itr != activeCameraMap.end())
      {
         if ((*itr).value == cam)
            return true;
      }
      return false;
   }

   void CameraComponent::setActiveCamera(CameraComponent* cam)
   {
      activeCameraMap[cam->mRenderCameraName] = cam;
      refreshAllCameras();
   }

   void CameraComponent::refreshAllCameras()
   {
      for (Vector< CameraComponent* >::iterator itr = smCameraComponents.begin(); itr != smCameraComponents.end(); ++itr)
      {
         (*itr)->refresh();
      }
   }
}
