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

#ifndef _MATERIAL_ASSET_H_
#define _MATERIAL_ASSET_H_

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

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

#ifndef _BASE_COMPONENT_H_
#include "3d/entity/components/baseComponent.h"
#endif

#ifndef _RENDERINGCOMMON_H_
#include "3d/rendering/common.h"
#endif

#ifndef _MATERIAL_TEMPLATE_H_
#include "materialTemplate.h"
#endif

//-----------------------------------------------------------------------------

DefineConsoleType( TypeMaterialAssetPtr )

//-----------------------------------------------------------------------------

class DLL_PUBLIC MaterialAsset : public AssetBase
{

private:
   typedef AssetBase  Parent;

protected:
   StringTableEntry                 mVertexShaderPath;
   StringTableEntry                 mPixelShaderPath;
   StringTableEntry                 mSkinnedVertexShaderPath;

   Scene::MaterialTemplate*         mTemplate;
   StringTableEntry                 mTemplateFile;

   S32                              mTextureCount;
   Vector<bgfx::TextureHandle>      mTextureHandles; 

   Graphics::Shader*                mMatShader;
   Graphics::Shader*                mMatSkinnedShader;

public:
   MaterialAsset();
   virtual ~MaterialAsset();

   static void initPersistFields();
   virtual bool onAdd();
   virtual void onRemove();
   virtual void copyTo(SimObject* object);

   // Asset validation.
   virtual bool isAssetValid( void ) const;

   Scene::MaterialTemplate* getTemplate() { return mTemplate; }
   StringTableEntry getTemplateFile() { return mTemplateFile; }
   void setTemplateFile(StringTableEntry templateFile) { mTemplateFile = templateFile; }
   S32 getTextureCount() { return mTextureCount; }

   virtual void applyMaterial(Rendering::RenderData* renderData, bool skinned = false, Scene::BaseComponent* component = NULL);
   virtual void saveMaterial();
   virtual void compileMaterial(bool recompile = false);
   virtual void reloadMaterial();

   void loadTextures();
   Vector<bgfx::TextureHandle>* getTextureHandles() { return &mTextureHandles; }

   /// Declare Console Object.
   DECLARE_CONOBJECT(MaterialAsset);

protected:
    virtual void initializeAsset( void );
    virtual void onAssetRefresh( void );
};

MaterialAsset* getMaterialAsset(const char* id);
void createMaterialAsset(const char* name, const char* templateFile, const char* savePath);

#endif // _Base_MATERIAL_ASSET_H_