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