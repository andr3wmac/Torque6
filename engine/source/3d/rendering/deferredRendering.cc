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


#include "deferredRendering.h"
#include "console/consoleInternal.h"
#include "graphics/shaders.h"

#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   DeferredRenderData deferredRenderList[65535];
   U32 deferredRenderCount = 0;

   DeferredRenderData* getDeferredRenderData()
   {
      DeferredRenderData* item = &deferredRenderList[deferredRenderCount];

      // Reset Values
      item->indexBuffer.idx = bgfx::invalidHandle;
      item->vertexBuffer.idx = bgfx::invalidHandle;
      item->shader.idx = bgfx::invalidHandle;
      item->transformCount = 0;
      item->transformTable = NULL;
      item->uniforms = NULL;
      item->textures = NULL;

      deferredRenderCount++;
      return item;
   }

   void renderDeferred()
   {
      for (U32 n = 0; n < deferredRenderCount; ++n)
      {
         DeferredRenderData* item = &deferredRenderList[n];

         // Transform Table.
         bgfx::setTransform(item->transformTable, item->transformCount);

         // Shader and Buffers
         bgfx::setProgram(item->shader);
	      bgfx::setVertexBuffer(item->vertexBuffer);
	      bgfx::setIndexBuffer(item->indexBuffer);
         
         // Setup Textures
         if ( item->textures )
         {
            for (S32 i = 0; i < item->textures->size(); ++i)
               bgfx::setTexture(i, item->textures->at(i).uniform, item->textures->at(i).handle);
         }

         // Setup Uniforms
         if ( item->uniforms )
         {
            for (S32 i = 0; i < item->uniforms->size(); ++i)
               bgfx::setUniform(item->uniforms->at(i).uniform, item->uniforms->at(i).data, item->uniforms->at(i).count);
         }

	      // Set render states.
	      bgfx::setState(BGFX_STATE_DEFAULT);

	      // Submit primitive for rendering to view 0.
	      bgfx::submit(0);
      }
   }

   // Debug Function
   void dumpDeferredRenderData()
   {
      
      Con::printf("Begin Deferred Render of %d Items", deferredRenderCount);
      for (U32 n = 0; n < deferredRenderCount; ++n)
      {
         DeferredRenderData* item = &deferredRenderList[n];

         // Transform Table.
         Con::printf("Transforms Count: %d Shader: %d Vertex Buffer: %d Index Buffer %d", item->transformCount, item->shader.idx, item->vertexBuffer.idx, item->indexBuffer.idx);
         
         // Setup Textures
         if ( item->textures )
         {
            //for (S32 i = 0; i < item->textures->size(); ++i)
            //   Con::printf("Texture%d: %d", i, item->textures->at(i).handle.idx);
         }

         // Setup Uniforms
         if ( item->uniforms )
         {
            //for (S32 i = 0; i < item->uniforms->size(); ++i)
            //   Con::printf("Uniform%d: %d", i, item->uniforms->at(i).uniform.idx);
         }
      }
   }
}