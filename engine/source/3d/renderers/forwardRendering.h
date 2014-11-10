#ifndef _FORWARDRENDERING_H_
#define _FORWARDRENDERING_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx.h>
#endif

namespace Scene 
{
   struct ForwardRenderComponent
   {
      bgfx::VertexBufferHandle   vertexBuffer;
      bgfx::IndexBufferHandle    indexBuffer;
      bgfx::TextureHandle        texture;
      bgfx::ProgramHandle        shader;
      F32                        transformMatrix[16];
   };
   extern Vector<ForwardRenderComponent> forwardRenderList;

   void renderForward();
}

#endif