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

#include "sceneCore.h"
#include "sceneEntity.h"
#include "camera.h"

#include "console/consoleInternal.h"
#include "graphics/shaders.h"
#include "graphics/utilities.h"
#include "rendering/forwardRendering.h"

#include <bgfx.h>
#include <bx/fpumath.h>

#include "sceneCore_ScriptBinding.h"

namespace Scene
{
   SimGroup sceneEntityGroup;
   F32 viewMatrix[16];
   F32 projectionMatrix[16];
   SceneCamera camera;

   // Init/Destroy
   void init()
   {
      Graphics::initShaderUniforms();
      Graphics::initUtilities();

      camera.registerObject();
      //testGetNearestLights();
      //testGetNearestLights();
      //testGetNearestLights();
   }

   void destroy()
   {
      camera.unregisterObject();
      sceneEntityGroup.clear();
   }

   // Process Frame
   void render(U32 width, U32 height, U32 clearColor)
   {
      // Clear Frame
      bgfx::setViewClear(0
		   , BGFX_CLEAR_COLOR_BIT | BGFX_CLEAR_DEPTH_BIT
		   , clearColor
		   , 1.0f
		   , 0
	   );

      // Dummy submit to ensure viewport is cleared.
      bgfx::submit(0);

      // Calculate Projection Matrix
      // TODO: This should be cached.
	   bx::mtxProj(projectionMatrix, 60.0f, float(width)/float(height), 0.1f, 1000.0f);

      // Setup Camera/View
      bgfx::setViewTransform(0, viewMatrix, projectionMatrix);
      bgfx::setViewRect(0, 0, 0, width, height);

      // Render Scene
      renderForward();
   }

   SceneCamera* getCamera()
   {
      return &camera;
   }
}