#ifndef _TERRAINBUILDER_H_
#define _TERRAINBUILDER_H_

#ifndef _PLUGINS_SHARED_H
#include <plugins/plugins_shared.h>
#endif

#ifndef _SIM_OBJECT_H_
#include <sim/simObject.h>
#endif

struct PosUVColorVertex
{
   F32 m_x;
	F32 m_y;
	F32 m_z;
	F32 m_u;
	F32 m_v;
	U32 m_abgr;
};

class TerrainBuilder
{
protected:
   Plugins::PluginLink Link;
   Vector<PosUVColorVertex> mVerts;
   Vector<uint16_t> mIndices;

public:
   F32*  heightMap;
   U32   width;
   U32   height;

   TerrainBuilder(Plugins::PluginLink _link);
   ~TerrainBuilder();

   void rebuild();
   bgfx::VertexBufferHandle   getVertexBuffer();
   bgfx::IndexBufferHandle    getIndexBuffer();
};

#endif