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

#ifndef _MEGA_TERRAIN_H_
#define _MEGA_TERRAIN_H_

#ifndef _PLUGINS_SHARED_H
#include <plugins/plugins_shared.h>
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _BASE_COMPONENT_H_
#include <scene/components/baseComponent.h>
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

namespace Scene
{
   class MegaTerrain : public BaseComponent, public Rendering::RenderHook
   {
      private:
         typedef BaseComponent Parent;

         U32                             megaTextureSize;
         bgfx::TextureHandle             megaTexture;
         bgfx::FrameBufferHandle         megaTextureBuffer;
         bgfx::ProgramHandle             megaShader;
         bool                            redrawMegaTexture;

         bgfx::TextureHandle             textures[3];
         Vector<Rendering::TextureData>  textureData;

         Rendering::UniformSet           uniformSet;
         Point2F                         lastFocusPoint;
         Graphics::ViewTableEntry*       v_TerrainMegaTexture;

      public:
         MegaTerrain();
         ~MegaTerrain();

         void onAddToScene();

         void loadTexture(S32 slot, const char* texture);
         void loadEmptyTerrain(S32 gridX, S32 gridY, S32 width, S32 height);
         void loadHeightMap(S32 gridX, S32 gridY, const char* heightMap);
         void refresh();

         virtual void preRender(Rendering::RenderCamera* camera);
         virtual void render(Rendering::RenderCamera* camera);
         virtual void postRender(Rendering::RenderCamera* camera);

         static void initPersistFields();

         DECLARE_PLUGIN_CONOBJECT(MegaTerrain);
   };
}

#endif // _MEGA_TERRAIN_H_