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


#include "combinedRendering.h"
#include "console/consoleInternal.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/utilities.h"
#include "3d/scene/core.h"

#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   CombinedRendering* _combinedRenderingInst = NULL;

   void combinedInit()
   {
      if ( _combinedRenderingInst != NULL ) return;
      _combinedRenderingInst = new CombinedRendering();
   }

   void combinedDestroy()
   {
      SAFE_DELETE(_combinedRenderingInst);
   }

   CombinedRendering::CombinedRendering()
   {
      finalShader = Graphics::getShader("final_vs.sc", "final_fs.sc");
      initBuffers();
      setRendering(true);
   }

   CombinedRendering::~CombinedRendering()
   {
      destroyBuffers();
   }

   void CombinedRendering::initBuffers()
   {
      //
   }

   void CombinedRendering::destroyBuffers()
   {
      //
   }

   void CombinedRendering::preRender()
   {
      // 
   }

   void CombinedRendering::render()
   {
      for (U32 n = 0; n < renderCount; ++n)
      {
         RenderData* item = &renderList[n];
         if ( item->deleted ) continue;

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
            {
               if ( item->textures->at(i).isDepthTexture )
                  bgfx::setTexture(i, item->textures->at(i).uniform, Rendering::getDepthTexture());
               else
                  bgfx::setTexture(i, item->textures->at(i).uniform, item->textures->at(i).handle);
            }
         }

         // Setup Uniforms
         if ( !item->uniforms.isEmpty() )
         {
            for (S32 i = 0; i < item->uniforms.uniforms->size(); ++i)
            {
               UniformData* uniform = &item->uniforms.uniforms->at(i);
               bgfx::setUniform(uniform->uniform, uniform->data, uniform->count);
            }
         }

	      // Set render states.
	      bgfx::setState(item->state);

	      // Submit primitive
	      bgfx::submit(item->view);
      }
   }

   void CombinedRendering::postRender()
   {
      // This projection matrix is used because its a full screen quad.
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
      bgfx::setViewTransform(Graphics::ViewTable::RenderLayer2, NULL, proj);
      bgfx::setViewRect(Graphics::ViewTable::RenderLayer2, 0, 0, canvasWidth, canvasHeight);

      // Flip to screen.
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getFinalTexture(), 0);
		bgfx::setProgram(finalShader->mProgram);

		bgfx::setState(0
			| BGFX_STATE_RGB_WRITE
			| BGFX_STATE_ALPHA_WRITE
         | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
			);

      fullScreenQuad(canvasWidth, canvasHeight);
      bgfx::submit(Graphics::ViewTable::RenderLayer2);
   }
}