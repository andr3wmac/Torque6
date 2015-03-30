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
#include <graphics/utilities.h>

#include <bx/fpumath.h>

TerrainCell::TerrainCell(S32 _gridX, S32 _gridY)
{
   heightMap = NULL;

   gridX = _gridX;
   gridY = _gridY;

   mVB.idx = bgfx::invalidHandle;
   mIB.idx = bgfx::invalidHandle;

   mRenderData = NULL;
   mTextures[0].idx = bgfx::invalidHandle;
   mTextures[1].idx = bgfx::invalidHandle;
   mTextures[2].idx = bgfx::invalidHandle;

   // Load Shader
   Graphics::ShaderAsset* terrainShaderAsset = Plugins::Link.Graphics.getShaderAsset("Terrain:terrainShader");
   if ( terrainShaderAsset )
      mShader = terrainShaderAsset->getProgram();
}

TerrainCell::~TerrainCell()
{
   SAFE_DELETE(heightMap);

   if ( mVB.idx != bgfx::invalidHandle )
      Plugins::Link.bgfx.destroyVertexBuffer(mVB);

   if ( mIB.idx != bgfx::invalidHandle )
      Plugins::Link.bgfx.destroyIndexBuffer(mIB);
}

bgfx::VertexBufferHandle TerrainCell::getVertexBuffer()
{
   const bgfx::Memory* mem;
   mem = Plugins::Link.bgfx.makeRef(&mVerts[0], sizeof(PosUVColorVertex) * mVerts.size() );
   bgfx::VertexBufferHandle terrainVB = Plugins::Link.bgfx.createVertexBuffer(mem, *Plugins::Link.Graphics.PosUVColorVertex, BGFX_BUFFER_NONE);
   return terrainVB;
}

bgfx::IndexBufferHandle TerrainCell::getIndexBuffer()
{
   const bgfx::Memory* mem;
	mem = Plugins::Link.bgfx.makeRef(&mIndices[0], sizeof(uint16_t) * mIndices.size() );
	bgfx::IndexBufferHandle terrainIB = Plugins::Link.bgfx.createIndexBuffer(mem, BGFX_BUFFER_NONE);
   return terrainIB;
}

void TerrainCell::loadHeightMap(const char* path)
{
   GBitmap *bmp = dynamic_cast<GBitmap*>(Plugins::Link.ResourceManager->loadInstance(path));  
   if(bmp != NULL)
   {
      height = (bmp->getHeight() / 2) * 2;
      width = (bmp->getWidth() / 2) * 2;
      heightMap = new F32[height * width];

      for(U32 y = 0; y < height; y++)
      {
         for(U32 x = 0; x < width; x++)
         {
            ColorI heightSample;
            bmp->getColor(x, y, heightSample);
            heightMap[(y * width) + x] = ((F32)heightSample.red) * 0.1f;
         }
      }

      rebuild();
   }
}

void TerrainCell::loadTexture(U32 layer, const char* path)
{
   TextureObject* texture_obj = Plugins::Link.Graphics.loadTexture(path, TextureHandle::TextureHandleType::BitmapKeepTexture, false, false, false);
   if ( texture_obj )
   {
      mTextures[layer] = texture_obj->getBGFXTexture();
      refresh();
   }
}

void TerrainCell::rebuild()
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

   refresh();
}

void TerrainCell::refresh()
{
   if ( mRenderData == NULL )
      mRenderData = Plugins::Link.Rendering.createRenderData();

   // Destroy Old Buffers
   if ( mVB.idx != bgfx::invalidHandle )
      Plugins::Link.bgfx.destroyVertexBuffer(mVB);

   if ( mIB.idx != bgfx::invalidHandle )
      Plugins::Link.bgfx.destroyIndexBuffer(mIB);

   // Get New Ones
   mVB = getVertexBuffer();
	mIB = getIndexBuffer();
   
   mRenderData->indexBuffer = mIB;
   mRenderData->vertexBuffer = mVB;

   mTextureData.clear();
   mRenderData->textures = &mTextureData;

   for ( U32 n = 0; n < 3; ++n )
   {
      if ( mTextures[n].idx != bgfx::invalidHandle )
      {
         Rendering::TextureData* layer = mRenderData->addTexture();
         layer->uniform = Plugins::Link.Graphics.getTextureUniform(n);
         layer->handle = mTextures[n];
      }
   }

   // Render in Forward (for now) with our custom terrain shader.
   mRenderData->shader = mShader;
   mRenderData->view = Graphics::ViewTable::Forward;

   // Transform
   F32* cubeMtx = new F32[16];
   bx::mtxSRT(cubeMtx, 1, 1, 1, 0, 0, 0, gridX * width - (1 * gridX), 0, gridY * height - (2 * gridY));
   mRenderData->transformTable = cubeMtx;
   mRenderData->transformCount = 1;
}