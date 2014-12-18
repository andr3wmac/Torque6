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

#include "core.h"
#include "entity.h"
#include "camera.h"

#include "console/consoleInternal.h"
#include "graphics/shaders.h"
#include "graphics/utilities.h"
#include "3d/rendering/common.h"

#include <bgfx.h>
#include <bx/fpumath.h>

#include "core_ScriptBinding.h"

namespace Scene
{
   SimGroup sceneEntityGroup;
   F32 viewMatrix[16];
   F32 projectionMatrix[16];
   SceneCamera camera;

   bool canvasSizeChanged = false;
   U32 canvasWidth = 0;
   U32 canvasHeight = 0;
   U32 canvasClearColor = 0;

   // Init/Destroy
   void init()
   {
      Graphics::initUniforms();
      Graphics::initUtilities();

      Rendering::init();

      camera.registerObject();
   }

   void destroy()
   {
      Graphics::destroyUniforms();
      Graphics::destroyUtilities();

      Rendering::destroy();

      camera.unregisterObject();
      sceneEntityGroup.clear();
   }

   // Process Frame
   void render(U32 width, U32 height, U32 clearColor)
   {
      canvasSizeChanged = ( canvasWidth != width || canvasHeight != height );
      canvasWidth = width;
      canvasHeight = height;
      canvasClearColor = clearColor;

      // Calculate Projection Matrix
      // TODO: This should be cached.
	   bx::mtxProj(Scene::projectionMatrix, 60.0f, float(Scene::canvasWidth)/float(Scene::canvasHeight), 0.1f, 1000.0f, true);

      // Render Scene
      Rendering::preRender();
      Rendering::render();
      Rendering::postRender();
   }

   SceneCamera* getCamera()
   {
      return &camera;
   }

   void refresh()
   {
      for(S32 n = 0; n < sceneEntityGroup.size(); ++n)
      {
         SceneEntity* entity = dynamic_cast<SceneEntity*>(sceneEntityGroup.at(n));
         if ( entity )
            entity->refresh();
      }
   }
}