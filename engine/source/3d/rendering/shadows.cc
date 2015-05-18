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


#include "shadows.h"
#include "console/consoleInternal.h"
#include "graphics/shaders.h"
#include "graphics/dgl.h"
#include "3d/scene/core.h"

#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   ShadowMapping* _shadowMappingInst = NULL;

   void shadowsInit()
   {
      if ( _shadowMappingInst != NULL ) return;
      _shadowMappingInst = new ShadowMapping();

   }

   void shadowsDestroy()
   {
      SAFE_DELETE(_shadowMappingInst);
   }

   void applyShadowMap(RenderData* renderData)
   {
      // Shadow Map
      Rendering::TextureData texture;
      texture.uniform = Graphics::Shader::getShadowmapUniform();
      texture.handle = _shadowMappingInst->shadowMapTexture;
      renderData->textures->push_front(texture);
   }

   bgfx::TextureHandle getShadowMap()
   {
      return _shadowMappingInst->shadowMapTexture;
   }

   ShadowMapping::ShadowMapping()
   {
      shadowMapSize = 512;

      shadowMapTexture.idx = bgfx::invalidHandle;
      shadowMapBuffer.idx = bgfx::invalidHandle;

      shadowmapShader = Graphics::getShader("shadows/shadowmap_vs.sc", "shadows/shadowmap_fs.sc");
      shadowmapSkinnedShader = Graphics::getShader("shadows/shadowmap_skinned_vs.sc", "shadows/shadowmap_fs.sc");
      initBuffers();

      lightMatrixUniform = Graphics::Shader::getUniform4x4Matrix("u_lightMtx");

      setRendering(true);
   }

   ShadowMapping::~ShadowMapping()
   {
      destroyBuffers();
   }

   void ShadowMapping::initBuffers()
   {
      destroyBuffers();

      const U32 samplerFlags = 0
            | BGFX_TEXTURE_RT
            | BGFX_TEXTURE_MIN_POINT
            | BGFX_TEXTURE_MAG_POINT
            | BGFX_TEXTURE_MIP_POINT
            | BGFX_TEXTURE_U_CLAMP
            | BGFX_TEXTURE_V_CLAMP;

		shadowMapTexture = bgfx::createTexture2D(shadowMapSize, shadowMapSize, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT);
		bgfx::TextureHandle fbtextures[] =
		{
			shadowMapTexture,
			bgfx::createTexture2D(shadowMapSize, shadowMapSize, 1, bgfx::TextureFormat::D16, BGFX_TEXTURE_RT_BUFFER_ONLY),
		};
		shadowMapBuffer = bgfx::createFrameBuffer(BX_COUNTOF(fbtextures), fbtextures, true);
   }

   void ShadowMapping::destroyBuffers()
   {
      // Destroy Frame Buffers
      if ( bgfx::isValid(shadowMapBuffer) )
         bgfx::destroyFrameBuffer(shadowMapBuffer);
      if ( bgfx::isValid(shadowMapTexture) )
         bgfx::destroyTexture(shadowMapTexture);
   }

   void ShadowMapping::refreshProjections()
   {
      // Light At: 50, 50, 0
      
		const float eye[3] =
		{
			Scene::directionalLightDir.x * 10.0f,
			Scene::directionalLightDir.y * 10.0f,
			Scene::directionalLightDir.z * 10.0f,
		};
		const float at[3] = { 0.0f, 0.0f, 0.0f };
		bx::mtxLookAt(lightView, eye, at);

		const float area = 150.0f;
		bx::mtxOrtho(lightProj, -area, area, -area, area, -100.0f, 200.0f);
   }

   void ShadowMapping::getShadowSampleMatrix(F32* result)
   {
      refreshProjections();

      bgfx::RendererType::Enum renderer = bgfx::getRendererType();
	   bool flipV = false
		   || renderer == bgfx::RendererType::OpenGL
		   || renderer == bgfx::RendererType::OpenGLES
		;

		const float sy = flipV ? 0.5f : -0.5f;
		const float mtxCrop[16] =
		{
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f,   sy, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f,
		};

		float mtxTmp[16];
		bx::mtxMul(mtxTmp,    lightProj, mtxCrop);
		bx::mtxMul(result,    lightView, mtxTmp);
   }

   void ShadowMapping::preRender()
   {
      refreshProjections();

      // Light Matrix
      F32 mtxShadow[16];
      _shadowMappingInst->getShadowSampleMatrix(&mtxShadow[0]);
      bgfx::setUniform(lightMatrixUniform, mtxShadow);

      // G-Buffer
      bgfx::setViewRect(Graphics::ShadowMap, 0, 0, shadowMapSize, shadowMapSize);
		bgfx::setViewFrameBuffer(Graphics::ShadowMap, shadowMapBuffer);
		bgfx::setViewTransform(Graphics::ShadowMap, lightView, lightProj);

		bgfx::setViewClear(Graphics::ShadowMap
			, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
			, UINT32_C(0x00000000), 1.0f, 0
			);

      bgfx::submit(Graphics::ShadowMap);
   }

   void ShadowMapping::render()
   {
      U64 state = 0
				| BGFX_STATE_RGB_WRITE
				| BGFX_STATE_ALPHA_WRITE
				| BGFX_STATE_DEPTH_WRITE
				| BGFX_STATE_DEPTH_TEST_LESS
				| BGFX_STATE_CULL_CCW
				;

      for (U32 n = 0; n < renderCount; ++n)
      {
         RenderData* item = &renderList[n];
         if ( item->deleted || !item->castShadow ) continue;

         // Transform Table.
         bgfx::setTransform(item->transformTable, item->transformCount);

         // We need to use a different shader if its a skinned mesh. 
         // Note: this may break down if a normal mesh uses more transforms
         //       for something.
         if ( item->transformCount > 1 )
            bgfx::setProgram(shadowmapSkinnedShader->mProgram);
         else
            bgfx::setProgram(shadowmapShader->mProgram);

         // Buffers
	      bgfx::setVertexBuffer(item->vertexBuffer);
	      bgfx::setIndexBuffer(item->indexBuffer);

	      // Set render states.
	      bgfx::setState(state);

	      // Submit primitive
	      bgfx::submit(Graphics::ShadowMap);
      }
   }

   void ShadowMapping::postRender()
   {
      // Unused
   }
}
