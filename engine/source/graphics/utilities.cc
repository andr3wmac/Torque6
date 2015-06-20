//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
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

#include "graphics/core.h"
#include "graphics/core.h"
#include <bgfx.h>

namespace Graphics
{
   bgfx::VertexDecl PosUVVertex::ms_decl;
   bgfx::VertexDecl PosUVNormalVertex::ms_decl;
   bgfx::VertexDecl PosUVBonesVertex::ms_decl;
   bgfx::VertexDecl PosUVNormalBonesVertex::ms_decl;
   bgfx::VertexDecl PosUVTBNBonesVertex::ms_decl;
   bgfx::VertexDecl PosColorVertex::ms_decl;
   bgfx::VertexDecl PosUVColorVertex::ms_decl;
   bgfx::VertexBufferHandle cubeVB;
   bgfx::IndexBufferHandle  cubeIB;
   bgfx::VertexBufferHandle planeVB;
   bgfx::IndexBufferHandle  planeIB;

   // Common Shape: Cube
   static Graphics::PosUVColorVertex s_cubeVertices[24] =
   {
      // Bottom
	   {-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0xffffffff },
	   {-1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0xffffffff },
	   { 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0xffffffff },
	   { 1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 0xffffffff },

      // Top
	   {-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0xffffffff },
	   {-1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 0xffffffff },
	   { 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0xffffffff },
	   { 1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0xffffffff },

      // Left
	   { 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0xffffffff },
	   { 1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 0xffffffff },
	   { 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0xffffffff },
	   { 1.0f, 1.0f,  1.0f, 1.0f, 0.0f, 0xffffffff },

      // Right
	   {-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0xffffffff },
	   {-1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 0xffffffff },
	   {-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0xffffffff },
	   {-1.0f, 1.0f,  1.0f, 0.0f, 0.0f, 0xffffffff },

      // Back
      {-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0xffffffff },
	   {-1.0f, 1.0f,  -1.0f, 0.0f, 0.0f, 0xffffffff },
	   { 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0xffffffff },
	   { 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 0xffffffff },

      // Front
	   {-1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0xffffffff },
	   {-1.0f, 1.0f,  1.0f, 1.0f, 0.0f, 0xffffffff },
	   { 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0xffffffff },
	   { 1.0f, 1.0f,  1.0f, 0.0f, 0.0f, 0xffffffff },
   };

   static const uint16_t s_cubeIndices[36] =
   {
	   0, 1, 2, // 0
	   1, 3, 2,

	   4, 6, 5, // 2
	   5, 6, 7,

	   8, 9, 10, // 0
	   9, 11, 10,

	   12, 14, 13, // 2
	   13, 14, 15,

	   16, 18, 17, // 2
	   17, 18, 19,

      20, 21, 22, // 0
	   21, 23, 22,
   };

      // Common Shape: Cube
   static Graphics::PosUVColorVertex s_planeVertices[4] =
   {
      // Top
	   {-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0xffffffff },
	   {-1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 0xffffffff },
	   { 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0xffffffff },
	   { 1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0xffffffff },
   };

   static const uint16_t s_planeIndices[6] =
   {
	   0, 2, 1,
	   1, 2, 3,
   };

   void initUtilities()
   {
      // Vertex Layouts
      PosUVVertex::init();
      PosUVNormalVertex::init();
      PosUVBonesVertex::init();
      PosUVNormalBonesVertex::init();
      PosUVTBNBonesVertex::init();
      PosColorVertex::init();
      PosUVColorVertex::init();

      // Common Shapes
      const bgfx::Memory* mem;

      // Create static vertex buffer.
      cubeVB.idx = bgfx::invalidHandle;
	   mem = bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices) );
	   cubeVB = bgfx::createVertexBuffer(mem, Graphics::PosUVColorVertex::ms_decl);

	   // Create static index buffer.
      cubeIB.idx = bgfx::invalidHandle;
	   mem = bgfx::makeRef(s_cubeIndices, sizeof(s_cubeIndices) );
	   cubeIB = bgfx::createIndexBuffer(mem);

      // Create static vertex buffer.
      planeVB.idx = bgfx::invalidHandle;
	   mem = bgfx::makeRef(s_planeVertices, sizeof(s_planeVertices) );
	   planeVB = bgfx::createVertexBuffer(mem, Graphics::PosUVColorVertex::ms_decl);

	   // Create static index buffer.
      planeIB.idx = bgfx::invalidHandle;
	   mem = bgfx::makeRef(s_planeIndices, sizeof(s_planeIndices) );
	   planeIB = bgfx::createIndexBuffer(mem);
   }

   void destroyUtilities()
   {
      if ( cubeVB.idx != bgfx::invalidHandle )
         bgfx::destroyVertexBuffer(cubeVB);
      if ( cubeIB.idx != bgfx::invalidHandle )
         bgfx::destroyIndexBuffer(cubeIB);

      if ( planeVB.idx != bgfx::invalidHandle )
         bgfx::destroyVertexBuffer(planeVB);
      if ( planeIB.idx != bgfx::invalidHandle )
         bgfx::destroyIndexBuffer(planeIB);
   }
}