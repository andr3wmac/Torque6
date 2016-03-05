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


#include "deferredShading.h"
#include "console/consoleInternal.h"
#include "graphics/shaders.h"
#include "graphics/dgl.h"
#include "scene/scene.h"
#include "rendering/renderCamera.h"
#include "materials/materialAsset.h"

#include <bgfx/bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   DeferredShading::DeferredShading(RenderCamera* camera)
   {
      mCamera        = camera;
      mInitialized   = false;
      
      mBackBuffer.idx         = bgfx::invalidHandle;
      mDepthBufferRead.idx    = bgfx::invalidHandle;
      mGBufferTextures[0].idx = bgfx::invalidHandle;
      mGBufferTextures[1].idx = bgfx::invalidHandle;
      mGBufferTextures[2].idx = bgfx::invalidHandle;
      mGBufferTextures[3].idx = bgfx::invalidHandle;
      mGBuffer.idx            = bgfx::invalidHandle;
      mDecalBuffer.idx        = bgfx::invalidHandle;
      mLightBuffer.idx        = bgfx::invalidHandle;
      mAmbientBuffer.idx      = bgfx::invalidHandle;
      mFinalBuffer.idx        = bgfx::invalidHandle;

      mCombineShader          = NULL;
      mDefaultShader          = NULL;
      mDeferredGeometryView   = NULL;
      mDeferredDecalView      = NULL;
      mDeferredLightView      = NULL;
      mDeferredAmbientView    = NULL;
      mDeferredFinalView      = NULL;
   }

   DeferredShading::~DeferredShading()
   {
      destroy();
   }

   void DeferredShading::init()
   {
      destroy();

      // Shaders
      mCombineShader = Graphics::getDefaultShader("rendering/combine_vs.tsh", "rendering/combine_fs.tsh");
      mDefaultShader = Graphics::getDefaultShader("rendering/default_deferred_vs.tsh", "rendering/default_deferred_fs.tsh");

      // Get Views
      mBackBufferView         = Graphics::getView("BackBuffer", 2000, mCamera);
      mDeferredGeometryView   = Graphics::getView("DeferredGeometry", 1000, mCamera);
      mDeferredDecalView      = Graphics::getView("DeferredDecal", 1250, mCamera);
      mDeferredLightView      = Graphics::getView("DeferredLight", 1500, mCamera);
      mDeferredAmbientView    = Graphics::getView("DeferredAmbient", 1600, mCamera);
      mDeferredFinalView      = Graphics::getView("DeferredFinal", 1750, mCamera);

      const uint32_t samplerFlags = 0
         | BGFX_TEXTURE_RT
         | BGFX_TEXTURE_MIN_POINT
         | BGFX_TEXTURE_MAG_POINT
         | BGFX_TEXTURE_MIP_POINT
         | BGFX_TEXTURE_U_CLAMP
         | BGFX_TEXTURE_V_CLAMP
         ;

      // G-Buffer
      mGBufferTextures[0]  = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, 1, bgfx::TextureFormat::BGRA8, samplerFlags); // Color
      mGBufferTextures[1]  = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, 1, bgfx::TextureFormat::BGRA8, samplerFlags); // Normals
      mGBufferTextures[2]  = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, 1, bgfx::TextureFormat::BGRA8, samplerFlags); // Mat Info
      mGBufferTextures[3]  = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, 1, bgfx::TextureFormat::D24, samplerFlags);   // Depth
      mGBuffer             = bgfx::createFrameBuffer(BX_COUNTOF(mGBufferTextures), mGBufferTextures, false);

      // BackBuffer
      mBackBufferTexture                        = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, 1, bgfx::TextureFormat::BGRA8, samplerFlags);
      bgfx::TextureHandle backBufferTextures[2] = { mBackBufferTexture, mGBufferTextures[3] };
      mBackBuffer                               = bgfx::createFrameBuffer(BX_COUNTOF(backBufferTextures), backBufferTextures, false);

      // Depth Buffer
      mDepthBufferRead = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, 1, bgfx::TextureFormat::D24, BGFX_TEXTURE_BLIT_DST);

      // Decal Buffer
      bgfx::TextureHandle decalBufferTextures[] =
      {
         mGBufferTextures[0],
         mGBufferTextures[3]
      };
      mDecalBuffer = bgfx::createFrameBuffer(BX_COUNTOF(decalBufferTextures), decalBufferTextures);

      // Light Buffer
      mLightBuffer = bgfx::createFrameBuffer(bgfx::BackbufferRatio::Equal, bgfx::TextureFormat::RGBA16, samplerFlags);

      // Ambient Buffer
      mAmbientBuffer = bgfx::createFrameBuffer(bgfx::BackbufferRatio::Equal, bgfx::TextureFormat::BGRA8, samplerFlags);

      // Final Buffer
      bgfx::TextureHandle finalBufferTextures[] =
      {
         mBackBufferTexture,
         bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, 1, bgfx::TextureFormat::D16, BGFX_TEXTURE_RT_WRITE_ONLY)
      };
      mFinalBuffer = bgfx::createFrameBuffer(BX_COUNTOF(finalBufferTextures), finalBufferTextures);

      mInitialized = true;
   }

   void DeferredShading::destroy()
   {
      if (bgfx::isValid(mBackBuffer))
         bgfx::destroyFrameBuffer(mBackBuffer);
      if (bgfx::isValid(mDepthBufferRead))
         bgfx::destroyTexture(mDepthBufferRead);

      // Destroy Frame Buffers
      if ( bgfx::isValid(mGBuffer) )
         bgfx::destroyFrameBuffer(mGBuffer);
      if (bgfx::isValid(mDecalBuffer))
         bgfx::destroyFrameBuffer(mDecalBuffer);
      if ( bgfx::isValid(mLightBuffer) )
         bgfx::destroyFrameBuffer(mLightBuffer);
      if (bgfx::isValid(mAmbientBuffer))
         bgfx::destroyFrameBuffer(mAmbientBuffer);
      if (bgfx::isValid(mFinalBuffer))
         bgfx::destroyFrameBuffer(mFinalBuffer);

      // Destroy G-Buffer Textures
      if ( bgfx::isValid(mGBufferTextures[0]) )
         bgfx::destroyTexture(mGBufferTextures[0]);
      if (bgfx::isValid(mGBufferTextures[1]))
         bgfx::destroyTexture(mGBufferTextures[1]);
      if (bgfx::isValid(mGBufferTextures[2]))
         bgfx::destroyTexture(mGBufferTextures[2]);
      if (bgfx::isValid(mGBufferTextures[3]))
         bgfx::destroyTexture(mGBufferTextures[3]);
   }

   void DeferredShading::preRender()
   {
      if (!mInitialized)
         init();

      // BackBuffer
      bgfx::setViewFrameBuffer(mBackBufferView->id, mBackBuffer);
      bgfx::setViewRect(mBackBufferView->id, 0, 0, canvasWidth, canvasHeight);
      bgfx::setViewTransform(mBackBufferView->id, mCamera->viewMatrix, mCamera->projectionMatrix);
      bgfx::touch(mBackBufferView->id);

      // G-Buffer
      bgfx::setPaletteColor(0, UINT32_C(0x00000000));

      // Geometry Buffer
      bgfx::setViewClear(mDeferredGeometryView->id
         , BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
         , 1.0f
         , 0
         , 0
         , 0
         , 0
         );
      bgfx::setViewRect(mDeferredGeometryView->id, 0, 0, canvasWidth, canvasHeight);
      bgfx::setViewFrameBuffer(mDeferredGeometryView->id, mGBuffer);
      bgfx::setViewTransform(mDeferredGeometryView->id, mCamera->viewMatrix, mCamera->projectionMatrix);
      bgfx::touch(mDeferredGeometryView->id);

      // Decal Buffer
      bgfx::setViewRect(mDeferredDecalView->id, 0, 0, canvasWidth, canvasHeight);
      bgfx::setViewFrameBuffer(mDeferredDecalView->id, mDecalBuffer);
      bgfx::setViewTransform(mDeferredDecalView->id, mCamera->viewMatrix, mCamera->projectionMatrix);
      bgfx::touch(mDeferredDecalView->id);

      // Light Buffer
      bgfx::setViewClear(mDeferredLightView->id
         , BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
         , 1.0f
         , 0
         , 0
         );
      bgfx::setViewRect(mDeferredLightView->id, 0, 0, canvasWidth, canvasHeight);
      bgfx::setViewFrameBuffer(mDeferredLightView->id, mLightBuffer);
      bgfx::setViewTransform(mDeferredLightView->id, mCamera->viewMatrix, mCamera->projectionMatrix);
      bgfx::touch(mDeferredLightView->id);

      // Ambient Buffer
      bgfx::setViewClear(mDeferredAmbientView->id
         , BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
         , 1.0f
         , 0
         , 0
         );
      bgfx::setViewRect(mDeferredAmbientView->id, 0, 0, canvasWidth, canvasHeight);
      bgfx::setViewFrameBuffer(mDeferredAmbientView->id, mAmbientBuffer);
      bgfx::setViewTransform(mDeferredAmbientView->id, mCamera->viewMatrix, mCamera->projectionMatrix);
      bgfx::touch(mDeferredAmbientView->id);

      // Final Buffer
      bgfx::setViewFrameBuffer(mDeferredFinalView->id, mFinalBuffer);
      bgfx::setViewRect(mDeferredFinalView->id, 0, 0, canvasWidth, canvasHeight);
      bgfx::setViewTransform(mDeferredFinalView->id, mCamera->viewMatrix, mCamera->projectionMatrix);

      // Temp hack.
      bgfx::blit(mDeferredDecalView->id, getDepthTextureRead(), 0, 0, getDepthTexture());
   }

   void DeferredShading::render()
   {
      // Render everything in the render list.
      Rendering::RenderData* item = Rendering::getRenderDataList();
      for (U32 n = 0; n < Rendering::getRenderDataCount(); ++n, ++item)
      {
         if (item->flags & RenderData::Deleted || item->flags & RenderData::Hidden)
            continue;

         // Transform Table.
         bgfx::setTransform(item->transformTable, item->transformCount);

         // Instancing Data
         if (item->instances && item->instances->size() > 0)
         {
            U16 stride = sizeof(Rendering::InstanceData);
            const bgfx::InstanceDataBuffer* idb = bgfx::allocInstanceDataBuffer(item->instances->size(), stride);

            for (S32 i = 0; i < item->instances->size(); ++i)
               dMemcpy(&idb->data[i * stride], &item->instances->at(i), stride);

            bgfx::setInstanceDataBuffer(idb);
         }

         // Vertex/Index Buffers (Optionally Dynamic)
         if (item->flags & RenderData::IsDynamic)
         {
            bgfx::setVertexBuffer(item->dynamicVertexBuffer);
            bgfx::setIndexBuffer(item->dynamicIndexBuffer);
         }
         else {
            bgfx::setVertexBuffer(item->vertexBuffer);
            bgfx::setIndexBuffer(item->indexBuffer);
         }

         // Setup Textures
         if (item->textures)
         {
            for (S32 i = 0; i < item->textures->size(); ++i)
            {
               if (item->textures->at(i).isDepthTexture)
                  bgfx::setTexture(i, item->textures->at(i).uniform, getDepthTexture());
               else if (item->textures->at(i).isNormalTexture)
                  bgfx::setTexture(i, item->textures->at(i).uniform, getNormalTexture());
               else
                  bgfx::setTexture(i, item->textures->at(i).uniform, item->textures->at(i).handle);
            }
         }

         // Setup Uniforms
         if (!item->uniforms.isEmpty())
         {
            for (S32 i = 0; i < item->uniforms.uniforms->size(); ++i)
            {
               UniformData* uniform = &item->uniforms.uniforms->at(i);
               bgfx::setUniform(uniform->uniform, uniform->_dataPtr, uniform->count);
            }
         }

         // Set render states.
         bgfx::setState(item->state, item->stateRGBA);

         // Check if this is a material render.
         if ( item->flags & RenderData::UsesMaterial )
         {
            // Let the material submit for us.
            item->material->submit(mDeferredGeometryView->id, (item->flags & RenderData::Skinned));
         }
         else 
         {
            // Submit primitive
            if (bgfx::isValid(item->shader))
               bgfx::submit(mDeferredGeometryView->id, item->shader);
            else
               bgfx::submit(mDeferredGeometryView->id, mDefaultShader->mProgram);
         }
      }
   }

   void DeferredShading::postRender()
   {
      // This projection matrix is used because its a full screen quad.
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
      bgfx::setViewTransform(mDeferredFinalView->id, NULL, proj);
      bgfx::setViewRect(mDeferredFinalView->id, 0, 0, canvasWidth, canvasHeight);

      // Combine Color + Direct Light
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), mGBuffer, 0);        // Albedo
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), mGBuffer, 1);        // Normals
      bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), mGBuffer, 2);        // Material Info
      bgfx::setTexture(3, Graphics::Shader::getTextureUniform(3), mGBuffer, 3);        // Depth Buffer
      bgfx::setTexture(4, Graphics::Shader::getTextureUniform(4), mLightBuffer, 0);    // Light Buffer
      bgfx::setTexture(5, Graphics::Shader::getTextureUniform(5), mAmbientBuffer, 0);  // Ambient Buffer

      bgfx::setState(0
         | BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
         );

      fullScreenQuad((F32)canvasWidth, (F32)canvasHeight);

      bgfx::submit(mDeferredFinalView->id, mCombineShader->mProgram);
   }

   void DeferredShading::resize()
   {

   }

   bgfx::FrameBufferHandle DeferredShading::getBackBuffer()
   {
      return mBackBuffer;
   }

   bgfx::TextureHandle DeferredShading::getColorTexture()
   {
      return mGBufferTextures[0];
   }

   bgfx::TextureHandle DeferredShading::getDepthTexture()
   {
      return mGBufferTextures[3];
   }

   bgfx::TextureHandle DeferredShading::getDepthTextureRead()
   {
      return mDepthBufferRead;
   }

   bgfx::TextureHandle DeferredShading::getNormalTexture()
   {
      return mGBufferTextures[1];
   }

   bgfx::TextureHandle DeferredShading::getMatInfoTexture()
   {
      return mGBufferTextures[2];
   }
}
