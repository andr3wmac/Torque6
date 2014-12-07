//-----------------------------------------------------------------------------
// Copyright (c) 2014 Andrew Mac
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
#include "forwardMaterialAsset.h"
#include "graphics/utilities.h"

// Script bindings.
#include "forwardMaterialAsset_ScriptBinding.h"

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

ConsoleType( ForwardMaterialAssetPtr, TypeForwardMaterialAssetPtr, sizeof(AssetPtr<ForwardMaterialAsset>), ASSET_ID_FIELD_PREFIX )

//-----------------------------------------------------------------------------

ConsoleGetType( TypeForwardMaterialAssetPtr )
{
    // Fetch asset Id.
    return (*((AssetPtr<ForwardMaterialAsset>*)dptr)).getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleSetType( TypeForwardMaterialAssetPtr )
{
    // Was a single argument specified?
    if( argc == 1 )
    {
        // Yes, so fetch field value.
        const char* pFieldValue = argv[0];

        // Fetch asset pointer.
        AssetPtr<ForwardMaterialAsset>* pAssetPtr = dynamic_cast<AssetPtr<ForwardMaterialAsset>*>((AssetPtrBase*)(dptr));

        // Is the asset pointer the correct type?
        if ( pAssetPtr == NULL )
        {
            // No, so fail.
            Con::warnf( "(TypeForwardMaterialAssetPtr) - Failed to set asset Id '%d'.", pFieldValue );
            return;
        }

        // Set asset.
        pAssetPtr->setAssetId( pFieldValue );

        return;
   }

    // Warn.
    Con::warnf( "(TypeForwardMaterialAssetPtr) - Cannot set multiple args to a single asset." );
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(ForwardMaterialAsset);

//------------------------------------------------------------------------------

ForwardMaterialAsset::ForwardMaterialAsset()
{
   //
}

//------------------------------------------------------------------------------

ForwardMaterialAsset::~ForwardMaterialAsset()
{
   //
}

//------------------------------------------------------------------------------

void ForwardMaterialAsset::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    addProtectedField("ShaderAsset", TypeAssetId, Offset(mShaderAssetId, ForwardMaterialAsset), &setShader, &defaultProtectedGetFn, "The image asset Id used for the image.");
}

//------------------------------------------------------------------------------

bool ForwardMaterialAsset::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

void ForwardMaterialAsset::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//------------------------------------------------------------------------------

void ForwardMaterialAsset::onAssetRefresh( void ) 
{
    // Ignore if not yet added to the sim.
    if ( !isProperlyAdded() )
        return;

    // Call parent.
    Parent::onAssetRefresh();

    // Time to load image.
    Con::printf("[ForwardMaterialAsset] Time to load the asset!");
}

//------------------------------------------------------------------------------

void ForwardMaterialAsset::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to asset.
    ForwardMaterialAsset* pAsset = static_cast<ForwardMaterialAsset*>(object);

    // Sanity!
    AssertFatal(pAsset != NULL, "ForwardMaterialAsset::copyTo() - Object is not the correct type.");
}

void ForwardMaterialAsset::initializeAsset( void )
{
    // Call parent.
    Parent::initializeAsset();

    loadTextures();
}

bool ForwardMaterialAsset::isAssetValid() const
{
   return false;
}

void ForwardMaterialAsset::setShader( const char* pShaderAssetId )
{
   // Sanity!
   AssertFatal( pShaderAssetId != NULL, "Cannot use a NULL asset Id." );

   // Fetch the asset Id.
   mShaderAssetId = StringTable->insert(pShaderAssetId);
   mShaderAsset.setAssetId(mShaderAssetId);

   if ( mShaderAsset.isNull() )
      Con::errorf("[Forward Render Component] Failed to load mesh asset.");
}

void ForwardMaterialAsset::loadTextures()
{
   // Maximum of 16 textures.
   for (U32 n = 0; n < 16; ++n)
   {
      char texture_name[32];
      dSprintf(texture_name, 32, "Texture%d", n);
      const char* texture_path = expandAssetFilePath(getDataField(StringTable->insert(texture_name), NULL));
      if ( dStrlen(texture_path) == 0 ) 
         continue;

      // The texture system automatically caches these so they only load once.
      TextureHandle newTexture(texture_path, TextureHandle::TextureHandleType::BitmapTexture);
      mTextureHandles.push_back( ((TextureObject*)newTexture)->getBGFXTexture() );
   }
}
