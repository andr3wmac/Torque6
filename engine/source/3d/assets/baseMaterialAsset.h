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

#ifndef _BASE_MATERIAL_ASSET_H_
#define _BASE_MATERIAL_ASSET_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _VERTEXLAYOUTS_H_
#include "graphics/utilities.h"
#endif

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

#ifndef _SHADERS_H_
#include "graphics/shaders.h"
#endif

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

#ifndef _BASE_COMPONENT_H_
#include "3d/scene/components/baseComponent.h"
#endif

#ifndef _RENDERINGCOMMON_H_
#include "3d/rendering/common.h"
#endif

//-----------------------------------------------------------------------------

DefineConsoleType( TypeBaseMaterialAssetPtr )

//-----------------------------------------------------------------------------

class BaseMaterialAsset : public AssetBase
{

private:
   typedef AssetBase  Parent;

protected:
   StringTableEntry                 mShaderAssetId;
   AssetPtr<Graphics::ShaderAsset>  mShaderAsset;
   Vector<bgfx::TextureHandle>      mTextureHandles; 

public:
   BaseMaterialAsset();
   virtual ~BaseMaterialAsset();

   static void initPersistFields();
   virtual bool onAdd();
   virtual void onRemove();
   virtual void copyTo(SimObject* object);

   // Asset validation.
   virtual bool isAssetValid( void ) const;

   virtual void applyMaterial(Rendering::RenderData* renderData, Scene::BaseComponent* component = NULL);

   void loadTextures();
   Vector<bgfx::TextureHandle> getTextureHandles() { return mTextureHandles; }

   void setShader( const char* pShaderAssetId );
   AssetPtr<Graphics::ShaderAsset> getShader() { return mShaderAsset; }

   /// Declare Console Object.
   DECLARE_CONOBJECT(BaseMaterialAsset);

   static bool setShader(void* obj, const char* data) { static_cast<BaseMaterialAsset*>(obj)->setShader( data ); return false; }

protected:
    virtual void initializeAsset( void );
    virtual void onAssetRefresh( void );
};

#endif // _Base_MATERIAL_ASSET_H_