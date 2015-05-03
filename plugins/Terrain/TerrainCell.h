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

#ifndef _TERRAINBUILDER_H_
#define _TERRAINBUILDER_H_

#ifndef _PLUGINS_SHARED_H
#include <plugins/plugins_shared.h>
#endif

#ifndef _SIM_OBJECT_H_
#include <sim/simObject.h>
#endif

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
};

class TerrainCell
{
protected:
   PosUVNormalVertex*               mVerts;
   U32                              mVertCount;
   U32*                             mIndices;
   U32                              mIndexCount;
   F32                              mTransformMtx[16];

   bgfx::TextureHandle*             mMegaTexture;
   Vector<Rendering::TextureData>   mTextureData;
   Vector<Rendering::UniformData>*  mUniformData;
   bgfx::ProgramHandle              mShader;
   Rendering::RenderData*           mRenderData;
   bgfx::DynamicVertexBufferHandle  mDynamicVB;
   bgfx::DynamicIndexBufferHandle   mDynamicIB;
   bgfx::VertexBufferHandle         mVB;
   bgfx::IndexBufferHandle          mIB;


public:

   bgfx::TextureHandle              mBlendTexture;
   S32      gridX;
   S32      gridY;
   F32*     heightMap;
   ColorI*  blendMap;
   U32      width;
   U32      height;
   F32      maxTerrainHeight;
   bool     dirty;

   TerrainCell(bgfx::TextureHandle* _megaTexture, Vector<Rendering::UniformData>* _uniformData, S32 _gridX, S32 _gridY);
   ~TerrainCell();

   Point3F getWorldSpacePos(U32 x, U32 y);
   void loadHeightMap(const char* path);
   void loadEmptyTerrain(S32 _width, S32 _height);
   void refresh();
   void rebuild();
   Point3F getNormal(U32 x, U32 y);
   void refreshVertexBuffer();
   void refreshIndexBuffer();
   void refreshBlendMap();

   void paintLayer(U32 layerNum, U32 x, U32 y, U8 strength);
};

extern Vector<TerrainCell> terrainGrid;
void stitchEdges(SimObject *obj, S32 argc, const char *argv[]);

#endif