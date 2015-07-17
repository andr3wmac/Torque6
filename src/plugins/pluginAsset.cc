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
#include "pluginAsset.h"
#include "graphics/core.h"

// Script bindings.
#include "pluginAsset_ScriptBinding.h"

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

ConsoleType( PluginAssetPtr, TypePluginAssetPtr, sizeof(AssetPtr<PluginAsset>), ASSET_ID_FIELD_PREFIX )

//-----------------------------------------------------------------------------

ConsoleGetType( TypePluginAssetPtr )
{
    // Fetch asset Id.
    return (*((AssetPtr<PluginAsset>*)dptr)).getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleSetType( TypePluginAssetPtr )
{
    // Was a single argument specified?
    if( argc == 1 )
    {
        // Yes, so fetch field value.
        const char* pFieldValue = argv[0];

        // Fetch asset pointer.
        AssetPtr<PluginAsset>* pAssetPtr = dynamic_cast<AssetPtr<PluginAsset>*>((AssetPtrBase*)(dptr));

        // Is the asset pointer the correct type?
        if ( pAssetPtr == NULL )
        {
            // No, so fail.
            Con::warnf( "(TypePluginAssetPtr) - Failed to set asset Id '%d'.", pFieldValue );
            return;
        }

        // Set asset.
        pAssetPtr->setAssetId( pFieldValue );

        return;
   }

    // Warn.
    Con::warnf( "(TypePluginAssetPtr) - Cannot set multiple args to a single asset." );
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(PluginAsset);

//------------------------------------------------------------------------------

PluginAsset::PluginAsset()
{
   mPluginPath = StringTable->EmptyString;
}

//------------------------------------------------------------------------------

PluginAsset::~PluginAsset()
{

}

//------------------------------------------------------------------------------

void PluginAsset::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    addProtectedField("pluginFile", TypeAssetLooseFilePath, Offset(mPluginPath, PluginAsset), &setPluginPath, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
}

//------------------------------------------------------------------------------

bool PluginAsset::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

void PluginAsset::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//------------------------------------------------------------------------------

void PluginAsset::onAssetRefresh( void ) 
{
    // Ignore if not yet added to the sim.
    if ( !isProperlyAdded() )
        return;

    // Call parent.
    Parent::onAssetRefresh();
}

//------------------------------------------------------------------------------

void PluginAsset::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to asset.
    PluginAsset* pAsset = static_cast<PluginAsset*>(object);

    // Sanity!
    AssertFatal(pAsset != NULL, "PluginAsset::copyTo() - Object is not the correct type.");
}

void PluginAsset::initializeAsset( void )
{
    // Call parent.
    Parent::initializeAsset();

    mPluginPath = expandAssetFilePath( mPluginPath );

    //compileAndLoad();
}

bool PluginAsset::isAssetValid() const
{
   return false;
}

void PluginAsset::setPluginPath( const char* pPluginPath )
{
   // Sanity!
   AssertFatal( pPluginPath != NULL, "Cannot use a NULL image file." );

   // Fetch image file.
   pPluginPath = StringTable->insert( pPluginPath );

   // Ignore no change.
   if ( pPluginPath == mPluginPath )
      return;

   // Update.
   mPluginPath = getOwned() ? expandAssetFilePath( mPluginPath ) : StringTable->insert( pPluginPath );

   // Refresh the asset.
   refreshAsset();
}