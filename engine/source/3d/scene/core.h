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

#ifndef _SCENECORE_H_
#define _SCENECORE_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _SCENEENTITY_H_
#include "entity.h"
#endif

#ifndef _SCENECAMERA_H_
#include "camera.h"
#endif

#ifndef _PHYSICS_H
#include <physics/physics.h>
#endif

namespace Scene
{
   // Init/Destroy
   void init();
   void destroy();

   // Camera
   extern Vector<SceneCamera*> activeCameraList;
   extern HashMap<const char*, SimObjectPtr<SceneCamera> > cameraList;
   SceneCamera* getActiveCamera();
   void pushActiveCamera(const char* name);
   void popActiveCamera();
   SceneCamera* getCamera(const char* name);

   // Entity Management
   extern SimGroup sceneEntityGroup;
   void addEntity(SceneEntity* entity, const char* name = "SceneEntity");
   void removeEntity(SceneEntity* entity);
   SimGroup* getEntityGroup();
   void refresh();

   // Directional Light
   extern Point3F directionalLightDir;
   extern ColorF  directionalLightColor;
   extern ColorF  directionalLightAmbient;
   void setDirectionalLight(Point3F direction, ColorF color, ColorF ambient);
}

#endif