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
#include "meshComponent.h"
#include "graphics/vertexLayouts.h"
#include "3d/scene/rendering/forwardRendering.h"
#include "3d/assets/shaderAsset.h"

// Script bindings.
#include "MeshComponent_ScriptBinding.h"

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
      : mRenderData (NULL)
   {
      Con::printf("Forward Render Component Created!");
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
      // Temporarily Load One Material
      const char* matAssetId = getDataField(StringTable->insert("Material0"), NULL);
      mMaterialAsset.setAssetId(matAssetId);
      
      Scene::ForwardRenderData data;
      Scene::forwardRenderList.push_back(data);
      mRenderData = &Scene::forwardRenderList.back();

      refresh();
   }

   void MeshComponent::refresh()
   {
      Parent::refresh();

      // Sanity Checks.
      if ( mOwnerEntity == NULL ) return;
      if ( mRenderData == NULL ) return;
      if ( mMeshAsset.isNull() ) return;
      if ( mMaterialAsset.isNull() ) return;

      // Material Data
      mRenderData->shader = mMaterialAsset->getShader()->getProgram();
      mRenderData->textures.clear();
      mRenderData->textureUniforms.clear();
      Vector<bgfx::TextureHandle> textureHandles = mMaterialAsset->getTextureHandles();
      for(S32 n = 0; n < textureHandles.size(); ++n)
      {
         mRenderData->textures.push_back(textureHandles[n]);
         mRenderData->textureUniforms.push_back(Graphics::Shader::getTextureUniform(n));
      }

      // Bone Transforms
      /*for(U32 i = 0; i < 49; ++i)
      {
         mRenderData->boneTransforms[i][0] = mMeshAsset->mBoneTransforms[i][0];
         mRenderData->boneTransforms[i][1] = mMeshAsset->mBoneTransforms[i][4];
         mRenderData->boneTransforms[i][2] = mMeshAsset->mBoneTransforms[i][8];
         mRenderData->boneTransforms[i][3] = mMeshAsset->mBoneTransforms[i][12];

         mRenderData->boneTransforms[i][4] = mMeshAsset->mBoneTransforms[i][1];
         mRenderData->boneTransforms[i][5] = mMeshAsset->mBoneTransforms[i][5];
         mRenderData->boneTransforms[i][6] = mMeshAsset->mBoneTransforms[i][9];
         mRenderData->boneTransforms[i][7] = mMeshAsset->mBoneTransforms[i][13];

         mRenderData->boneTransforms[i][8] = mMeshAsset->mBoneTransforms[i][2];
         mRenderData->boneTransforms[i][9] = mMeshAsset->mBoneTransforms[i][6];
         mRenderData->boneTransforms[i][10] = mMeshAsset->mBoneTransforms[i][10];
         mRenderData->boneTransforms[i][11] = mMeshAsset->mBoneTransforms[i][14];

         mRenderData->boneTransforms[i][12] = mMeshAsset->mBoneTransforms[i][3];
         mRenderData->boneTransforms[i][13] = mMeshAsset->mBoneTransforms[i][7];
         mRenderData->boneTransforms[i][14] = mMeshAsset->mBoneTransforms[i][11];
         mRenderData->boneTransforms[i][15] = mMeshAsset->mBoneTransforms[i][15];
      }*/
      dMemcpy(mRenderData->boneTransforms, mMeshAsset->mBoneTransforms, sizeof(mRenderData->boneTransforms));

      // Update render data.
      mRenderData->indexBuffer = mMeshAsset->getIndexBuffer();
      mRenderData->vertexBuffer = mMeshAsset->getVertexBuffer();
      dMemcpy(mRenderData->transformMatrix, mTransformMatrix, sizeof(mTransformMatrix));
   }
}