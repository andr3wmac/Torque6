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

#include "console/consoleTypes.h"
#include "csgComponent.h"
#include "graphics/core.h"
#include "rendering/common.h"

// Script bindings.
#include "csgComponent_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

// bgfx/bx
#include <bgfx/bgfx.h>
#include <bx/fpumath.h>

// Assimp - Asset Import Library
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

// CSG Functions
#include "csgFunctions.h"

using namespace Plugins;

namespace Scene
{
   IMPLEMENT_PLUGIN_CONOBJECT(CSGComponent);

   CSGComponent::CSGComponent()
   {
      Link.Con.printf("[CSG] Constructed.");

      mRenderData       = NULL;
      mVertexBuffer.idx = bgfx::invalidHandle;
      mIndexBuffer.idx  = bgfx::invalidHandle;

      // var cube = CSG.cube();
      // var sphere = CSG.sphere({ radius: 1.3 });
      // var polygons = cube.subtract(sphere).toPolygons();

      csgjs_model cubeA = createCube(0.0f, 0.0f, 0.0f);
      csgjs_model cubeB = createCube(0.5f, 0.5f, 0.5f);
      //csgjs_model cubeUnion = csgjs_union(cubeA, cubeB);
      csgjs_model cubeUnion = csgjs_intersection(cubeA, cubeB);
      //csgjs_model cubeUnion = csgjs_difference(cubeA, cubeB);

      const bgfx::Memory* mem;

      // Verts
      for (U32 i = 0; i < cubeUnion.vertices.size(); ++i)
      {
         Graphics::PosUVNormalVertex vert;
         vert.m_x = cubeUnion.vertices[i].pos.x;
         vert.m_y = cubeUnion.vertices[i].pos.y;
         vert.m_z = cubeUnion.vertices[i].pos.z;
         vert.m_normal_x = cubeUnion.vertices[i].normal.x;
         vert.m_normal_y = cubeUnion.vertices[i].normal.y;
         vert.m_normal_z = cubeUnion.vertices[i].normal.z;
         vert.m_u = cubeUnion.vertices[i].uv.x;
         vert.m_v = cubeUnion.vertices[i].uv.y;
         mVertexList.push_back(vert);
      }

      mem = Link.bgfx.makeRef(&mVertexList[0], sizeof(Graphics::PosUVNormalVertex) * mVertexList.size(), NULL, NULL);
      mVertexBuffer = Link.bgfx.createVertexBuffer(mem, *Link.Graphics.PosUVNormalVertex, BGFX_BUFFER_NONE);

      // Indices
      for (U32 i = 0; i < cubeUnion.indices.size(); i += 3)
      {
         // csg.js uses a different winding order so we need to change it back.
         mIndexList.push_back((U16)cubeUnion.indices[i + 2]);
         mIndexList.push_back((U16)cubeUnion.indices[i + 1]);
         mIndexList.push_back((U16)cubeUnion.indices[i]);
      }

      mem = Link.bgfx.makeRef(&mIndexList[0], sizeof(uint16_t) * mIndexList.size(), NULL, NULL);
      mIndexBuffer = Link.bgfx.createIndexBuffer(mem, BGFX_BUFFER_NONE);
   }

   void CSGComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();
   }

   void CSGComponent::onAddToScene()
   {  
      Link.Con.printf("[CSG] Added to scene.");

      refresh();
   }

   void CSGComponent::refresh()
   {
      Parent::refresh();

      if (!bgfx::isValid(mVertexBuffer) || !bgfx::isValid(mIndexBuffer))
         return;

      if (mRenderData != NULL)
         return;

      Link.Con.printf("[CSG] Refreshed.");

      mRenderData = Plugins::Link.Rendering.createRenderData();

      mRenderData->indexBuffer   = mIndexBuffer;
      mRenderData->vertexBuffer  = mVertexBuffer;

      // Render in Forward (for now) with our custom terrain shader.
      mRenderData->shader  = Link.Rendering.getDeferredRendering()->mDefaultShader->mProgram;
      mRenderData->view    = Plugins::Link.Graphics.getView("DeferredGeometry", 1000);
      //mRenderData->state   = 0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_DEPTH_WRITE;

      // Transform of emitter.
      mRenderData->transformTable = &mTransformMatrix[0];
      mRenderData->transformCount = 1;
   }
}
