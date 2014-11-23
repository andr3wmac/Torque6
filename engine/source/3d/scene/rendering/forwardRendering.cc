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


#include "forwardRendering.h"
#include "console/consoleInternal.h"
#include "graphics/shaders.h"

#include <bgfx.h>
#include <bx/fpumath.h>

namespace Scene
{
   Vector<ForwardRenderData> forwardRenderList;

   void renderForward()
   {
      for (S32 n = 0; n < forwardRenderList.size(); ++n)
      {
         ForwardRenderData* item = &forwardRenderList[n];

         // Temporary retarded matrix thing. I just want it to work.
         F32 holyMatrix[50][16];
         memcpy(holyMatrix[0], item->transformMatrix, sizeof(item->transformMatrix));
         memcpy(holyMatrix[1], item->boneTransforms[0], sizeof(item->boneTransforms[0]) * 49);
         bgfx::setTransform(holyMatrix, 50);

         bgfx::setProgram(item->shader);
	      bgfx::setVertexBuffer(item->vertexBuffer);
	      bgfx::setIndexBuffer(item->indexBuffer);

         // Setup Textures
         for (S32 i = 0; i < item->textures.size(); ++i)
            bgfx::setTexture(i, item->textureUniforms[i], item->textures[i]);

	      // Set render states.
	      bgfx::setState(BGFX_STATE_DEFAULT);

	      // Submit primitive for rendering to view 0.
	      bgfx::submit(0);
      }
   }

}