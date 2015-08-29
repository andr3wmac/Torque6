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

#include "console/consoleTypes.h"
#include "MaterialAsset.h"
#include "graphics/core.h"

#include "3d/rendering/common.h"
#include "3d/scene/core.h"

// Script bindings.
#include "materialAsset_Binding.h"

// Debug Profiling.
#include "debug/profiler.h"

// bgfx/bx
#include <bgfx.h>
#include <bx/fpumath.h>

// Assimp - Asset Import Library
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

MaterialAsset* getMaterialAsset(const char* id)
{
   AssetPtr<MaterialAsset> result;
   StringTableEntry assetId = StringTable->insert(id);
   result.setAssetId(assetId);
   return result;
}

//------------------------------------------------------------------------------

ConsoleType( MaterialAssetPtr, TypeMaterialAssetPtr, sizeof(AssetPtr<MaterialAsset>), ASSET_ID_FIELD_PREFIX )

//-----------------------------------------------------------------------------

ConsoleGetType( TypeMaterialAssetPtr )
{
    // Fetch asset Id.
    return (*((AssetPtr<MaterialAsset>*)dptr)).getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleSetType( TypeMaterialAssetPtr )
{
    // Was a single argument specified?
    if( argc == 1 )
    {
        // Yes, so fetch field value.
        const char* pFieldValue = argv[0];

        // Fetch asset pointer.
        AssetPtr<MaterialAsset>* pAssetPtr = dynamic_cast<AssetPtr<MaterialAsset>*>((AssetPtrBase*)(dptr));

        // Is the asset pointer the correct type?
        if ( pAssetPtr == NULL )
        {
            // No, so fail.
            Con::warnf( "(TypeMaterialAssetPtr) - Failed to set asset Id '%d'.", pFieldValue );
            return;
        }

        // Set asset.
        pAssetPtr->setAssetId( pFieldValue );

        return;
   }

    // Warn.
    Con::warnf( "(TypeMaterialAssetPtr) - Cannot set multiple args to a single asset." );
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(MaterialAsset);

//------------------------------------------------------------------------------

MaterialAsset::MaterialAsset()
{
   mTemplate = NULL;
   mMatShader = NULL;
   mMatSkinnedShader = NULL;
}

//------------------------------------------------------------------------------

MaterialAsset::~MaterialAsset()
{
   //
}

//------------------------------------------------------------------------------

void MaterialAsset::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    addField("TemplateFile", TypeAssetLooseFilePath, Offset(mTemplateFile, MaterialAsset), "");
}

//------------------------------------------------------------------------------

bool MaterialAsset::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

void MaterialAsset::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//------------------------------------------------------------------------------

void MaterialAsset::onAssetRefresh( void ) 
{
    // Ignore if not yet added to the sim.
    if ( !isProperlyAdded() )
        return;

    // Call parent.
    Parent::onAssetRefresh();
}

//------------------------------------------------------------------------------

void MaterialAsset::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to asset.
    MaterialAsset* pAsset = static_cast<MaterialAsset*>(object);

    // Sanity!
    AssertFatal(pAsset != NULL, "MaterialAsset::copyTo() - Object is not the correct type.");
}

void MaterialAsset::initializeAsset( void )
{
    // Call parent.
    Parent::initializeAsset();

    Taml taml;
    mTemplate = taml.read<Scene::MaterialTemplate>( expandAssetFilePath(mTemplateFile) );

    compileMaterial();
    loadTextures();
}

bool MaterialAsset::isAssetValid() const
{
   return false;
}

void MaterialAsset::loadTextures()
{
   // Maximum of 16 textures.
   for (U32 n = 0; n < 16; ++n)
   {
      char texture_name[32];
      dSprintf(texture_name, 32, "Texture%d", n);
      const char* texture_path = expandAssetFilePath(getDataField(StringTable->insert(texture_name), NULL));
      if ( dStrlen(texture_path) == 0 ) 
         break;

      // The texture system automatically caches these so they only load once.
      TextureHandle newTexture(texture_path, TextureHandle::BitmapTexture);
      if ( newTexture.NotNull() )
         mTextureHandles.push_back( ((TextureObject*)newTexture)->getBGFXTexture() );
      else
         Con::errorf("MaterialAsset : Failed to load texture.");
   }
}

void MaterialAsset::applyMaterial(Rendering::RenderData* renderData, bool skinned, Scene::BaseComponent* component)
{
   if ( skinned )
      renderData->shader = mMatSkinnedShader->mProgram;
   else
      renderData->shader = mMatShader->mProgram;

   renderData->view = mTemplate->getRenderView();

   if ( renderData->textures != NULL )
   {
      for(S32 t = 0; t < mTextureHandles.size(); ++t)
      {
         Rendering::TextureData texture;
         texture.uniform = Graphics::Shader::getTextureUniform(t);
         texture.handle = mTextureHandles[t];
         renderData->textures->push_back(texture);
      }
   }

   if ( !mTemplate->uniforms.isEmpty() )
   {
      if ( renderData->uniforms.uniforms != NULL )
         renderData->uniforms.uniforms->merge(*mTemplate->uniforms.uniforms);
      else
         renderData->uniforms.uniforms = mTemplate->uniforms.uniforms;
   }
}

void MaterialAsset::saveMaterial()
{
   Taml tamlWriter;
   tamlWriter.write(mTemplate, expandAssetFilePath(mTemplateFile));
}

void MaterialAsset::reloadMaterial()
{
   compileMaterial();
   Scene::refresh();
}

void MaterialAsset::compileMaterial()
{
   FileObject* shaderFile = new FileObject();

   // Clear template for non-skinned versions
   mTemplate->isSkinned = false;
   mTemplate->clearVertex();
   mTemplate->clearPixel();

   // Vertex
   char vs_name[200];
   dSprintf(vs_name, 200, "%s_vs.sc", getAssetName());
   shaderFile->openForWrite(expandAssetFilePath(vs_name));
   shaderFile->writeLine((const U8*)mTemplate->getVertexShaderOutput());
   shaderFile->close();

   // Pixel
   char fs_name[200];
   dSprintf(fs_name, 200, "%s_fs.sc", getAssetName());
   shaderFile->openForWrite(expandAssetFilePath(fs_name));
   shaderFile->writeLine((const U8*)mTemplate->getPixelShaderOutput());
   shaderFile->close();

   Graphics::destroyShader(mMatShader);
   mMatShader = Graphics::getShader(expandAssetFilePath(vs_name), expandAssetFilePath(fs_name), false);

   // Clear template for skinned versions
   mTemplate->isSkinned = true;
   mTemplate->clearVertex();
   mTemplate->clearPixel();
    
   // Vertex Skinned
   char skinned_vs_name[200];
   dSprintf(skinned_vs_name, 200, "%s_skinned_vs.sc", getAssetName());
   shaderFile->openForWrite(expandAssetFilePath(skinned_vs_name));
   shaderFile->writeLine((const U8*)mTemplate->getVertexShaderOutput());
   shaderFile->close();

   // Pixel Skinned
   char skinned_fs_name[200];
   dSprintf(skinned_fs_name, 200, "%s_skinned_fs.sc", getAssetName());
   shaderFile->openForWrite(expandAssetFilePath(skinned_fs_name));
   shaderFile->writeLine((const U8*)mTemplate->getPixelShaderOutput());
   shaderFile->close();

   Graphics::destroyShader(mMatSkinnedShader);
   mMatSkinnedShader = Graphics::getShader(expandAssetFilePath(skinned_vs_name), expandAssetFilePath(skinned_fs_name), false);

   SAFE_DELETE(shaderFile);
}