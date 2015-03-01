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


#ifndef _RENDERINGCOMMON_H_
#define _RENDERINGCOMMON_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _SHADERS_H_
#include "graphics/shaders.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx.h>
#endif

#include "memory/safeDelete.h"

namespace Rendering 
{
   struct TexureData
   {
      bgfx::UniformHandle  uniform;
      bgfx::TextureHandle  handle;
      bool                 isDepthTexture;

      TexureData()
      {
         isDepthTexture = false;
         uniform.idx = bgfx::invalidHandle;
         handle.idx = bgfx::invalidHandle;
      }
   };

   struct UniformData
   {
      bgfx::UniformHandle  uniform;
      void*                data;
      U32                  count;

      UniformData()
      {
         uniform.idx = bgfx::invalidHandle;
         data = NULL;
         count = 0;
      }

      UniformData(bgfx::UniformHandle _uniform, void* _data = NULL, U32 _count = 1)
      {
         uniform = _uniform;
         data = _data;
         count = _count;
      }

      ~UniformData()
      {
         SAFE_DELETE(data);
      }
   };

   struct LightData
   {
      Point3F              position;
      F32                  radius;
      F32                  color[3];
      F32                  attenuation;
   };

   extern Vector<LightData> lightList;
   Vector<LightData*> getNearestLights(Point3F position);

   // Current Size: 24 Bytes. 65k = ~1.5 MB of Memory
   struct RenderData
   {
      bgfx::VertexBufferHandle      vertexBuffer;
      bgfx::IndexBufferHandle       indexBuffer;
      bgfx::ProgramHandle           shader;

      Vector<TexureData>*           textures;
      Vector<UniformData>*          uniforms;

      F32*                          transformTable;
      U8                            transformCount;
      U8                            view;
      U64                           state;
   };
   extern RenderData renderList[65535];
   extern U32 renderCount;
   RenderData* createRenderData();

   extern bgfx::TextureHandle finalTexture;
   bgfx::TextureHandle getFinalTexture();
   extern bgfx::TextureHandle depthTexture;
   bgfx::TextureHandle getDepthTexture();

   // Canvas Information
   extern bool canvasSizeChanged;
   extern U32 canvasWidth;
   extern U32 canvasHeight;
   extern U32 canvasClearColor;

   // View/Projection
   extern F32 viewMatrix[16];
   extern F32 projectionMatrix[16];

   // Renderers
   void init();
   void destroy();
   void resize();
   
   // Process Frame
   void render(U32 width, U32 height, U32 clearColor = 0);

   // Debug Functions
   void testGetNearestLights();
}

#endif