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

#ifndef _CAMERA_COMPONENT_H_
#define _CAMERA_COMPONENT_H_

#ifndef _BASE_COMPONENT_H_
#include "baseComponent.h"
#endif

#ifndef _RENDER_CAMERA_H_
#include "rendering/renderCamera.h"
#endif

namespace Scene 
{
   class DLL_PUBLIC CameraComponent : public BaseComponent
   {
      protected:
         typedef BaseComponent Parent;

         
         StringTableEntry           mRenderTextureName;
         Rendering::RenderCamera*   mRenderCamera;
         StringTableEntry           mRenderCameraName;
         S32                        mRenderCameraPriority;

      public:
         bool mActive;

         CameraComponent();
         ~CameraComponent();

         virtual void onAddToScene();
         virtual void onRemoveFromScene();

         virtual void refresh();
         
         void setActive(bool value);
         void setRenderTextureName(const char* name);
         void setRenderCameraName(const char* name);
         void setRenderCameraPriority(S32 priority);
         Rendering::RenderCamera* getRenderCamera();

         static Vector<CameraComponent*> smCameraComponents;
         static void addCameraComponent(CameraComponent* cam);
         static bool removeCameraComponent(CameraComponent* cam);
         static void setActiveCamera(CameraComponent* cam);
         static bool isActiveCamera(CameraComponent* cam);
         static void refreshAllCameras();

         static void initPersistFields();
         static bool setActive(void* obj, const char* data) { static_cast<CameraComponent*>(obj)->setActive(dAtob(data)); return false; }
         static bool setRenderCameraName(void* obj, const char* data) { static_cast<CameraComponent*>(obj)->setRenderCameraName(data); return false; }
         static bool setRenderCameraPriority(void* obj, const char* data) { static_cast<CameraComponent*>(obj)->setRenderCameraPriority(dAtoi(data)); return false; }
         static bool setRenderTextureName(void* obj, const char* data) { static_cast<CameraComponent*>(obj)->setRenderTextureName(data); return false; }

         DECLARE_CONOBJECT(CameraComponent);
   };
}

#endif // _CAMERA_COMPONENT_H_