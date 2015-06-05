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

#include "TerrainCell.h"
#include <plugins/plugins_shared.h>

#include <sim/simObject.h>
#include <3d/rendering/common.h>
#include <graphics/core.h>

#include <bx/fpumath.h>

using namespace Plugins;

Vector<TerrainCell> terrainGrid;

TerrainCell::TerrainCell(bgfx::TextureHandle* _megaTexture, Vector<Rendering::UniformData>* _uniformData, S32 _gridX, S32 _gridY)
{
   mVerts = NULL;
   mVertCount = 0;
   mIndices = NULL;
   mIndexCount = 0;

   dirty = false;
   heightMap = NULL;
   blendMap = NULL;

   mMegaTexture = _megaTexture;
   mUniformData = _uniformData;
   gridX = _gridX;
   gridY = _gridY;

   mDynamicVB.idx = bgfx::invalidHandle;
   mDynamicIB.idx = bgfx::invalidHandle;
   mVB.idx = bgfx::invalidHandle;
   mIB.idx = bgfx::invalidHandle;
   mBlendTexture.idx = bgfx::invalidHandle;

   mRenderData = NULL;

   maxTerrainHeight = 0;

   // Load Shader
   Graphics::ShaderAsset* terrainShaderAsset = Link.Graphics.getShaderAsset("Terrain:terrainShader");
   if ( terrainShaderAsset )
      mShader = terrainShaderAsset->getProgram();
}

TerrainCell::~TerrainCell()
{
   SAFE_DELETE(mVerts);
   SAFE_DELETE(mIndices);
   SAFE_DELETE(heightMap);
   SAFE_DELETE(blendMap);

   if ( mVB.idx != bgfx::invalidHandle )
      Link.bgfx.destroyVertexBuffer(mVB);

   if ( mIB.idx != bgfx::invalidHandle )
      Link.bgfx.destroyIndexBuffer(mIB);

   if ( mDynamicVB.idx != bgfx::invalidHandle )
      Link.bgfx.destroyDynamicVertexBuffer(mDynamicVB);

   if ( mDynamicIB.idx != bgfx::invalidHandle )
      Link.bgfx.destroyDynamicIndexBuffer(mDynamicIB);

   if ( mBlendTexture.idx != bgfx::invalidHandle )
      Link.bgfx.destroyTexture(mBlendTexture);
}

void TerrainCell::refreshVertexBuffer()
{
   if ( mVertCount <= 0 ) return;

   if ( mVB.idx != bgfx::invalidHandle )
      Link.bgfx.destroyVertexBuffer(mVB);

   const bgfx::Memory* mem;
   mem = Link.bgfx.makeRef(&mVerts[0], sizeof(PosUVNormalVertex) * mVertCount, NULL, NULL );
   mVB = Link.bgfx.createVertexBuffer(mem, *Link.Graphics.PosUVNormalVertex, BGFX_BUFFER_NONE);

   //const bgfx::Memory* mem;
   //mem = Link.bgfx.makeRef(&mVerts[0], sizeof(PosUVColorVertex) * mVerts.size(), NULL, NULL );

   //if ( mDynamicVB.idx == bgfx::invalidHandle )
   //   mDynamicVB = Link.bgfx.createDynamicVertexBuffer(mem, *Link.Graphics.PosUVColorVertex, BGFX_BUFFER_NONE);
   //else
   //   Link.bgfx.updateDynamicVertexBuffer(mDynamicVB, mem);
}

void TerrainCell::refreshIndexBuffer()
{
   if ( mIndexCount <= 0 ) return;

   if ( mIB.idx != bgfx::invalidHandle )
      Link.bgfx.destroyIndexBuffer(mIB);

   const bgfx::Memory* mem;
	mem = Link.bgfx.makeRef(&mIndices[0], sizeof(U32) * mIndexCount, NULL, NULL );
   mIB = Link.bgfx.createIndexBuffer(mem, BGFX_BUFFER_INDEX32);

   //const bgfx::Memory* mem;
	//mem = Link.bgfx.makeRef(&mIndices[0], sizeof(uint16_t) * mIndices.size(), NULL, NULL );

   //if ( mDynamicIB.idx == bgfx::invalidHandle )
   //   mDynamicIB = Link.bgfx.createDynamicIndexBuffer(mem, BGFX_BUFFER_NONE);
   //else
   //   Link.bgfx.updateDynamicIndexBuffer(mDynamicIB, mem);
}

void TerrainCell::refreshBlendMap()
{
   const bgfx::Memory* mem;
   //mem = Link.bgfx.makeRef(&mVerts[0], sizeof(PosUVColorVertex) * mVertCount, NULL, NULL );

	mem = Link.bgfx.alloc(width * height * 4);
   dMemcpy(mem->data, &blendMap[0].red, width * height * 4);

   if ( mBlendTexture.idx == bgfx::invalidHandle )
      mBlendTexture = Link.bgfx.createTexture2D(width, height, 0, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP, mem);
   else
      Link.bgfx.updateTexture2D(mBlendTexture, 0, 0, 0, width, height, mem, width * 4);
}

void TerrainCell::loadEmptyTerrain(S32 _width, S32 _height)
{
   height = _height;
   width = _width;
   heightMap = new F32[height * width];
   blendMap = new ColorI[height * width];
   dMemset(heightMap, 0, height * width * sizeof(F32));
   maxTerrainHeight = 0;
}

void TerrainCell::loadHeightMap(const char* path)
{
   GBitmap *bmp = dynamic_cast<GBitmap*>(Link.ResourceManager->loadInstance(path));  
   if(bmp != NULL)
   {
      height = (bmp->getHeight() / 2) * 2;
      width = (bmp->getWidth() / 2) * 2;
      heightMap = new F32[height * width];
      blendMap = new ColorI[height * width];

      for(U32 y = 0; y < height; y++)
      {
         for(U32 x = 0; x < width; x++)
         {
            ColorI heightSample;
            bmp->getColor(x, y, heightSample);
            heightMap[(y * width) + x] = ((F32)heightSample.red) * 0.25f;

            // Temp blendmap for testing
            if ( heightMap[(y * width) + x] > 35 )
               blendMap[(y * width) + x].set(55, 200, 0, 0);
            else
               blendMap[(y * width) + x].set(255, 0, 0, 0);

            if ( heightMap[(y * width) + x] > maxTerrainHeight )
               maxTerrainHeight = heightMap[(y * width) + x];
         }
      }

      rebuild();
   }
}

Point3F TerrainCell::getWorldSpacePos(U32 x, U32 y)
{
   U32 pos = (y * width) + x;
   if ( pos < 0 ) return Point3F::Zero;
   if ( pos >= (width * height) ) return Point3F::Zero;

   F32 heightValue = heightMap[pos];
   Point3F worldPos;
   worldPos.set(gridX * width - (1 * gridX) + x, heightValue, gridY * height - (2 * gridY) + y);
   return worldPos;
}

Point3F TerrainCell::getNormal(U32 x, U32 y)
{
   // Thanks to:
   // http://stackoverflow.com/questions/13983189/opengl-how-to-calculate-normals-in-a-terrain-height-grid

   U32 offset = ( (S32)x - 1 < 0 ) ? 0 : 1;
   F32 hL = heightMap[(y * width) + (x - offset)];

   offset = ( (S32)x + 1 >= width ) ? 0 : 1;
   F32 hR = heightMap[(y * width) + (x + offset)];

   offset = ( (S32)y - 1 < 0 ) ? 0 : 1;
   F32 hD = heightMap[((y - offset) * width) + x];

   offset = ( (S32)y + 1 >= height ) ? 0 : 1;
   F32 hU = heightMap[((y + offset) * width) + x];

   // deduce terrain normal
   Point3F normal(hL - hR, hD - hU, 2.0);
   normal.normalize();
   return normal;
}

void TerrainCell::rebuild()
{
   dirty = true;

   SAFE_DELETE(mVerts);
   SAFE_DELETE(mIndices);

   mVertCount = 0;
   mVerts = new PosUVNormalVertex[width * height];
   for(U32 y = 0; y < height; y++ )
   {
      for(U32 x = 0; x < width; x++ )
      {
         mVerts[mVertCount].m_x = x;
         mVerts[mVertCount].m_y = heightMap[(y * width) + x];
         mVerts[mVertCount].m_z = y;
         mVerts[mVertCount].m_u = (F32)x / (F32)width;
         mVerts[mVertCount].m_v = (F32)y / (F32)height;

         Point3F normal = getNormal(x, y);
         mVerts[mVertCount].m_normal_x = normal.x;
         mVerts[mVertCount].m_normal_y = normal.y;
         mVerts[mVertCount].m_normal_z = normal.z;
         mVertCount++;
      }
   }

   mIndexCount = 0;
   mIndices = new U32[width * height * 6];
   for(U32 y = 0; y < (height - 1); y++ )
   {
      U32 y_offset = (y * width);
      for(U32 x = 0; x < (width - 1); x++ )
      {
         mIndices[mIndexCount] = y_offset + x;
         mIndices[mIndexCount + 1] = y_offset + x + 1;
         mIndices[mIndexCount + 2] = y_offset + x + width;
         mIndices[mIndexCount + 3] = y_offset + x + 1;
         mIndices[mIndexCount + 4] = y_offset + x + width + 1;
         mIndices[mIndexCount + 5] = y_offset + x + width;
         mIndexCount += 6;
      }
   }

   refreshVertexBuffer();
   refreshIndexBuffer();
   refreshBlendMap();
   refresh();
}

void TerrainCell::refresh()
{
   if ( mRenderData == NULL )
      mRenderData = Link.Rendering.createRenderData();

   //mRenderData->isDynamic = true;
   //mRenderData->dynamicIndexBuffer = mDynamicIB;
   //mRenderData->dynamicVertexBuffer = mDynamicVB;
   mRenderData->indexBuffer = mIB;
   mRenderData->vertexBuffer = mVB;

   mTextureData.clear();
   mRenderData->textures = &mTextureData;

   Rendering::TextureData* layer = mRenderData->addTexture();
   layer->uniform = Link.Graphics.getTextureUniform(0);
   layer->handle = *mMegaTexture;

   // Render in Deferred
   mRenderData->shader = mShader;
   mRenderData->view = Link.Graphics.getView("DeferredGeometry", "", false);
   mRenderData->uniforms.uniforms = mUniformData;

   // Transform
   bx::mtxSRT(mTransformMtx, 1, 1, 1, 0, 0, 0, gridX * width - (1 * gridX), 0, gridY * height - (1 * gridY));
   mRenderData->transformTable = mTransformMtx;
   mRenderData->transformCount = 1;
}

void TerrainCell::paintLayer(U32 layerNum, U32 x, U32 y, U8 strength)
{
   U32 mapPos = (y  * width) + x;

   ColorI sub(blendMap[mapPos].red < strength ? blendMap[mapPos].red : strength, 
              blendMap[mapPos].green < strength ? blendMap[mapPos].green : strength, 
              blendMap[mapPos].blue < strength ? blendMap[mapPos].blue : strength, 
              blendMap[mapPos].alpha < strength ? blendMap[mapPos].alpha : strength);

   ColorI add(layerNum == 0 ? strength : 0,
              layerNum == 1 ? strength : 0,
              layerNum == 2 ? strength : 0,
              layerNum == 3 ? strength : 0);
}

void stitchEdges(SimObject *obj, S32 argc, const char *argv[])
{
   for(U32 i = 0; i < terrainGrid.size(); ++i)
   {
      TerrainCell* curCell = &terrainGrid[i];

      for(U32 n = 0; n < terrainGrid.size(); ++n)
      {
         TerrainCell* compareCell = &terrainGrid[n];
         
         // Left
         if ( compareCell->gridX == (curCell->gridX - 1) && compareCell->gridY == curCell->gridY )
         {
            for(U32 y = 0; y < curCell->height; ++y)
            {
               U32 left_index = ((y + 1) * compareCell->width) - 1;
               U32 right_index = y * curCell->width;
               F32 average_height = (compareCell->heightMap[left_index] + curCell->heightMap[right_index]) / 2.0f;

               compareCell->heightMap[left_index] = average_height;
               curCell->heightMap[right_index] = average_height;
            }

            compareCell->rebuild();
            curCell->rebuild();
         }

         // Bottom
         if ( compareCell->gridY == (curCell->gridY - 1) && compareCell->gridX == curCell->gridX )
         {
            for(U32 x = 0; x < curCell->width; ++x)
            {
               U32 bottom_index = (curCell->width * (curCell->height - 2)) + x;
               U32 top_index = x;
               F32 average_height = (compareCell->heightMap[bottom_index] + curCell->heightMap[top_index]) / 2.0f;

               compareCell->heightMap[bottom_index] = average_height;
               curCell->heightMap[top_index] = average_height;
            }

            compareCell->rebuild();
            curCell->rebuild();
         }
      }
   }
}
