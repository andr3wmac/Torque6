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

#ifndef _TEXT_COMPONENT_H_
#define _TEXT_COMPONENT_H_

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
#include "3d/scene/object/meshAsset.h"
#endif

#ifndef _RENDERINGCOMMON_H_
#include "3d/rendering/common.h"
#endif

#ifndef _BASE_COMPONENT_H_
#include "baseComponent.h"
#endif

#ifndef _RENDERABLE_H_
#include <3d/rendering/renderable.h>
#endif

#ifndef NANOVG_H
#include <../common/nanovg/nanovg.h>
#endif

namespace Scene 
{
   class DLL_PUBLIC TextComponent : public virtual Rendering::Renderable, public BaseComponent
   {
      private:
         typedef BaseComponent Parent;

         Vector<Rendering::UniformData>         mUniforms;
         Vector<Rendering::TextureData>         mTextures;
         Rendering::RenderData*                 mRenderData;
         AssetPtr<Graphics::ShaderAsset>        mShaderAsset;
         bool                                   mRedrawText;
         bgfx::TextureHandle                    mTextTexture;
         StringTableEntry                       mText;
         ColorF                                 mTextColor;
         F32                                    mTextSize;
         F32                                    mTextureWidth;
         F32                                    mTextureHeight;
         F32                                    mUScrollSpeed;
         F32                                    mVScrollSpeed;

      public:
         TextComponent();
         ~TextComponent();

         virtual void preRender();
         virtual void render();
         virtual void postRender();
         virtual void onAddToScene();
         virtual void onRemoveFromScene();

         void refresh();
         void initTexture();
         void destroyTexture();
         void setText(const char* text);
         void setTextColor(ColorF textColor);
         void setTextSize(F32 textSize);
         void setTextureWidth(F32 textSize);
         void setTextureHeight(F32 textSize);

         static bool smCanRender;
         static bool renderText(F32 width, F32 height, StringTableEntry text, ColorF textColor, F32 textSize, bgfx::TextureHandle targetTexture);
         static void initPersistFields();
         static bool setText(void* obj, const char* data) { static_cast<TextComponent*>(obj)->setText(data); return false; }
         static bool setTextColor(void* obj, const char* data) 
         { 
            ColorF colorVal;
            Con::setData(TypeColorF, &colorVal, 0, 1, &data);
            static_cast<TextComponent*>(obj)->setTextColor(colorVal); 
            return false;
         }
         static bool setTextSize(void* obj, const char* data)
         {
            F32 sizeVal;
            Con::setData(TypeF32, &sizeVal, 0, 1, &data);
            static_cast<TextComponent*>(obj)->setTextSize(sizeVal);
            return false;
         }
         static bool setTextureWidth(void* obj, const char* data)
         {
            F32 widthVal;
            Con::setData(TypeF32, &widthVal, 0, 1, &data);
            static_cast<TextComponent*>(obj)->setTextureWidth(widthVal);
            return false;
         }
         static bool setTextureHeight(void* obj, const char* data)
         {
            F32 heightVal;
            Con::setData(TypeF32, &heightVal, 0, 1, &data);
            static_cast<TextComponent*>(obj)->setTextureHeight(heightVal);
            return false;
         }

         DECLARE_CONOBJECT(TextComponent);
   };
}

#endif _TEXT_COMPONENT_H_