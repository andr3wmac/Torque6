//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
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
#include "meshAsset.h"
#include "graphics/vertexLayouts.h"

// Script bindings.
#include "meshAsset_ScriptBinding.h"

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

ConsoleType( MeshAssetPtr, TypeMeshAssetPtr, sizeof(AssetPtr<MeshAsset>), ASSET_ID_FIELD_PREFIX )

//-----------------------------------------------------------------------------

ConsoleGetType( TypeMeshAssetPtr )
{
    // Fetch asset Id.
    return (*((AssetPtr<MeshAsset>*)dptr)).getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleSetType( TypeMeshAssetPtr )
{
    // Was a single argument specified?
    if( argc == 1 )
    {
        // Yes, so fetch field value.
        const char* pFieldValue = argv[0];

        // Fetch asset pointer.
        AssetPtr<MeshAsset>* pAssetPtr = dynamic_cast<AssetPtr<MeshAsset>*>((AssetPtrBase*)(dptr));

        // Is the asset pointer the correct type?
        if ( pAssetPtr == NULL )
        {
            // No, so fail.
            Con::warnf( "(TypeMeshAssetPtr) - Failed to set asset Id '%d'.", pFieldValue );
            return;
        }

        // Set asset.
        pAssetPtr->setAssetId( pFieldValue );

        return;
   }

    // Warn.
    Con::warnf( "(TypeMeshAssetPtr) - Cannot set multiple args to a single asset." );
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(MeshAsset);

//------------------------------------------------------------------------------

MeshAsset::MeshAsset() : 
   mMeshFile(StringTable->EmptyString)
{
    mVertexBuffer.idx = bgfx::invalidHandle;
    mIndexBuffer.idx = bgfx::invalidHandle;
}

//------------------------------------------------------------------------------

MeshAsset::~MeshAsset()
{
   if ( mVertexBuffer.idx != bgfx::invalidHandle )
      bgfx::destroyVertexBuffer(mVertexBuffer);

   if ( mIndexBuffer.idx != bgfx::invalidHandle )
      bgfx::destroyIndexBuffer(mIndexBuffer);
}

//------------------------------------------------------------------------------

void MeshAsset::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    addProtectedField("MeshFile", TypeAssetLooseFilePath, Offset(mMeshFile, MeshAsset), &setMeshFile, &getMeshFile, &defaultProtectedWriteFn, "");
}

//------------------------------------------------------------------------------

bool MeshAsset::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

void MeshAsset::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//------------------------------------------------------------------------------

void MeshAsset::onAssetRefresh( void ) 
{
    // Ignore if not yet added to the sim.
    if ( !isProperlyAdded() )
        return;

    // Call parent.
    Parent::onAssetRefresh();

    // Time to load image.
    Con::printf("[MESHASSET] Time to load the asset!");
}

//------------------------------------------------------------------------------

void MeshAsset::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to asset.
    MeshAsset* pAsset = static_cast<MeshAsset*>(object);

    // Sanity!
    AssertFatal(pAsset != NULL, "MeshAsset::copyTo() - Object is not the correct type.");
}

void MeshAsset::initializeAsset( void )
{
    // Call parent.
    Parent::initializeAsset();

    // Currently there is no specific initialization required.
}

bool MeshAsset::isAssetValid() const
{
   return false;
}

void MeshAsset::setMeshFile( const char* pMeshFile )
{
   // Sanity!
   AssertFatal( pMeshFile != NULL, "Cannot use a NULL image file." );

   // Fetch image file.
   pMeshFile = StringTable->insert( pMeshFile );

   // Ignore no change.
   if ( pMeshFile == mMeshFile )
      return;

   Con::printf("[MESHASSET] Trying to load mesh asset: %s", pMeshFile);

   // Update.
   mMeshFile = getOwned() ? expandAssetFilePath( pMeshFile ) : StringTable->insert( pMeshFile );

   loadMesh();

   // Refresh the asset.
   refreshAsset();
}

void MeshAsset::loadMesh()
{
   // Use Assimp To Load Dwarf.x
   const struct aiScene* mScene = aiImportFile(mMeshFile, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
   if ( !mScene ) return;

   //Con::printf("[ASSIMP] Mesh Count: %d", mScene->mNumMeshes);
   //Con::printf("[ASSIMP] Material Count: %d", mScene->mNumMaterials);

   // Dwarf Mesh
   aiMesh* mMeshData = mScene->mMeshes[1];
         
   // Verts/Normals/Tangent/UVs
   for ( U32 n = 0; n < mMeshData->mNumVertices; ++n)
   {
      Graphics::PosTexcoordVertex vert;

      // Verts
      aiVector3D pt = mMeshData->mVertices[n];
      vert.m_x = pt.x;
      vert.m_y = pt.y;
      vert.m_z = pt.z;

      // UVs
      if ( mMeshData->HasTextureCoords(0) )
      {
         vert.m_u = mMeshData->mTextureCoords[0][n].x;
         vert.m_v = mMeshData->mTextureCoords[0][n].y;
      }

      mRawVerts.push_back(vert);
   }
         
   // Faces
   for ( U32 n = 0; n < mMeshData->mNumFaces; ++n)
   {
      const struct aiFace* face = &mMeshData->mFaces[n];
      if ( face->mNumIndices == 3 )
      {
         // Con::printf("[ASSIMP] Face: %d %d %d", face->mIndices[0], face->mIndices[1], face->mIndices[2]);
         mRawIndicies.push_back(face->mIndices[0]);
         mRawIndicies.push_back(face->mIndices[1]);
         mRawIndicies.push_back(face->mIndices[2]);
      } else {
         Con::printf("[ASSIMP] Non-Triangle Face Found.");
      }
   }

   // Clean up.
   aiReleaseImport(mScene);

   // Load the verts and indices into bgfx buffers
	mVertexBuffer = bgfx::createVertexBuffer(
		   bgfx::makeRef(&mRawVerts[0], mRawVerts.size() * sizeof(Graphics::PosTexcoordVertex) ), 
         Graphics::PosTexcoordVertex::ms_decl
		);

	mIndexBuffer = bgfx::createIndexBuffer(
         bgfx::makeRef(&mRawIndicies[0], mRawIndicies.size() * sizeof(U16) )
		);

   // Dwarf Texture
   mTexture = TextureHandle("models/dwarf.png", TextureHandle::TextureHandleType::BitmapTexture);

   Con::printf("Mesh successfully loaded.");
}