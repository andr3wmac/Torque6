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
#include <bgfx/bgfx.h>

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
   bgfx::TextureHandle      noiseTexture;

   // Common Shape: Cube
   static Graphics::PosUVColorVertex s_cubeVertices[36] =
   {
      {  1.0f,  1.0f,  1.0f, 1.0f, -1.0f, 0xffffffff },
      {  1.0f, -1.0f, -1.0f, 0.0f,  0.0f, 0xffffffff },
      {  1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0xffffffff },
      {  1.0f,  1.0f, -1.0f, 1.0f,  0.0f, 0xffffffff },
      {  1.0f, -1.0f, -1.0f, 0.0f,  0.0f, 0xffffffff },
      {  1.0f,  1.0f,  1.0f, 1.0f, -1.0f, 0xffffffff },
      { -1.0f,  1.0f,  1.0f, 0.0f, -1.0f, 0xffffffff },
      { -1.0f, -1.0f, -1.0f, 1.0f,  0.0f, 0xffffffff },
      { -1.0f,  1.0f, -1.0f, 0.0f,  0.0f, 0xffffffff },
      { -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, 0xffffffff },
      { -1.0f, -1.0f, -1.0f, 1.0f,  0.0f, 0xffffffff },
      { -1.0f,  1.0f,  1.0f, 0.0f, -1.0f, 0xffffffff },
      {  1.0f,  1.0f,  1.0f, 0.0f, -1.0f, 0xffffffff },
      { -1.0f,  1.0f, -1.0f, 1.0f,  0.0f, 0xffffffff },
      {  1.0f,  1.0f, -1.0f, 0.0f,  0.0f, 0xffffffff },
      { -1.0f,  1.0f,  1.0f, 1.0f, -1.0f, 0xffffffff },
      { -1.0f,  1.0f, -1.0f, 1.0f,  0.0f, 0xffffffff },
      {  1.0f,  1.0f,  1.0f, 0.0f, -1.0f, 0xffffffff },
      {  1.0f, -1.0f,  1.0f, 1.0f, -1.0f, 0xffffffff },
      { -1.0f, -1.0f, -1.0f, 0.0f,  0.0f, 0xffffffff },
      { -1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0xffffffff },
      {  1.0f, -1.0f, -1.0f, 1.0f,  0.0f, 0xffffffff },
      { -1.0f, -1.0f, -1.0f, 0.0f,  0.0f, 0xffffffff },
      {  1.0f, -1.0f,  1.0f, 1.0f, -1.0f, 0xffffffff },
      {  1.0f,  1.0f,  1.0f, 1.0f, -1.0f, 0xffffffff },
      { -1.0f, -1.0f,  1.0f, 0.0f,  0.0f, 0xffffffff },
      { -1.0f,  1.0f,  1.0f, 0.0f, -1.0f, 0xffffffff },
      {  1.0f, -1.0f,  1.0f, 1.0f,  0.0f, 0xffffffff },
      { -1.0f, -1.0f,  1.0f, 0.0f,  0.0f, 0xffffffff },
      {  1.0f,  1.0f,  1.0f, 1.0f, -1.0f, 0xffffffff },
      {  1.0f,  1.0f, -1.0f, 0.0f, -1.0f, 0xffffffff },
      { -1.0f, -1.0f, -1.0f, 1.0f,  0.0f, 0xffffffff },
      {  1.0f, -1.0f, -1.0f, 0.0f,  0.0f, 0xffffffff },
      { -1.0f,  1.0f, -1.0f, 1.0f, -1.0f, 0xffffffff },
      { -1.0f, -1.0f, -1.0f, 1.0f,  0.0f, 0xffffffff },
      {  1.0f,  1.0f, -1.0f, 0.0f, -1.0f, 0xffffffff }
   };

   static const uint16_t s_cubeIndices[36] =
   {
       0,  1,  2,  3,  4,  5, 
       6,  7,  8,  9, 10, 11, 
      12, 13, 14, 15, 16, 17, 
      18, 19, 20, 21, 22, 23, 
      24, 25, 26, 27, 28, 29, 
      30, 31, 32, 33, 34, 35,
   };

   // Common Shape: Plane
   static Graphics::PosUVColorVertex s_planeVertices[4] =
   {
      // Top
	   {-1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0xffffffff },
	   {-1.0f, 0.0f,  1.0f, 0.0f, 0.0f, 0xffffffff },
	   { 1.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0xffffffff },
	   { 1.0f, 0.0f,  1.0f, 1.0f, 0.0f, 0xffffffff },
   };

   static const uint16_t s_planeIndices[6] =
   {
	   0, 2, 1,
	   1, 2, 3,
   };

   static const uint8_t s_noiseTexture[64] = {
      150, 123, 254, 255,
      127, 3,   97,  255,
      164, 246, 99,  255,
      155, 177, 14,  255,

      54, 83,  221, 255,
      2,  142, 143, 255,
      32, 57,  79,  255,
      49, 160, 32,  255,

      57,  232, 115, 255,
      178, 216, 203, 255,
      70,  196, 218, 255,
      241, 164, 82,  255,

      225, 56, 85,  255,
      233, 88, 189, 255,
      144, 25, 203, 255,
      117, 73, 12,  255
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

      // Noise texture, 4x4 RGBA
      mem = bgfx::makeRef(s_noiseTexture, 64);
      noiseTexture = bgfx::createTexture2D(4, 4, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MAG_POINT | BGFX_TEXTURE_MIP_POINT, mem);
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