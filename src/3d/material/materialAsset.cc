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
#include "graphics/textureAsset.h"

#include "3d/rendering/common.h"
#include "3d/scene/scene.h"

// Script bindings.
#include "materialAsset_Binding.h"

// Debug Profiling.
#include "debug/profiler.h"

// bgfx/bx
#include <bgfx/bgfx.h>
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

void createMaterialAsset(const char* name, const char* templateFile, const char* savePath)
{
   MaterialAsset* newAsset = new MaterialAsset();
   newAsset->setAssetName(name);
   newAsset->setTemplateFile(StringTable->insert(templateFile));

   Platform::createPath(savePath);

   // Save the module file.
   Taml taml;
   taml.write(newAsset, savePath);

   SAFE_DELETE(newAsset);
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
   mVertexShaderPath          = StringTable->insert("");
   mPixelShaderPath           = StringTable->insert("");
   mSkinnedVertexShaderPath   = StringTable->insert("");

   mTemplate         = NULL;
   mMatShader        = NULL;
   mMatSkinnedShader = NULL;

   mTextureCount = -1;
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

    addGroup("MaterialAsset");

      addField("TemplateFile", TypeAssetLooseFilePath, Offset(mTemplateFile, MaterialAsset), "");

    endGroup("MaterialAsset");

    addGroup("Textures");

      addField("TextureCount", TypeS32, Offset(mTextureCount, MaterialAsset), "");

    endGroup("Textures");
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

    // Determine paths. This should only need to be done this once.
    // Vertex
    char vs_name[200];
    dSprintf(vs_name, 200, "%s_vs.sc", getAssetName());
    mVertexShaderPath = Platform::getCachedFilePath(expandAssetFilePath(vs_name));

    // Pixel
    char fs_name[200];
    dSprintf(fs_name, 200, "%s_fs.sc", getAssetName());
    mPixelShaderPath = Platform::getCachedFilePath(expandAssetFilePath(fs_name));

    // Vertex (Skinned)
    char skinned_vs_name[200];
    dSprintf(skinned_vs_name, 200, "%s_skinned_vs.sc", getAssetName());
    mSkinnedVertexShaderPath = Platform::getCachedFilePath(expandAssetFilePath(skinned_vs_name));

    compileMaterial();
    loadTextures();
}

bool MaterialAsset::isAssetValid() const
{
   return false;
}

void MaterialAsset::loadTextures()
{
   mTextureHandles.clear();
   S32 textureCount = mTextureCount > -1 ? mTextureCount : 16;
   for (U32 n = 0; n < textureCount; ++n)
   {
      char fieldName[32];

      // Texture Asset?
      dSprintf(fieldName, 32, "TextureAsset%d", n);
      const char* textureAssetId = getDataField(StringTable->insert(fieldName), NULL);
      if (dStrlen(textureAssetId) > 0)
      {
         TextureAsset* textureAsset = getTextureAsset(textureAssetId);
         if (textureAsset != NULL && textureAsset->isTextureLoaded())
         {
            mTextureHandles.push_back(textureAsset->getTextureObject()->getBGFXTexture());
            continue;
         }
         else
            Con::errorf("MaterialAsset : Failed to load texture asset.");
      }

      // Texture File?
      dSprintf(fieldName, 32, "TextureFile%d", n);
      const char* texturePath = expandAssetFilePath(getDataField(StringTable->insert(fieldName), NULL));
      if (dStrlen(texturePath) > 0)
      {
         // The texture system automatically caches these so they only load once.
         TextureHandle newTexture(texturePath, TextureHandle::BitmapTexture);
         if (newTexture.NotNull())
         {
            mTextureHandles.push_back(((TextureObject*)newTexture)->getBGFXTexture());
            continue;
         }
         else
            Con::errorf("MaterialAsset : Failed to load texture.");
      }
   }
}

void MaterialAsset::applyMaterial(Rendering::RenderData* renderData, bool skinned, Scene::BaseComponent* component)
{
   if (mMatShader == NULL || mMatSkinnedShader == NULL)
      return;

   renderData->shader = skinned ? mMatSkinnedShader->mProgram : mMatShader->mProgram;
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
   compileMaterial(true);
   Scene::refresh();
}

void MaterialAsset::compileMaterial(bool recompile)
{
   FileObject* shaderFile = new FileObject();

   // Clear template for non-skinned versions
   mTemplate->isSkinned = false;
   mTemplate->clearVertex();
   mTemplate->clearPixel();

   // Vertex
   if (!Platform::isFile(mVertexShaderPath) || recompile)
   {
      Con::printf("Generating material vertex shader..");
      Platform::createPath(mVertexShaderPath);
      shaderFile->openForWrite(mVertexShaderPath);
      shaderFile->writeLine((const U8*)mTemplate->getVertexShaderOutput());
      shaderFile->close();
   }

   // Pixel
   if (!Platform::isFile(mPixelShaderPath) || recompile)
   {
      Con::printf("Generating material pixel shader..");
      Platform::createPath(mPixelShaderPath);
      shaderFile->openForWrite(mPixelShaderPath);
      shaderFile->writeLine((const U8*)mTemplate->getPixelShaderOutput());
      shaderFile->close();
   }

   // Clear template for skinned
   mTemplate->isSkinned = true;
   mTemplate->clearVertex();
    
   // Vertex (Skinned)
   if (!Platform::isFile(mSkinnedVertexShaderPath) || recompile)
   {
      Con::printf("Generating material skinned vertex shader..");
      Platform::createPath(mSkinnedVertexShaderPath);
      shaderFile->openForWrite(mSkinnedVertexShaderPath);
      shaderFile->writeLine((const U8*)mTemplate->getVertexShaderOutput());
      shaderFile->close();
   }

   // Mat Shader = Pixel + Vertex
   Graphics::destroyShader(mMatShader);
   mMatShader = Graphics::getShader(mVertexShaderPath, mPixelShaderPath, recompile, false);

   // Mat Skinned Shader = Pixel + Vertex (Skinned)
   Graphics::destroyShader(mMatSkinnedShader);
   mMatSkinnedShader = Graphics::getShader(mSkinnedVertexShaderPath, mPixelShaderPath, recompile, false);

   SAFE_DELETE(shaderFile);
}