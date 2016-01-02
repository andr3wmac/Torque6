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
#include "textureAsset.h"
#include "graphics/core.h"

#include "rendering/rendering.h"
#include "scene/scene.h"

// Script bindings.
#include "textureAsset_Binding.h"

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

TextureAsset* getTextureAsset(const char* id)
{
   AssetPtr<TextureAsset> result;
   StringTableEntry assetId = StringTable->insert(id);
   result.setAssetId(assetId);
   return result;
}

void createTextureAsset(const char* name, const char* textureFile, const char* savePath)
{
   TextureAsset* newAsset = new TextureAsset();
   newAsset->setAssetName(name);
   newAsset->setTextureFile(StringTable->insert(textureFile));

   Platform::createPath(savePath);

   // Save the module file.
   Taml taml;
   taml.write(newAsset, savePath);

   SAFE_DELETE(newAsset);
}

//------------------------------------------------------------------------------

ConsoleType( TextureAssetPtr, TypeTextureAssetPtr, sizeof(AssetPtr<TextureAsset>), ASSET_ID_FIELD_PREFIX )

//-----------------------------------------------------------------------------

ConsoleGetType( TypeTextureAssetPtr )
{
    // Fetch asset Id.
    return (*((AssetPtr<TextureAsset>*)dptr)).getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleSetType( TypeTextureAssetPtr )
{
    // Was a single argument specified?
    if( argc == 1 )
    {
        // Yes, so fetch field value.
        const char* pFieldValue = argv[0];

        // Fetch asset pointer.
        AssetPtr<TextureAsset>* pAssetPtr = dynamic_cast<AssetPtr<TextureAsset>*>((AssetPtrBase*)(dptr));

        // Is the asset pointer the correct type?
        if ( pAssetPtr == NULL )
        {
            // No, so fail.
            Con::warnf( "(TypeTextureAssetPtr) - Failed to set asset Id '%d'.", pFieldValue );
            return;
        }

        // Set asset.
        pAssetPtr->setAssetId( pFieldValue );

        return;
   }

    // Warn.
    Con::warnf( "(TypeTextureAssetPtr) - Cannot set multiple args to a single asset." );
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(TextureAsset);

//------------------------------------------------------------------------------

TextureAsset::TextureAsset()
{
   mTextureLoaded = false;
   mTextureObject = NULL;
   mTextureFile = StringTable->EmptyString;
}

//------------------------------------------------------------------------------

TextureAsset::~TextureAsset()
{
   //
}

//------------------------------------------------------------------------------

void TextureAsset::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    addField("TextureFile", TypeAssetLooseFilePath, Offset(mTextureFile, TextureAsset), "");
}

//------------------------------------------------------------------------------

bool TextureAsset::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

void TextureAsset::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//------------------------------------------------------------------------------

void TextureAsset::onAssetRefresh( void ) 
{
    // Ignore if not yet added to the sim.
    if ( !isProperlyAdded() )
        return;

    // Call parent.
    Parent::onAssetRefresh();
}

//------------------------------------------------------------------------------

void TextureAsset::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to asset.
    TextureAsset* pAsset = static_cast<TextureAsset*>(object);

    // Sanity!
    AssertFatal(pAsset != NULL, "TextureAsset::copyTo() - Object is not the correct type.");
}

void TextureAsset::initializeAsset( void )
{
    // Call parent.
    Parent::initializeAsset();

    // Load Texture
    if (mTextureFile != StringTable->EmptyString)
    {
       mTextureFile = expandAssetFilePath(mTextureFile);
       mTextureObject = TextureManager::loadTexture(mTextureFile, TextureHandle::BitmapTexture, 0);
       mTextureLoaded = (mTextureObject != NULL);
    }
}

bool TextureAsset::isAssetValid() const
{
   return false;
}