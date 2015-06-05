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
#include "meshComponent.h"
#include "graphics/core.h"
#include "3d/rendering/common.h"

// Script bindings.
#include "meshComponent_ScriptBinding.h"

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

namespace Scene
{
   IMPLEMENT_CONOBJECT(MeshComponent);

   MeshComponent::MeshComponent()
   {
      mTransformCount = 0;
   }

   MeshComponent::~MeshComponent()
   {
      for ( S32 n = 0; n < mSubMeshes.size(); ++n )
      {
         SubMesh* subMesh = &mSubMeshes[n];
         subMesh->renderData->deleted = true;
      }
   }

   void MeshComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addProtectedField("MeshAsset", TypeAssetId, Offset(mMeshAssetId, MeshComponent), &setMesh, &defaultProtectedGetFn, "The image asset Id used for the image.");      
   }

   void MeshComponent::setMesh( const char* pImageAssetId )
   {
      // Sanity!
      AssertFatal( pImageAssetId != NULL, "Cannot use a NULL asset Id." );

      // Fetch the asset Id.
      mMeshAssetId = StringTable->insert(pImageAssetId);
      mMeshAsset.setAssetId(mMeshAssetId);

      if ( mMeshAsset.isNull() )
         Con::errorf("[Forward Render Component] Failed to load mesh asset.");
   }

   void MeshComponent::onAddToScene()
   {  
      // Maximum of 32 materials (arbitrary)
      for (U32 n = 0; n < 32; ++n)
      {
         char mat_name[32];
         dSprintf(mat_name, 32, "Material%d", n);
         const char* mat_asset_id = getDataField(StringTable->insert(mat_name), NULL);
         if ( dStrlen(mat_asset_id) == 0 ) 
            continue;

         AssetPtr<MaterialAsset> mat;
         mat.setAssetId(mat_asset_id);
         if ( !mat.isNull() )
            mMaterialAssets.push_back(mat);
      }

      for ( U32 n = 0; n < mMeshAsset->getMeshCount(); ++n )
      {
         SubMesh subMesh;
         subMesh.renderData = Rendering::createRenderData();
         mSubMeshes.push_back(subMesh);
      }

      refresh();
   }

   void MeshComponent::onRemoveFromScene()
   {
      for ( S32 n = 0; n < mSubMeshes.size(); ++n )
      {
         SubMesh* subMesh = &mSubMeshes[n];
         subMesh->renderData->deleted = true;
      }
   }

   void MeshComponent::refresh()
   {
      Parent::refresh();

      // Sanity Checks.
      if ( mOwnerEntity == NULL ) return;
      if ( mMeshAsset.isNull() ) return;
      if ( mMaterialAssets.size() < 1 ) return;

      refreshTransforms();

      for ( S32 n = 0; n < mSubMeshes.size(); ++n )
      {
         SubMesh* subMesh = &mSubMeshes[n];

         // Buffers
         subMesh->renderData->castShadow = true;
         subMesh->renderData->indexBuffer = mMeshAsset->getIndexBuffer(n);
         subMesh->renderData->vertexBuffer = mMeshAsset->getVertexBuffer(n);

         // Uniform/Texture Vectors (these are filled by applyMaterial)
         subMesh->uniforms.clear();
         subMesh->renderData->uniforms.uniforms = &subMesh->uniforms;
         subMesh->textures.clear();
         subMesh->renderData->textures = &subMesh->textures;

         // Apply Material
         U32 matIndex = mMeshAsset->getMaterialIndex(n);
         if ( matIndex > (U32)mMaterialAssets.size() ) matIndex = 0;
         AssetPtr<MaterialAsset> material = mMaterialAssets[matIndex];
         material->applyMaterial(subMesh->renderData, mMeshAsset->isSkinned(), this);

         // Merge in any component-wide uniforms.
         if ( !mUniforms.isEmpty() )
            subMesh->uniforms.merge(*mUniforms.uniforms);
      }
   }

   // By making this a separate function animations don't need to update everything.
   // That includes skipping getNearestLights call.
   void MeshComponent::refreshTransforms()
   {
      for ( S32 n = 0; n < mSubMeshes.size(); ++n )
      {
         SubMesh* subMesh = &mSubMeshes[n];

         // Base Component transform matrix is always slot 0 in the transform table.
         dMemcpy(mTransformTable[0], mTransformMatrix, sizeof(mTransformMatrix));
         if ( mTransformCount < 1 ) mTransformCount = 1;
         subMesh->renderData->transformTable = mTransformTable[0];
         subMesh->renderData->transformCount = mTransformCount;
      }

      // Bounding Box
      mBoundingBox = mMeshAsset->getBoundingBox();
      mBoundingBox.minExtents += mPosition;
      mBoundingBox.maxExtents += mPosition;
   }
}
