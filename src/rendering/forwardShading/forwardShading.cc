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


#include "forwardShading.h"
#include "console/consoleInternal.h"
#include "graphics/shaders.h"
#include "graphics/dgl.h"
#include "scene/scene.h"
#include "rendering/renderCamera.h"
#include "materials/materials.h"
#include "materials/materialAsset.h"

#include <bgfx/bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Rendering
{
   IMPLEMENT_RENDER_PATH("ForwardShading", ForwardShading);

   ForwardShading::ForwardShading(RenderCamera* camera)
      : RenderPath(camera)
   {
      mBackBuffer.idx               = bgfx::invalidHandle;
      mDepthBuffer.idx              = bgfx::invalidHandle;
      mDepthBufferRead.idx          = bgfx::invalidHandle;
      mDefaultShader                = NULL;
      mForwardMaterialVariantIndex  = Materials::getVariantIndex("forward");
   }

   ForwardShading::~ForwardShading()
   {
      destroy();
   }

   void ForwardShading::init()
   {
      destroy();

      // Shaders
      mDefaultShader = Graphics::getDefaultShader("rendering/default_forward_vs.tsh", "rendering/default_forward_fs.tsh");

      // Get Views
      mBackBufferView         = Graphics::getView("BackBuffer", 2000, mCamera);

      const uint32_t samplerFlags = 0
         | BGFX_TEXTURE_RT
         | BGFX_TEXTURE_MIN_POINT
         | BGFX_TEXTURE_MAG_POINT
         | BGFX_TEXTURE_MIP_POINT
         | BGFX_TEXTURE_U_CLAMP
         | BGFX_TEXTURE_V_CLAMP
         ;

      // BackBuffer
      mBackBufferTexture                        = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, 1, bgfx::TextureFormat::BGRA8, samplerFlags); // Main Buffer
      mDepthBuffer                              = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, 1, bgfx::TextureFormat::D24, samplerFlags);   // Depth
      bgfx::TextureHandle backBufferTextures[2] = { mBackBufferTexture, mDepthBuffer };
      mBackBuffer                               = bgfx::createFrameBuffer(BX_COUNTOF(backBufferTextures), backBufferTextures, false);

      // Depth Buffer
      mDepthBufferRead = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, 1, bgfx::TextureFormat::D24, BGFX_TEXTURE_BLIT_DST);

      mInitialized = true;
   }

   void ForwardShading::destroy()
   {
      if (bgfx::isValid(mBackBuffer))
         bgfx::destroyFrameBuffer(mBackBuffer);

      if (bgfx::isValid(mDepthBuffer))
         bgfx::destroyTexture(mDepthBuffer);
      if (bgfx::isValid(mDepthBufferRead))
         bgfx::destroyTexture(mDepthBufferRead);
   }

   void ForwardShading::preRender()
   {
      if (!mInitialized)
         init();

      // BackBuffer
      bgfx::setViewFrameBuffer(mBackBufferView->id, mBackBuffer);
      bgfx::setViewRect(mBackBufferView->id, 0, 0, canvasWidth, canvasHeight);
      bgfx::setViewTransform(mBackBufferView->id, mCamera->viewMatrix, mCamera->projectionMatrix);
      bgfx::touch(mBackBufferView->id);

      // Temp hack.
      //bgfx::blit(mDeferredDecalView->id, getDepthTextureRead(), 0, 0, getDepthTexture());
   }

   void ForwardShading::render()
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
            item->material->submit(mBackBufferView->id, (item->flags & RenderData::Skinned), mForwardMaterialVariantIndex);
         }
         else 
         {
            // Submit primitive
            if (bgfx::isValid(item->shader))
               bgfx::submit(mBackBufferView->id, item->shader);
            else
               bgfx::submit(mBackBufferView->id, mDefaultShader->mProgram);
         }
      }
   }

   void ForwardShading::postRender()
   {
      //
   }

   void ForwardShading::resize()
   {

   }

   bgfx::FrameBufferHandle ForwardShading::getBackBuffer()
   {
      return mBackBuffer;
   }

   bgfx::TextureHandle ForwardShading::getDepthTexture()
   {
      return mDepthBuffer;
   }

   bgfx::TextureHandle ForwardShading::getDepthTextureRead()
   {
      return mDepthBufferRead;
   }
}
