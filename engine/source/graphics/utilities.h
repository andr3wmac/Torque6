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

#ifndef _GRAPHICS_UTILITIES_H_
#define _GRAPHICS_UTILITIES_H_

#ifndef _FILEOBJECT_H_
#include "io/fileObject.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx.h>
#endif

namespace Graphics
{
   // TODO: Organize this stuff better rather than throwing everything in here.

   extern bgfx::VertexBufferHandle cubeVB;
   extern bgfx::IndexBufferHandle  cubeIB;
   extern bgfx::VertexBufferHandle planeVB;
   extern bgfx::IndexBufferHandle  planeIB;

   void initUtilities();
   void destroyUtilities();

   // Vertex Layout
   struct PosUVVertex
   {
	   F32 m_x;
	   F32 m_y;
	   F32 m_z;
	   F32 m_u;
	   F32 m_v;

	   static void init()
	   {
		   ms_decl
			   .begin()
			   .add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
			   .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			   .end();
	   }

	   static bgfx::VertexDecl ms_decl;
   };

   struct PosUVNormalVertex
   {
	   F32 m_x;
	   F32 m_y;
	   F32 m_z;
	   F32 m_u;
	   F32 m_v;
      F32 m_normal_x;
      F32 m_normal_y;
      F32 m_normal_z;

	   static void init()
	   {
		   ms_decl
			   .begin()
			   .add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
			   .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal,    3, bgfx::AttribType::Float)
			   .end();
	   }

	   static bgfx::VertexDecl ms_decl;
   };

   struct PosUVBonesVertex
   {
	   F32 m_x;
	   F32 m_y;
	   F32 m_z;
	   F32 m_u;
	   F32 m_v;
      U8 m_boneindex[4];
      F32 m_boneweight[4];

	   static void init()
	   {
		   ms_decl
			   .begin()
			   .add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
			   .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Indices,   4, bgfx::AttribType::Uint8, false, true)
            .add(bgfx::Attrib::Weight,    4, bgfx::AttribType::Float)
			   .end();
	   }

	   static bgfx::VertexDecl ms_decl;
   };

   struct PosUVNormalBonesVertex
   {
	   F32 m_x;
	   F32 m_y;
	   F32 m_z;
	   F32 m_u;
	   F32 m_v;
      F32 m_normal_x;
      F32 m_normal_y;
      F32 m_normal_z;
      U8 m_boneindex[4];
      F32 m_boneweight[4];

	   static void init()
	   {
		   ms_decl
			   .begin()
			   .add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
			   .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal,    3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Indices,   4, bgfx::AttribType::Uint8, false, true)
            .add(bgfx::Attrib::Weight,    4, bgfx::AttribType::Float)
			   .end();
	   }

	   static bgfx::VertexDecl ms_decl;
   };

   struct PosUVTBNBonesVertex
   {
	   F32 m_x;
	   F32 m_y;
	   F32 m_z;
	   F32 m_u;
	   F32 m_v;
      F32 m_tangent_x;
      F32 m_tangent_y;
      F32 m_tangent_z;
      F32 m_bitangent_x;
      F32 m_bitangent_y;
      F32 m_bitangent_z;
      F32 m_normal_x;
      F32 m_normal_y;
      F32 m_normal_z;
      U8 m_boneindex[4];
      F32 m_boneweight[4];

	   static void init()
	   {
		   ms_decl
			   .begin()
			   .add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
			   .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Tangent,   3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Bitangent, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal,    3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Indices,   4, bgfx::AttribType::Uint8, false, true)
            .add(bgfx::Attrib::Weight,    4, bgfx::AttribType::Float)
			   .end();
	   }

	   static bgfx::VertexDecl ms_decl;
   };

   struct PosColorVertex
   {
	   F32 m_x;
	   F32 m_y;
	   F32 m_z;
	   U32 m_abgr;

	   static void init()
	   {
		   ms_decl
			   .begin()
			   .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			   .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
			   .end();
	   };

	   static bgfx::VertexDecl ms_decl;
   };

   struct PosUVColorVertex
   {
	   F32 m_x;
	   F32 m_y;
	   F32 m_z;
	   F32 m_u;
	   F32 m_v;
	   U32 m_abgr;

	   static void init()
	   {
		   ms_decl
			   .begin()
			   .add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			   .add(bgfx::Attrib::Color0,    4, bgfx::AttribType::Uint8, true)
			   .end();
	   };

	   static bgfx::VertexDecl ms_decl;
   };

   #define BGFXCOLOR_RGBA(r,g,b,a) \
	      ((U32)((((r)&0xff)<<24)|(((g)&0xff)<<16)|(((b)&0xff)<<8)|((a)&0xff)))

}
#endif //_GRAPHICS_UTILITIES_H_
