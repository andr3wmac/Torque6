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
#include "entityTemplateAsset.h"
#include "graphics/core.h"
#include "../scene/core.h"
#include "3d/rendering/shadows.h"

// Script bindings.
#include "entityTemplateAsset_ScriptBinding.h"

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

ConsoleType( EntityTemplateAssetPtr, TypeEntityTemplateAssetPtr, sizeof(AssetPtr<EntityTemplateAsset>), ASSET_ID_FIELD_PREFIX )

//-----------------------------------------------------------------------------

ConsoleGetType( TypeEntityTemplateAssetPtr )
{
    // Fetch asset Id.
    return (*((AssetPtr<EntityTemplateAsset>*)dptr)).getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleSetType( TypeEntityTemplateAssetPtr )
{
    // Was a single argument specified?
    if( argc == 1 )
    {
        // Yes, so fetch field value.
        const char* pFieldValue = argv[0];

        // Fetch asset pointer.
        AssetPtr<EntityTemplateAsset>* pAssetPtr = dynamic_cast<AssetPtr<EntityTemplateAsset>*>((AssetPtrBase*)(dptr));

        // Is the asset pointer the correct type?
        if ( pAssetPtr == NULL )
        {
            // No, so fail.
            Con::warnf( "(TypeEntityTemplateAssetPtr) - Failed to set asset Id '%d'.", pFieldValue );
            return;
        }

        // Set asset.
        pAssetPtr->setAssetId( pFieldValue );

        return;
   }

    // Warn.
    Con::warnf( "(TypeEntityTemplateAssetPtr) - Cannot set multiple args to a single asset." );
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(EntityTemplateAsset);

//------------------------------------------------------------------------------

EntityTemplateAsset::EntityTemplateAsset()
{
   mTemplateFile = StringTable->insert("");
   //
   Con::printf("Entity Template Asset Created!");
}

//------------------------------------------------------------------------------

EntityTemplateAsset::~EntityTemplateAsset()
{
   //
}

void EntityTemplateAsset::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    addField("TemplateFile", TypeAssetLooseFilePath, Offset(mTemplateFile, EntityTemplateAsset), "");
}

Scene::EntityTemplate* EntityTemplateAsset::getInstance()
{
   Taml taml;
   Scene::EntityTemplate* result = taml.read<Scene::EntityTemplate>( expandAssetFilePath(mTemplateFile) );
   return result;
}
