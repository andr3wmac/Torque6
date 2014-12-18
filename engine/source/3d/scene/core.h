//-----------------------------------------------------------------------------
// Copyright (c) 2014 Andrew Mac
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

namespace Scene
{
   // Init/Destroy
   void init();
   void destroy();

   // Canvas Information
   extern bool canvasSizeChanged;
   extern U32 canvasWidth;
   extern U32 canvasHeight;
   extern U32 canvasClearColor;

   // Camera
   extern SceneCamera camera;
   SceneCamera* getCamera();

   // View/Projection
   extern F32 viewMatrix[16];
   extern F32 projectionMatrix[16];

   // Entity Management
   extern SimGroup sceneEntityGroup;
   void addEntity(SceneEntity entity);
   void removeEntity(SceneEntity* entity);
   void refresh();

   // Process Frame
   void render(U32 width, U32 height, U32 clearColor = 0);
}

#endif