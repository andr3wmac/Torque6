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

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx.h>
#endif

namespace Rendering 
{
   struct TexureData
   {
      bgfx::UniformHandle  uniform;
      bgfx::TextureHandle  handle;
   };

   struct UniformData
   {
      bgfx::UniformHandle  uniform;
      const void*          data;
      U32                  count;
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
   };
   extern RenderData renderList[65535];
   extern U32 renderCount;

   RenderData* createRenderData();
   void render();

   // Debug Functions
   void dumpForwardRenderData();
   void testGetNearestLights();
}

#endif