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

#ifndef _LIGHT_COMPONENT_H_
#define _LIGHT_COMPONENT_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _VERTEXLAYOUTS_H_
#include "graphics/core.h"
#endif

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

#ifndef _SHADERS_H_
#include "graphics/shaders.h"
#endif

#ifndef _MESH_ASSET_H_
#include "3d/entity/meshAsset.h"
#endif

#ifndef _RENDERINGCOMMON_H_
#include "3d/rendering/common.h"
#endif

#ifndef _BASE_COMPONENT_H_
#include "baseComponent.h"
#endif

namespace Scene 
{
   class LightComponent : public BaseComponent
   {
      private:
         typedef BaseComponent Parent;

         // Light Data
         Rendering::LightData*                  mLightData;
         F32                                    mLightRadius;
         ColorF                                 mLightColor;
         F32                                    mLightAtten;
         Vector<Rendering::UniformData>         uniforms;
         Vector<Rendering::TextureData>         textures;

         // Debug Render
         Rendering::RenderData*                 mRenderData;
         AssetPtr<Graphics::ShaderAsset>        mShaderAsset;

      public:
         LightComponent();

         void onAddToScene();
         void onRemoveFromScene();
         void refresh();

         F32 getRadius() { return mLightRadius; }
         void setRadius(F32 val) { mLightRadius = val; }
         ColorF getColor() { return mLightColor; }
         void setColor(ColorF val) { mLightColor = val; }
         F32 getAttenuation() { return mLightAtten; }
         void setAttenuation(F32 val) { mLightAtten = val; }

         static void initPersistFields();

         DECLARE_CONOBJECT(LightComponent);
   };
}

#endif _MESH_COMPONENT_H_