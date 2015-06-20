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

#ifndef _TEXTURE_OBJECT_H_
#include "graphics/TextureObject.h"
#endif

#ifndef _GRAPHICS_CORE_H_
#include "graphics/core.h"
#endif

#include "memory/safeDelete.h"

namespace Rendering 
{
   struct DLL_PUBLIC TextureData
   {
      bgfx::UniformHandle  uniform;
      bgfx::TextureHandle  handle;
      bool                 isDepthTexture;
      bool                 isNormalTexture;

      TextureData()
      {
         isDepthTexture = false;
         isNormalTexture = false;
         uniform.idx = bgfx::invalidHandle;
         handle.idx = bgfx::invalidHandle;
      }
   };

   struct DLL_PUBLIC UniformData
   {
      bgfx::UniformHandle  uniform;
      U32                  count;
      void*                _dataPtr;
      Point4F              _floatValues;

      UniformData()
      {
         uniform.idx = bgfx::invalidHandle;
         count = 0;
         _dataPtr = NULL;
      }

      UniformData(bgfx::UniformHandle _uniform, U32 _count = 1)
      {
         uniform = _uniform;
         count = _count;
         _dataPtr = NULL;
      }

      ~UniformData()
      {
         //
      }

      void setValue(F32 value)
      {
         _floatValues.set(value, 0.0f, 0.0f, 0.0f);
         _dataPtr = &_floatValues.x;
      }

      void setValue(Point2F value)
      {
         _floatValues.set(value.x, value.y, 0.0f, 0.0f);
         _dataPtr = &_floatValues.x;
      }

      void setValue(Point3F value)
      {
         _floatValues.set(value.x, value.y, value.z, 0.0f);
         _dataPtr = &_floatValues.x;
      }

      void setValue(Point4F value)
      {
         _floatValues.set(value.x, value.y, value.z, value.w);
         _dataPtr = &_floatValues.x;
      }
   };

   struct DLL_PUBLIC UniformSet
   {
      Vector<UniformData>* uniforms;
      bool                 _selfMade;

      UniformSet()
      {
         _selfMade = false;
         uniforms = NULL;
      }

      ~UniformSet()
      {
         if ( _selfMade )
            SAFE_DELETE(uniforms);
      }

      void create()
      {
         uniforms = new Vector<UniformData>;
         _selfMade = true;
      }

      void clear()
      {
         if ( !uniforms ) return;
         uniforms->clear();
      }

      bool isEmpty()
      {
         if ( !uniforms ) return true;
         return uniforms->size() < 1;
      }

      UniformData* addUniform()
      {
         if ( !uniforms ) create();
         uniforms->insert(0);
         return &uniforms->front();
      }
   };

   struct DLL_PUBLIC LightData
   {
      Point3F              position;
      F32                  radius;
      F32                  color[3];
      F32                  attenuation;
   };

   extern Vector<LightData> lightList;
   Vector<LightData*> getNearestLights(Point3F position);

   struct DLL_PUBLIC InstanceData
   {
      Point4F i_data0;
      Point4F i_data1;
      Point4F i_data2;
      Point4F i_data3;
      Point4F i_data4;
   };

   // Current Size: 24 Bytes. 65k = ~1.5 MB of Memory
   struct DLL_PUBLIC RenderData
   {
      bool                             deleted;
      bool                             castShadow;
      bool                             isDynamic;

      bgfx::DynamicVertexBufferHandle  dynamicVertexBuffer;
      bgfx::DynamicIndexBufferHandle   dynamicIndexBuffer;
      bgfx::VertexBufferHandle         vertexBuffer;
      bgfx::IndexBufferHandle          indexBuffer;

      bgfx::ProgramHandle              shader;

      Vector<InstanceData>*            instances;
      Vector<TextureData>*             textures;
      UniformSet                       uniforms;

      F32*                             transformTable;
      U8                               transformCount;
      Graphics::ViewTableEntry*        view;
      U64                              state;
      U32                              stateRGBA;

      TextureData* addTexture()
      {
         textures->insert(0);
         return &textures->front();
      }
   };
   extern RenderData renderList[65535];
   extern U32 renderCount;
   RenderData* createRenderData();

   extern bgfx::FrameBufferHandle   backBuffer; 
   extern bgfx::TextureHandle       colorTexture;
   extern bgfx::TextureHandle       normalTexture;
   extern bgfx::TextureHandle       matInfoTexture;
   extern bgfx::TextureHandle       depthTexture;

   bgfx::FrameBufferHandle getBackBuffer();
   bgfx::TextureHandle     getColorTexture();
   bgfx::TextureHandle     getDepthTexture();
   bgfx::TextureHandle     getNormalTexture();
   bgfx::TextureHandle     getMatInfoTexture();

   // Canvas Information
   extern bool canvasSizeChanged;
   extern U32  canvasWidth;
   extern U32  canvasHeight;
   extern U32  canvasClearColor;
   void        updateCanvas(U32 width, U32 height, U32 clearColor = 0);

   // View/Projection
   extern F32 nearPlane;
   extern F32 farPlane;
   extern F32 viewMatrix[16];
   extern F32 projectionMatrix[16];
   extern F32 projectionWidth;
   extern F32 projectionHeight;
   extern bgfx::UniformHandle u_camPos;
   extern bgfx::UniformHandle u_sceneViewMat;
   extern bgfx::UniformHandle u_sceneInvViewMat;
   extern bgfx::UniformHandle u_sceneProjMat;
   extern bgfx::UniformHandle u_sceneInvProjMat;
   extern bgfx::UniformHandle u_sceneViewProjMat;
   extern bgfx::UniformHandle u_sceneInvViewProjMat;


   // Misc
   extern bgfx::UniformHandle u_time;

   // Render Layer Views
   extern Graphics::ViewTableEntry* v_RenderLayer0;
   extern Graphics::ViewTableEntry* v_RenderLayer1;
   extern Graphics::ViewTableEntry* v_RenderLayer2;
   extern Graphics::ViewTableEntry* v_RenderLayer3;
   extern Graphics::ViewTableEntry* v_RenderLayer4;

   Point2I worldToScreen(Point3F worldPos);
   Point3F screenToWorld(Point2I screenPos);

   // Renderers
   void init();
   void destroy();
   void resize();
   
   // Process Frame
   void preRender();
   void render();
   void postRender();

   // Debug Functions
   void testGetNearestLights();
}

#endif