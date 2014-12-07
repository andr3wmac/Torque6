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
   {
      mTransformCount = 0;
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
      // Maximum of 16 materials (arbitrary)
      for (U32 n = 0; n < 16; ++n)
      {
         char mat_name[32];
         dSprintf(mat_name, 32, "Material%d", n);
         const char* mat_asset_id = getDataField(StringTable->insert(mat_name), NULL);
         if ( dStrlen(mat_asset_id) == 0 ) 
            continue;

         AssetPtr<ForwardMaterialAsset> mat;
         mat.setAssetId(mat_asset_id);
         if ( !mat.isNull() )
            mMaterialAssets.push_back(mat);
      }
      
      for ( U32 n = 0; n < mMeshAsset->getMeshCount(); ++n )
      {
         Scene::ForwardRenderData data;
         Scene::forwardRenderList.push_back(data);
         mRenderDataList.push_back(&Scene::forwardRenderList.back());
      }

      refresh();
   }

   void MeshComponent::refresh()
   {
      Parent::refresh();

      // Sanity Checks.
      if ( mOwnerEntity == NULL ) return;
      
      if ( mMeshAsset.isNull() ) return;
      if ( mMaterialAssets.size() < 1 ) return;

      for ( S32 n = 0; n < mRenderDataList.size(); ++n )
      {
         Scene::ForwardRenderData* renderData = mRenderDataList[n];

         // Material Data
         U32 matIndex = n < mMaterialAssets.size() ? n : 0;
         AssetPtr<ForwardMaterialAsset> material = mMaterialAssets[matIndex];

         renderData->shader = material->getShader()->getProgram();
         renderData->textures.clear();
         Vector<bgfx::TextureHandle> textureHandles = material->getTextureHandles();
         for(S32 t = 0; t < textureHandles.size(); ++t)
         {
            Scene::TexureData texture;
            texture.uniform = Graphics::Shader::getTextureUniform(t);
            texture.handle = textureHandles[t];
            renderData->textures.push_back(texture);
         }

         // Base Component transform matrix is always slot 0 in the transform table.
         dMemcpy(mTransformTable[0], mTransformMatrix, sizeof(mTransformMatrix));
         if ( mTransformCount < 1 ) mTransformCount = 1;
         renderData->transformTable = mTransformTable[0];
         renderData->transformCount = mTransformCount;

         // Lighting Uniforms
         renderData->uniforms.clear();

         Vector<LightData*> nearestLights = getNearestLights(mPosition + mOwnerEntity->mPosition);
         for( U32 t = 0; t < nearestLights.size(); ++t )
         {
            mLightPosRadius[t][0] = nearestLights[t]->position.x;
            mLightPosRadius[t][1] = nearestLights[t]->position.y;
            mLightPosRadius[t][2] = nearestLights[t]->position.z;
            mLightPosRadius[t][3] = nearestLights[t]->radius;

            mLightColorAttn[t][0] = nearestLights[t]->color[0];
            mLightColorAttn[t][1] = nearestLights[t]->color[1];
            mLightColorAttn[t][2] = nearestLights[t]->color[2];
            mLightColorAttn[t][3] = nearestLights[t]->attenuation;

            if ( nearestLights[t]->color[0] == 1.0f && nearestLights[t]->color[1] == 1.0f && nearestLights[t]->color[2] == 1.0f)
            {
               Con::printf("Found white light as nearest in refresh..");
            }

            //Con::printf("Light Color: %f %f %f", nearestLights[t]->color[0], nearestLights[t]->color[1], nearestLights[t]->color[2]);
         }

         Scene::UniformData lightPosRadius;
         lightPosRadius.data = mLightPosRadius;
         lightPosRadius.uniform = Graphics::Shader::getUniform("lightPosRadius");
         lightPosRadius.count = nearestLights.size();
         renderData->uniforms.push_back(lightPosRadius);

         // Lighting Uniforms
         Scene::UniformData lightColorAttn;
         lightColorAttn.data = mLightColorAttn;
         lightColorAttn.uniform = Graphics::Shader::getUniform("lightColorAttn");
         lightColorAttn.count = nearestLights.size();
         renderData->uniforms.push_back(lightColorAttn);

         // Update render data.
         renderData->indexBuffer = mMeshAsset->getIndexBuffer(n);
         renderData->vertexBuffer = mMeshAsset->getVertexBuffer(n);
      }
   }
}