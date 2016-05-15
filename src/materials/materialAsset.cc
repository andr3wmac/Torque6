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
#include "materialAsset.h"
#include "graphics/core.h"
#include "graphics/textureAsset.h"
#include "materials.h"
#include "rendering/rendering.h"
#include "scene/scene.h"

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
   mTemplate      = NULL;
   mTextureCount  = -1;
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

      addProtectedField("TemplateFile", TypeAssetLooseFilePath, Offset(mTemplateFile, MaterialAsset), &setTemplateFile, &defaultProtectedGetFn, "");

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

   // Load Textures
   loadTextures();
}

void MaterialAsset::loadTextures()
{
   // Load textures
   mTextureHandles.clear();
   S32 textureCount = mTextureCount > -1 ? mTextureCount : 16;
   for (S32 n = 0; n < textureCount; ++n)
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

      // Render Texture?
      dSprintf(fieldName, 32, "RenderTexture%d", n);
      const char* renderTextureName = getDataField(StringTable->insert(fieldName), NULL);
      if (dStrlen(renderTextureName) > 0)
      {
         Rendering::RenderTexture* rt = Rendering::createRenderTexture(StringTable->insert(renderTextureName), bgfx::BackbufferRatio::Equal);
         mTextureHandles.push_back(rt->handle);
      }
   }
}

bool MaterialAsset::isAssetValid() const
{
   return false;
}

void MaterialAsset::setTemplateFile(const char* templateFile)
{ 
   mTemplateFile = StringTable->insert(templateFile);

   Taml taml;
   mTemplate = taml.read<Materials::MaterialTemplate>(expandAssetFilePath(mTemplateFile));
}

void MaterialAsset::destroyShaders()
{
   for (S32 n = 0; n < mShaders.size(); ++n)
   {
      Graphics::destroyShader(mShaders[n]);
   }

   for (S32 n = 0; n < mSkinnedShaders.size(); ++n)
   {
      Graphics::destroyShader(mSkinnedShaders[n]);
   }
}

void MaterialAsset::applyMaterial(Rendering::RenderData* renderData)
{
   if (mTemplate == NULL)
      return;

   renderData->material = this;
   renderData->flags |= Rendering::RenderData::UsesMaterial;

   if (renderData->textures != NULL)
   {
      for (S32 t = 0; t < mTextureHandles.size(); ++t)
      {
         Rendering::TextureData texture;
         texture.uniform = Graphics::Shader::getTextureUniform(t);
         texture.handle = mTextureHandles[t];
         renderData->textures->push_back(texture);
      }
   }

   if (!mTemplate->uniforms.isEmpty())
   {
      if (renderData->uniforms.uniforms != NULL)
         renderData->uniforms.addUniformSet(mTemplate->uniforms);
      else
         renderData->uniforms.uniforms = mTemplate->uniforms.uniforms;
   }
}

void MaterialAsset::submit(U8 viewID, bool skinned, S32 variantIndex)
{
   if (mShaders.size() < 1 || mSkinnedShaders.size() < 1)
   {
      bgfx::touch(viewID);
      return;
   }

   if (variantIndex < 0)
      bgfx::submit(viewID, skinned ? mSkinnedShaders[0]->mProgram : mShaders[0]->mProgram);
   else
      bgfx::submit(viewID, skinned ? mSkinnedShaders[1 + variantIndex]->mProgram : mShaders[1 + variantIndex]->mProgram);
}

void MaterialAsset::saveMaterial()
{
   Taml tamlWriter;
   tamlWriter.write(mTemplate, expandAssetFilePath(mTemplateFile));
}

void MaterialAsset::compileMaterial(bool recompile)
{
   if (mTemplate == NULL)
      return;

   // Clear old shaders
   destroyShaders();

   // Compile regular material.
   compileMaterialVariant(NULL, recompile);

   // Compile each material variant.
   Vector<const char*> materialVariants = Materials::getVariants();
   for (S32 n = 0; n < materialVariants.size(); ++n)
   {
      compileMaterialVariant(materialVariants[n], recompile);
   }
}

void MaterialAsset::compileMaterialVariant(const char* variant, bool recompile)
{
   FileObject* shaderFile = new FileObject();

   Materials::MaterialGenerationSettings settings;
   settings.matTemplate = mTemplate;
   settings.matVariant = variant;

   char variantPrefix[32];
   variantPrefix[0] = '\0';
   if (variant != NULL)
      dSprintf(variantPrefix, 32, "%s_", variant);

   // Clear template for non-skinned versions
   mTemplate->clearShader();

   // Is this a valid variant?
   if (!mTemplate->isValid(settings))
   {
      Con::printf("Invalid Material: %s_%s", getAssetName(), variant);
      return;
   }

   // Determine paths.
   // Vertex
   char vs_name[200];
   dSprintf(vs_name, 200, "%s_%svs.tsh", getAssetName(), variantPrefix);
   StringTableEntry mVertexShaderPath = Platform::getCachedFilePath(expandAssetFilePath(vs_name));

   // Pixel
   char fs_name[200];
   dSprintf(fs_name, 200, "%s_%sfs.tsh", getAssetName(), variantPrefix);
   StringTableEntry mPixelShaderPath = Platform::getCachedFilePath(expandAssetFilePath(fs_name));

   // Vertex (Skinned)
   char skinned_vs_name[200];
   dSprintf(skinned_vs_name, 200, "%s_%sskinned_vs.tsh", getAssetName(), variantPrefix);
   StringTableEntry mSkinnedVertexShaderPath = Platform::getCachedFilePath(expandAssetFilePath(skinned_vs_name));

   // Vertex
   if (!Platform::isFile(mVertexShaderPath) || recompile)
   {
      Con::printf("Generating material %svertex shader..", variantPrefix);
      Platform::createPath(mVertexShaderPath);
      shaderFile->openForWrite(mVertexShaderPath);
      shaderFile->writeString((const U8*)mTemplate->getVertexShaderOutput(settings));
      shaderFile->close();
   }

   // Pixel
   if (!Platform::isFile(mPixelShaderPath) || recompile)
   {
      Con::printf("Generating material %spixel shader..", variantPrefix);
      Platform::createPath(mPixelShaderPath);
      shaderFile->openForWrite(mPixelShaderPath);
      shaderFile->writeString((const U8*)mTemplate->getPixelShaderOutput(settings));
      shaderFile->close();
   }

   // Clear template for skinned
   settings.isSkinned = true;
   mTemplate->clearShader();

   // Vertex (Skinned)
   if (!Platform::isFile(mSkinnedVertexShaderPath) || recompile)
   {
      Con::printf("Generating material skinned %svertex shader..", variantPrefix);
      Platform::createPath(mSkinnedVertexShaderPath);
      shaderFile->openForWrite(mSkinnedVertexShaderPath);
      shaderFile->writeString((const U8*)mTemplate->getVertexShaderOutput(settings));
      shaderFile->close();
   }

   // Mat Shader = Pixel + Vertex
   mShaders.push_back(Graphics::getShader(mVertexShaderPath, mPixelShaderPath, recompile, false));

   // Mat Skinned Shader = Pixel + Vertex (Skinned)
   mSkinnedShaders.push_back(Graphics::getShader(mSkinnedVertexShaderPath, mPixelShaderPath, recompile, false));

   SAFE_DELETE(shaderFile);
}

void MaterialAsset::reloadMaterial()
{
   compileMaterial(true);
   Scene::refresh();
}