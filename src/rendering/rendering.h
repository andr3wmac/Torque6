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


#ifndef _RENDERING_H_
#define _RENDERING_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _SHADERS_H_
#include "graphics/shaders.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx/bgfx.h>
#endif

#ifndef _TEXTURE_OBJECT_H_
#include "graphics/TextureObject.h"
#endif

#ifndef _GRAPHICS_CORE_H_
#include "graphics/core.h"
#endif

#include "memory/safeDelete.h"

#define TORQUE_MAX_RENDER_DATA 65535

class MaterialAsset;

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
         isDepthTexture    = false;
         isNormalTexture   = false;
         uniform.idx       = bgfx::invalidHandle;
         handle.idx        = bgfx::invalidHandle;
      }
   };

   struct DLL_PUBLIC UniformData
   {
      bgfx::UniformHandle  uniform;
      U32                  count;
      void*                _dataPtr;
      Point4F              _vecValues;
      F32                  _matValues[16];

      UniformData();
      UniformData(bgfx::UniformHandle _uniform, U32 _count = 1);
      ~UniformData();

      void setValue(F32 value);
      void setValue(F32* value);
      void setValue(Point2F value);
      void setValue(Point3F value);
      void setValue(Point4F value);
   };

   struct DLL_PUBLIC UniformSet
   {
      Vector<UniformData>* uniforms;
      bool                 _selfMade;

      UniformSet();
      ~UniformSet();

      void create();
      void clear();
      bool isEmpty();

      UniformData* addUniform();
      UniformData* addUniform(const UniformData& uniform);

      void addUniformSet(const UniformSet& uniformSet);
   };

   struct DLL_PUBLIC InstanceData
   {
      Point4F i_data0;
      Point4F i_data1;
      Point4F i_data2;
      Point4F i_data3;
      Point4F i_data4;
   };

   // RenderData is designed to be processed as quick as possible. Anything
   // that can be rendered using RenderData is recommended to do so.
   struct DLL_PUBLIC RenderData
   {
      enum Enum
      {
         Deleted        = BIT(0),
         Filtered       = BIT(1), // This is reset for each render.
         Hidden         = BIT(2),
         Static         = BIT(3),
         CastShadow     = BIT(4),
         UsesMaterial   = BIT(5),
         Transparent    = BIT(6),
         Skinned        = BIT(7),
         HasBounds      = BIT(8),
         DynamicBuffers = BIT(9)
      };
      U32                              flags;

      bgfx::VertexBufferHandle         vertexBuffer;
      bgfx::IndexBufferHandle          indexBuffer;
      bgfx::ProgramHandle              shader;
      Vector<InstanceData>*            instances;
      Vector<TextureData>*             textures;
      UniformSet                       uniforms;
      F32*                             transformTable;
      U8                               transformCount;
      U64                              state;
      U32                              stateRGBA;

      // IsDynamic
      bgfx::DynamicVertexBufferHandle  dynamicVertexBuffer;
      bgfx::DynamicIndexBufferHandle   dynamicIndexBuffer;
      
      // UsesMaterial
      MaterialAsset*                   material;
      
      // HasBounds
      Box3F                            boundingBox;
      SphereF                          boundingSphere;

      TextureData* addTexture()
      {
         textures->insert(0);
         return &textures->front();
      }
   };
   RenderData* createRenderData();
   RenderData* getRenderDataList();
   U32 getRenderDataCount();

   // RenderCamera is an actual rendering camera view. Either
   // to texture or to canvas.
   class RenderCamera;
   RenderCamera* createRenderCamera(StringTableEntry name, StringTableEntry renderingPath);
   RenderCamera* getRenderCamera(StringTableEntry name);
   RenderCamera* getPriorityRenderCamera();
   bool destroyRenderCamera(RenderCamera* camera);
   bool destroyRenderCamera(StringTableEntry name);

   // RenderHooks allow you to perform more complex rendering by
   // handling the process yourself.
   class DLL_PUBLIC RenderHook
   {
      public:
         virtual void beginFrame() { }
         virtual void endFrame() { }
         virtual void preRender(RenderCamera*) { }
         virtual void render(RenderCamera*) { }
         virtual void postRender(RenderCamera*) { }
   };
   void addRenderHook(RenderHook* hook);
   bool removeRenderHook(RenderHook* hook);
   Vector<RenderHook*>* getRenderHookList();

   // RenderTexture is a target for cameras and
   // input source for materials.
   struct DLL_PUBLIC RenderTexture
   {
      StringTableEntry name;
      bgfx::TextureHandle handle;
      U32 width;
      U32 height;
   };
   RenderTexture* createRenderTexture(StringTableEntry name, bgfx::BackbufferRatio::Enum ratio);
   RenderTexture* createRenderTexture(StringTableEntry name, U32 width, U32 height);
   RenderTexture* getRenderTexture(StringTableEntry name);
   bool destroyRenderTexture(StringTableEntry name);

   // Window Information
   extern bool windowSizeChanged;
   extern U32  windowWidth;
   extern U32  windowHeight;
   void        updateWindow(U32 width, U32 height);

   Point2I worldToScreen(Point3F worldPos);
   void screenToWorld(Point2I screenPos, Point3F& nearPoint, Point3F& farPoint);
   bool closestPointsOnTwoLines(Point3F& closestPointLine1, Point3F& closestPointLine2, Point3F linePoint1, Point3F lineVec1, Point3F linePoint2, Point3F lineVec2);

   // Renderers
   void init();
   void destroy();
   void resize();
   
   // Process Frame
   void render();

   // Debug Functions
   void testGetNearestLights();
}

#endif