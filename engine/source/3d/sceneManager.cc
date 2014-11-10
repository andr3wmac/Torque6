#include "sceneManager.h"
#include "sceneEntity.h"

#include "console/consoleInternal.h"
#include "graphics/shaders.h"
#include "graphics/vertexLayouts.h"
#include "renderers/forwardRendering.h"

#include <bgfx.h>
#include <bx/fpumath.h>

#include "sceneManager_ScriptBinding.h"

namespace Scene
{
   Vector<SceneEntity> entityList;

   // Init/Destroy
   void init()
   {
      Graphics::initShaderUniforms();
      Graphics::initVertexLayouts();

      //cameraLookAt(Point3F(0, 45, 75), Point3F(0, 45, 0));
   }

   void destroy()
   {

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
	   bx::mtxProj(projectionMatrix, 60.0f, float(width)/float(height), 0.1f, 200.0f);

      // Setup Camera/View
      bgfx::setViewTransform(0, viewMatrix, projectionMatrix);
	   bgfx::setViewRect(0, 0, 0, width, height);

      // Render Scene
      renderForward();
   }

   // Camera/View Controls
   F32 viewMatrix[16];
   F32 projectionMatrix[16];
   void cameraLookAt(Point3F eye_position, Point3F look_at_position)
   {
	   bx::mtxLookAt(viewMatrix, eye_position, look_at_position);
   }
}