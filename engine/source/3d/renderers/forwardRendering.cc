#include "forwardRendering.h"
#include "console/consoleInternal.h"
#include "graphics/shaders.h"

#include <bgfx.h>

namespace Scene
{
   Vector<ForwardRenderComponent> forwardRenderList;

   void renderForward()
   {
      for (S32 n = 0; n < forwardRenderList.size(); ++n)
      {
         ForwardRenderComponent* item = &forwardRenderList[n];

      	// Set model matrix for rendering.
	      bgfx::setTransform(item->transformMatrix);
         bgfx::setTexture(0, Graphics::Shader::u_texColor, item->texture);
         bgfx::setProgram(item->shader);
	      bgfx::setVertexBuffer(item->vertexBuffer);
	      bgfx::setIndexBuffer(item->indexBuffer);

	      // Set render states.
	      bgfx::setState(BGFX_STATE_DEFAULT);

	      // Submit primitive for rendering to view 0.
	      bgfx::submit(0);
      }
   }

}