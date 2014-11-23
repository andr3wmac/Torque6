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
#include "shaderAsset.h"
#include "graphics/vertexLayouts.h"

// Script bindings.
#include "ShaderAsset_ScriptBinding.h"

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
   mPixelShader.idx = bgfx::invalidHandle;
   mVertexShader.idx = bgfx::invalidHandle;
   mProgram.idx = bgfx::invalidHandle;
}

//------------------------------------------------------------------------------

ShaderAsset::~ShaderAsset()
{
   if ( mPixelShader.idx != bgfx::invalidHandle )
      bgfx::destroyShader(mPixelShader);

   if ( mVertexShader.idx != bgfx::invalidHandle )
      bgfx::destroyShader(mVertexShader);

   if ( mProgram.idx != bgfx::invalidHandle )
      bgfx::destroyProgram(mProgram);
}

//------------------------------------------------------------------------------

void ShaderAsset::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    addField("vertexShaderFile", TypeString, Offset(mVertexShaderPath, ShaderAsset));
    addField("pixelShaderFile", TypeString, Offset(mPixelShaderPath, ShaderAsset));
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

    // Time to load image.
    Con::printf("[ShaderAsset] Time to load the asset!");
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

    compileAndLoad();
}

bool ShaderAsset::isAssetValid() const
{
   return false;
}

void ShaderAsset::compileAndLoad()
{
   if ( mVertexShaderPath == StringTable->EmptyString || mPixelShaderPath == StringTable->EmptyString )
      return;

   // DirectX or OpenGL?
   bool is_dx9 = (bgfx::getRendererType() == bgfx::RendererType::Direct3D9);

   // Vertex Shader
   char vertex_compiled_path[256];
   dSprintf(vertex_compiled_path, 256, "%s.bin", mVertexShaderPath); 
   if ( is_dx9 )
      bgfx::compileShader(0, mVertexShaderPath, vertex_compiled_path, "v", "windows", "vs_3_0", NULL, "shaders/includes/", "shaders/includes/varying.def.sc");
   else
      bgfx::compileShader(0, mVertexShaderPath, vertex_compiled_path, "v", "linux", NULL, NULL, "shaders/includes/", "shaders/includes/varying.def.sc");

   if ( mVertexShaderFile.readMemory(vertex_compiled_path) )
   {
      const bgfx::Memory* mem = bgfx::makeRef(mVertexShaderFile.getBuffer(), mVertexShaderFile.getBufferSize());
      mVertexShader = bgfx::createShader(mem);
   }

   // Pixel Shader
   char pixel_compiled_path[256];
   dSprintf(pixel_compiled_path, 256, "%s.bin", mPixelShaderPath); 
   if ( is_dx9 )
      bgfx::compileShader(0, mPixelShaderPath, pixel_compiled_path, "f", "windows", "ps_3_0", NULL, "shaders/includes/", "shaders/includes/varying.def.sc");
   else
      bgfx::compileShader(0, mPixelShaderPath, pixel_compiled_path, "f", "linux", NULL, NULL, "shaders/includes/", "shaders/includes/varying.def.sc");

   if ( mPixelShaderFile.readMemory(pixel_compiled_path) )
   {
      const bgfx::Memory* mem = bgfx::makeRef(mPixelShaderFile.getBuffer(), mPixelShaderFile.getBufferSize());
      mPixelShader = bgfx::createShader(mem);
   }

   // Load Program
   if ( mPixelShader.idx != bgfx::invalidHandle && mVertexShader.idx != bgfx::invalidHandle )
   {
      mProgram = bgfx::createProgram(mVertexShader, mPixelShader, true);
   }
}