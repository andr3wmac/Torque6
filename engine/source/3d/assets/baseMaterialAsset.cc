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
#include "baseMaterialAsset.h"
#include "graphics/utilities.h"
#include "3d/rendering/common.h"

// Script bindings.
#include "baseMaterialAsset_ScriptBinding.h"

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

//------------------------------------------------------------------------------

ConsoleType( BaseMaterialAssetPtr, TypeBaseMaterialAssetPtr, sizeof(AssetPtr<BaseMaterialAsset>), ASSET_ID_FIELD_PREFIX )

//-----------------------------------------------------------------------------

ConsoleGetType( TypeBaseMaterialAssetPtr )
{
    // Fetch asset Id.
    return (*((AssetPtr<BaseMaterialAsset>*)dptr)).getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleSetType( TypeBaseMaterialAssetPtr )
{
    // Was a single argument specified?
    if( argc == 1 )
    {
        // Yes, so fetch field value.
        const char* pFieldValue = argv[0];

        // Fetch asset pointer.
        AssetPtr<BaseMaterialAsset>* pAssetPtr = dynamic_cast<AssetPtr<BaseMaterialAsset>*>((AssetPtrBase*)(dptr));

        // Is the asset pointer the correct type?
        if ( pAssetPtr == NULL )
        {
            // No, so fail.
            Con::warnf( "(TypeBaseMaterialAssetPtr) - Failed to set asset Id '%d'.", pFieldValue );
            return;
        }

        // Set asset.
        pAssetPtr->setAssetId( pFieldValue );

        return;
   }

    // Warn.
    Con::warnf( "(TypeBaseMaterialAssetPtr) - Cannot set multiple args to a single asset." );
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(BaseMaterialAsset);

//------------------------------------------------------------------------------

BaseMaterialAsset::BaseMaterialAsset()
{
   //
}

//------------------------------------------------------------------------------

BaseMaterialAsset::~BaseMaterialAsset()
{
   //
}

//------------------------------------------------------------------------------

void BaseMaterialAsset::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    addProtectedField("ShaderAsset", TypeAssetId, Offset(mShaderAssetId, BaseMaterialAsset), &setShader, &defaultProtectedGetFn, "The image asset Id used for the image.");
}

//------------------------------------------------------------------------------

bool BaseMaterialAsset::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

void BaseMaterialAsset::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//------------------------------------------------------------------------------

void BaseMaterialAsset::onAssetRefresh( void ) 
{
    // Ignore if not yet added to the sim.
    if ( !isProperlyAdded() )
        return;

    // Call parent.
    Parent::onAssetRefresh();
}

//------------------------------------------------------------------------------

void BaseMaterialAsset::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to asset.
    BaseMaterialAsset* pAsset = static_cast<BaseMaterialAsset*>(object);

    // Sanity!
    AssertFatal(pAsset != NULL, "BaseMaterialAsset::copyTo() - Object is not the correct type.");
}

void BaseMaterialAsset::initializeAsset( void )
{
    // Call parent.
    Parent::initializeAsset();

    loadTextures();
}

bool BaseMaterialAsset::isAssetValid() const
{
   return false;
}

void BaseMaterialAsset::setShader( const char* pShaderAssetId )
{
   // Sanity!
   AssertFatal( pShaderAssetId != NULL, "Cannot use a NULL asset Id." );

   // Fetch the asset Id.
   mShaderAssetId = StringTable->insert(pShaderAssetId);
   mShaderAsset.setAssetId(mShaderAssetId);

   if ( mShaderAsset.isNull() )
      Con::errorf("[Base Render Component] Failed to load mesh asset.");
}

void BaseMaterialAsset::loadTextures()
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
         Con::errorf("BaseMaterialAsset : Failed to load texture.");
   }
}

void BaseMaterialAsset::applyMaterial(Rendering::RenderData* renderData, Scene::BaseComponent* component)
{
   renderData->shader = mShaderAsset->getProgram();

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
}
