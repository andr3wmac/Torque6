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
#include "shaderAsset.h"
#include "graphics/utilities.h"

// Script bindings.
#include "ShaderAsset_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

// bgfx/bx
#include <bgfx.h>
#include <../tools/shaderc/shaderc.h>
#include <bx/fpumath.h>

// Assimp - Asset Import Library
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

//------------------------------------------------------------------------------

ConsoleType( ShaderAssetPtr, TypeShaderAssetPtr, sizeof(AssetPtr<ShaderAsset>), ASSET_ID_FIELD_PREFIX )

//-----------------------------------------------------------------------------

ConsoleGetType( TypeShaderAssetPtr )
{
    // Fetch asset Id.
    return (*((AssetPtr<ShaderAsset>*)dptr)).getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleSetType( TypeShaderAssetPtr )
{
    // Was a single argument specified?
    if( argc == 1 )
    {
        // Yes, so fetch field value.
        const char* pFieldValue = argv[0];

        // Fetch asset pointer.
        AssetPtr<ShaderAsset>* pAssetPtr = dynamic_cast<AssetPtr<ShaderAsset>*>((AssetPtrBase*)(dptr));

        // Is the asset pointer the correct type?
        if ( pAssetPtr == NULL )
        {
            // No, so fail.
            Con::warnf( "(TypeShaderAssetPtr) - Failed to set asset Id '%d'.", pFieldValue );
            return;
        }

        // Set asset.
        pAssetPtr->setAssetId( pFieldValue );

        return;
   }

    // Warn.
    Con::warnf( "(TypeShaderAssetPtr) - Cannot set multiple args to a single asset." );
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(ShaderAsset);

//------------------------------------------------------------------------------

ShaderAsset::ShaderAsset()
{
   mVertexShaderPath = StringTable->EmptyString;
   mPixelShaderPath = StringTable->EmptyString;
}

//------------------------------------------------------------------------------

ShaderAsset::~ShaderAsset()
{
   //
}

//------------------------------------------------------------------------------

void ShaderAsset::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    addProtectedField("vertexShaderFile", TypeAssetLooseFilePath, Offset(mVertexShaderPath, ShaderAsset), &setVertexShaderPath, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
    addProtectedField("pixelShaderFile", TypeAssetLooseFilePath, Offset(mPixelShaderPath, ShaderAsset), &setPixelShaderPath, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
}

//------------------------------------------------------------------------------

bool ShaderAsset::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

void ShaderAsset::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//------------------------------------------------------------------------------

void ShaderAsset::onAssetRefresh( void ) 
{
    // Ignore if not yet added to the sim.
    if ( !isProperlyAdded() )
        return;

    // Call parent.
    Parent::onAssetRefresh();
}

//------------------------------------------------------------------------------

void ShaderAsset::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to asset.
    ShaderAsset* pAsset = static_cast<ShaderAsset*>(object);

    // Sanity!
    AssertFatal(pAsset != NULL, "ShaderAsset::copyTo() - Object is not the correct type.");
}

void ShaderAsset::initializeAsset( void )
{
    // Call parent.
    Parent::initializeAsset();

    mVertexShaderPath = expandAssetFilePath( mVertexShaderPath );
    mPixelShaderPath = expandAssetFilePath( mPixelShaderPath );

    compileAndLoad();
}

bool ShaderAsset::isAssetValid() const
{
   return false;
}

void ShaderAsset::setPixelShaderPath( const char* pShaderPath )
{
   // Sanity!
   AssertFatal( pShaderPath != NULL, "Cannot use a NULL image file." );

   // Fetch image file.
   pShaderPath = StringTable->insert( pShaderPath );

   // Ignore no change.
   if ( pShaderPath == mPixelShaderPath )
      return;

   // Update.
   mPixelShaderPath = getOwned() ? expandAssetFilePath( pShaderPath ) : StringTable->insert( pShaderPath );

   // Refresh the asset.
   refreshAsset();
}

void ShaderAsset::setVertexShaderPath( const char* pShaderPath )
{
   // Sanity!
   AssertFatal( pShaderPath != NULL, "Cannot use a NULL image file." );

   // Fetch image file.
   pShaderPath = StringTable->insert( pShaderPath );

   // Ignore no change.
   if ( pShaderPath == mVertexShaderPath )
      return;

   // Update.
   mVertexShaderPath = getOwned() ? expandAssetFilePath( pShaderPath ) : StringTable->insert( pShaderPath );

   // Refresh the asset.
   refreshAsset();
}

void ShaderAsset::compileAndLoad()
{
   if ( mVertexShaderPath == StringTable->EmptyString || mPixelShaderPath == StringTable->EmptyString )
      return;

   mShader.load(mVertexShaderPath, mPixelShaderPath);
}