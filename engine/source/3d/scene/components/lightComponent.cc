//-----------------------------------------------------------------------------
// Copyright (c) 2014 Andrew Mac
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
#include "lightComponent.h"
#include "graphics/vertexLayouts.h"
#include "3d/scene/rendering/forwardRendering.h"
#include "3d/assets/shaderAsset.h"

// Script bindings.
#include "lightComponent_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

// bgfx/bx
#include <bgfx.h>
#include <bx/fpumath.h>

// Assimp - Asset Import Library
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

struct PosColorVertex
{
	float m_x;
	float m_y;
	float m_z;
	uint32_t m_abgr;

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

bgfx::VertexDecl PosColorVertex::ms_decl;

static PosColorVertex s_cubeVertices[8] =
{
	{-1.0f,  1.0f,  1.0f, 0xffffffff },
	{ 1.0f,  1.0f,  1.0f, 0xffffffff },
	{-1.0f, -1.0f,  1.0f, 0xffffffff },
	{ 1.0f, -1.0f,  1.0f, 0xffffffff },
	{-1.0f,  1.0f, -1.0f, 0xffffffff },
	{ 1.0f,  1.0f, -1.0f, 0xffffffff },
	{-1.0f, -1.0f, -1.0f, 0xffffffff },
	{ 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t s_cubeIndices[36] =
{
	0, 1, 2, // 0
	1, 3, 2,
	4, 6, 5, // 2
	5, 6, 7,
	0, 2, 4, // 4
	4, 2, 6,
	1, 5, 3, // 6
	5, 7, 3,
	0, 4, 1, // 8
	4, 5, 1,
	2, 3, 6, // 10
	6, 3, 7,
};

#define BGFXCOLOR_RGBA(r,g,b,a) \
	   ((U32)((((r)&0xff)<<24)|(((g)&0xff)<<16)|(((b)&0xff)<<8)|((a)&0xff)))

namespace Scene
{
   IMPLEMENT_CONOBJECT(LightComponent);

   LightComponent::LightComponent()
   {
      mRenderData = NULL;
      mLightData = NULL;
      mVertexBuffer.idx = bgfx::invalidHandle;
      mIndexBuffer.idx = bgfx::invalidHandle;

      mLightRadius = 10.0f;
      mLightColor = ColorF(1.0f, 1.0f, 1.0f);
      mLightAtten = 0.8f;

      Con::printf("Light Component Created!");
   }

   void LightComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("Radius",      TypeF32, Offset(mLightRadius, LightComponent), "");
      addField("Color",       TypeColorF, Offset(mLightColor, LightComponent), "");
      addField("Attenuation", TypeF32, Offset(mLightAtten, LightComponent), "");
   }

   void LightComponent::onAddToScene()
   {  
      const bgfx::Memory* mem;
      PosColorVertex::init();
      
      //ColorI lightColor(mLightColor);
      //U32 vertColor = BGFXCOLOR_RGBA(255, lightColor.blue, lightColor.green, lightColor.red);

      // Create static vertex buffer.
	   mem = bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices) );
	   mVertexBuffer = bgfx::createVertexBuffer(mem, PosColorVertex::ms_decl);

	   // Create static index buffer.
	   mem = bgfx::makeRef(s_cubeIndices, sizeof(s_cubeIndices) );
	   mIndexBuffer = bgfx::createIndexBuffer(mem);

      // Setup Forward Render Data (for now)
      //Scene::ForwardRenderData data;
      //Scene::forwardRenderList.push_back(data);
      //mRenderData = &Scene::forwardRenderList.back();

      //mRenderData->uniforms.clear();
      //Scene::UniformData lightUniformData;
      //lightUniformData.count = 1;
      //lightUniformData.data = &mLightColor.red;
      //lightUniformData.uniform = Graphics::Shader::getUniform("lightColor");
      //mRenderData->uniforms.push_back(lightUniformData);
      

      // Setup Light Data
      Scene::LightData light_data;
      Scene::lightList.push_back(light_data);
      mLightData = &Scene::lightList.back();

      mShaderAsset.setAssetId(StringTable->insert("AnimatedMeshExample:lightShader"));

      refresh();
   }

   void LightComponent::refresh()
   {
      Parent::refresh();

      // Sanity Checks.
      if ( mOwnerEntity == NULL ) return;
      //if ( mRenderData == NULL ) return;
      if ( mLightData == NULL ) return;
      //if ( mShaderAsset.isNull() ) return;

      // Material Data
      //mRenderData->shader = mShaderAsset->getProgram();

      // Base Component transform matrix is always slot 0 in the transform table.
      //mRenderData->transformTable = mTransformMatrix;
      //mRenderData->transformCount = 1;

      // Update render data.
      //mRenderData->indexBuffer = mIndexBuffer;
      //mRenderData->vertexBuffer = mVertexBuffer;

      mLightData->position = mOwnerEntity->mPosition + mPosition;
      Con::printf("Light Position: %f %f %f", mLightData->position.x, mLightData->position.y, mLightData->position.z);
      mLightData->radius = mLightRadius;
      mLightData->color[0] = mLightColor.red;
      mLightData->color[1] = mLightColor.green;
      mLightData->color[2] = mLightColor.blue;
      mLightData->attenuation = mLightAtten;
   }
}