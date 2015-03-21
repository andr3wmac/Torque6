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

#include "TerrainBuilder.h"
#include <plugins/plugins_shared.h>

#include <sim/simObject.h>
#include <3d/rendering/common.h>
#include <graphics/utilities.h>

#include <bx/fpumath.h>

TerrainBuilder::TerrainBuilder(Plugins::PluginLink _link)
{
   Link = _link;
}

TerrainBuilder::~TerrainBuilder()
{
   SAFE_DELETE(heightMap);
}

bgfx::VertexBufferHandle TerrainBuilder::getVertexBuffer()
{
   const bgfx::Memory* mem;
   mem = Link.bgfx.makeRef(&mVerts[0], sizeof(PosUVColorVertex) * mVerts.size() );
   bgfx::VertexBufferHandle terrainVB = Link.bgfx.createVertexBuffer(mem, *Link.Graphics.PosUVColorVertex, BGFX_BUFFER_NONE);
   return terrainVB;
}

bgfx::IndexBufferHandle TerrainBuilder::getIndexBuffer()
{
   const bgfx::Memory* mem;
	mem = Link.bgfx.makeRef(&mIndices[0], sizeof(uint16_t) * mIndices.size() );
	bgfx::IndexBufferHandle terrainIB = Link.bgfx.createIndexBuffer(mem, BGFX_BUFFER_NONE);
   return terrainIB;
}

void TerrainBuilder::rebuild()
{
   mVerts.clear();
   mIndices.clear();

   for(U32 y = 0; y < height; y++ )
   {
      for(U32 x = 0; x < width; x++ )
      {
         PosUVColorVertex vert = {x, heightMap[(y * width) + x], y, (F32)x / (F32)width, (F32)y / (F32)height, 0xffffffff };
         mVerts.push_back(vert);
      }
   }

   for(U32 y = 0; y < (height - 1); y++ )
   {
      U32 y_offset = (y * width);
      for(U32 x = 0; x < (width - 1); x++ )
      {
         mIndices.push_back(y_offset + x);
         mIndices.push_back(y_offset + x + 1);
         mIndices.push_back(y_offset + x + width);
         mIndices.push_back(y_offset + x + 1);
         mIndices.push_back(y_offset + x + width + 1);
         mIndices.push_back(y_offset + x + width);
      }
   }
}