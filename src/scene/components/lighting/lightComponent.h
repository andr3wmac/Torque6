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
#include "mesh/meshAsset.h"
#endif

#ifndef _RENDERING_H_
#include "rendering/rendering.h"
#endif

#ifndef _BASE_COMPONENT_H_
#include "scene/components/baseComponent.h"
#endif

namespace Scene 
{
   class DLL_PUBLIC LightComponent : public BaseComponent, public Rendering::RenderHook
   {
      private:
         typedef BaseComponent Parent;

         // Light Data
         Rendering::LightData*                  mLightData;
         ColorF                                 mLightTint;
         F32                                    mLightAttenuation;
         F32                                    mLightIntensity;

         Graphics::ViewTableEntry*              mDeferredLightView;
         bgfx::ProgramHandle                    mDeferredLightShader;
         bgfx::UniformHandle                    mDeferredLightPosUniform;
         bgfx::UniformHandle                    mDeferredLightColorUniform;
         bgfx::UniformHandle                    mDeferredLightParamsUniform;
         AssetPtr<Graphics::ShaderAsset>        mDeferredShaderAsset;

      public:
         LightComponent();

         void onAddToScene();
         void onRemoveFromScene();
         void refresh();

         virtual void preRender(Rendering::RenderCamera* camera);
         virtual void render(Rendering::RenderCamera* camera);
         virtual void postRender(Rendering::RenderCamera* camera);

         static void initPersistFields();

         DECLARE_CONOBJECT(LightComponent);
   };
}

#endif _MESH_COMPONENT_H_