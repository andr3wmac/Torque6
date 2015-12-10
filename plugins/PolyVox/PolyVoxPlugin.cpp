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

#include "PolyVoxPlugin.h"
#include <plugins/plugins_shared.h>

#include <sim/simObject.h>
#include <3d/rendering/common.h>
#include <graphics/core.h>

#include "PolyVoxCore/CubicSurfaceExtractorWithNormals.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/SimpleVolume.h"

#include <bx/fpumath.h>

using namespace Plugins;

// Test PolyVox
PolyVox::SimpleVolume<U8> volData(PolyVox::Region(PolyVox::Vector3DInt32(0, 0, 0), PolyVox::Vector3DInt32(63, 63, 63)));

PosUVNormalVertex*         mVerts = NULL;
U32                        mVertCount = 0;
U32*                       mIndices = NULL;
U32                        mIndexCount = 0;
F32                        mTransformMtx[16];
bgfx::VertexBufferHandle   mVB = BGFX_INVALID_HANDLE;
bgfx::IndexBufferHandle    mIB = BGFX_INVALID_HANDLE;
Rendering::RenderData*     mRenderData = NULL;
bgfx::ProgramHandle        mShader = BGFX_INVALID_HANDLE;

void createSphereInVolume(PolyVox::SimpleVolume<U8>& volData, float fRadius)
{
   //This vector hold the position of the center of the volume
   PolyVox::Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);

   //This three-level for loop iterates over every voxel in the volume
   for (int z = 0; z < volData.getDepth(); z++)
   {
      for (int y = 0; y < volData.getHeight(); y++)
      {
         for (int x = 0; x < volData.getWidth(); x++)
         {
            //Store our current position as a vector...
            PolyVox::Vector3DFloat v3dCurrentPos(x, y, z);
            //And compute how far the current position is from the center of the volume
            float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

            uint8_t uVoxelValue = 0;

            //If the current voxel is less than 'radius' units from the center then we make it solid.
            if (fDistToCenter <= fRadius)
            {
               //Our new voxel value
               uVoxelValue = 255;
            }

            //Wrte the voxel value into the volume
            volData.setVoxelAt(x, y, z, uVoxelValue);
         }
      }
   }
}

void refreshVertexBuffer()
{
   if (mVertCount <= 0) return;

   if (mVB.idx != bgfx::invalidHandle)
      Link.bgfx.destroyVertexBuffer(mVB);

   const bgfx::Memory* mem;
   mem = Link.bgfx.makeRef(&mVerts[0], sizeof(PosUVNormalVertex) * mVertCount, NULL, NULL);
   mVB = Link.bgfx.createVertexBuffer(mem, *Link.Graphics.PosUVNormalVertex, BGFX_BUFFER_NONE);
}

void refreshIndexBuffer()
{
   if (mIndexCount <= 0) return;

   if (mIB.idx != bgfx::invalidHandle)
      Link.bgfx.destroyIndexBuffer(mIB);

   const bgfx::Memory* mem;
   mem = Link.bgfx.makeRef(&mIndices[0], sizeof(U32) * mIndexCount, NULL, NULL);
   mIB = Link.bgfx.createIndexBuffer(mem, BGFX_BUFFER_INDEX32);
}

void refresh()
{
   refreshVertexBuffer();
   refreshIndexBuffer();

   if (mRenderData == NULL)
      mRenderData = Link.Rendering.createRenderData();

   mRenderData->indexBuffer = mIB;
   mRenderData->vertexBuffer = mVB;

   // Render in Deferred
   mRenderData->shader = mShader;
   mRenderData->view = Link.Graphics.getView("DeferredGeometry", 1000);
   mRenderData->state = 0
                        | BGFX_STATE_RGB_WRITE
                        | BGFX_STATE_ALPHA_WRITE
                        | BGFX_STATE_DEPTH_TEST_LESS
                        | BGFX_STATE_DEPTH_WRITE
                        | BGFX_STATE_CULL_CCW
                        | BGFX_STATE_MSAA;

   // Transform
   bx::mtxSRT(mTransformMtx, 10, 10, 10, 0, 0, 0, 0, 0, 0);
   mRenderData->transformTable = mTransformMtx;
   mRenderData->transformCount = 1;
}

// Called when the plugin is loaded.
void create()
{
   // Load Shader
   Graphics::ShaderAsset* shaderAsset = Link.Graphics.getShaderAsset("PolyVox:renderShader");
   if (shaderAsset)
      mShader = shaderAsset->getProgram();

   // Generate polyvox sphere.
   createSphereInVolume(volData, 30);
   PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> surfaceMesh;
   PolyVox::CubicSurfaceExtractorWithNormals< PolyVox::SimpleVolume<U8> > surfaceExtractor(&volData, volData.getEnclosingRegion(), &surfaceMesh);
   surfaceExtractor.execute();

   // Verts
   const std::vector<PolyVox::PositionMaterialNormal>& vecVertices = surfaceMesh.getVertices();
   mVerts = new PosUVNormalVertex[vecVertices.size()];
   for (U32 i = 0; i < vecVertices.size(); ++i)
   {
      mVerts[mVertCount].m_x = vecVertices[i].position.getX();
      mVerts[mVertCount].m_y = vecVertices[i].position.getY();
      mVerts[mVertCount].m_z = vecVertices[i].position.getZ();

      mVerts[mVertCount].m_u = 0.0f;
      mVerts[mVertCount].m_v = 0.0f;

      mVerts[mVertCount].m_normal_x = vecVertices[i].normal.getX();
      mVerts[mVertCount].m_normal_y = vecVertices[i].normal.getY();
      mVerts[mVertCount].m_normal_z = vecVertices[i].normal.getZ();
      mVertCount++;
   }

   // Indices
   const std::vector<U32>& vecIndices = surfaceMesh.getIndices();
   mIndices = new U32[vecIndices.size()];
   for (U32 i = 0; i < vecIndices.size(); ++i)
   {
      mIndices[mIndexCount] = vecIndices[mIndexCount];
      mIndexCount++;
   }

   refresh();
}

void destroy()
{
   
}

// Per-Frame render function
void render()
{

}
