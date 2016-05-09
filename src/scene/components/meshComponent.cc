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
#include "scene/object.h"
#include "rendering/rendering.h"
#include "physics/physics.h"
#include "physics/physicsThread.h"

// Script bindings.
#include "meshComponent_Binding.h"

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

namespace Scene
{
   IMPLEMENT_CONOBJECT(MeshComponent);

   MeshComponent::MeshComponent()
   {
      mTransformCount = 0;
   }

   MeshComponent::~MeshComponent()
   {
      onRemoveFromScene();
   }

   void MeshComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addGroup("MeshComponent");
         addProtectedField("MeshAsset", TypeAssetId, Offset(mMeshAssetId, MeshComponent), &setMesh, &defaultProtectedGetFn, "The asset Id used for the mesh.", AbstractClassRep::MeshAssetField);
      endGroup("MeshComponent");
   }

   void MeshComponent::setMesh( const char* pImageAssetId )
   {
      // Sanity!
      AssertFatal( pImageAssetId != NULL, "Cannot use a NULL asset Id." );

      // Fetch the asset Id.
      mMeshAssetId = StringTable->insert(pImageAssetId);
      mMeshAsset.setAssetId(mMeshAssetId);

      if ( mMeshAsset.isNull() )
         Con::errorf("[MeshComponent] Failed to load mesh asset.");
      else {
         for (U32 n = 0; n < mMeshAsset->getMaterialCount(); ++n)
         {
            char mat_name[32];
            dSprintf(mat_name, 32, "MaterialAsset%d", n);
            setDataField(StringTable->insert(mat_name), NULL, "Empty");
         }
         for (U32 n = 0; n < mMeshAsset->getMeshCount(); ++n)
         {
            char mesh_name[32];
            dSprintf(mesh_name, 32, "SubMesh%d", n);
            setDataField(StringTable->insert(mesh_name), NULL, "true");
         }
      }

      // Re-add to scene if we're already in it and the mesh has been changed.
      if (mAddedToScene)
      {
         onRemoveFromScene();
         onAddToScene();
      }
   }

   void MeshComponent::onAddToScene()
   {  
      mAddedToScene = true;
      if (mMeshAsset.isNull()) return;

      bool renderMesh = false;
      if ( mOwnerObject )
         renderMesh = !mOwnerObject->mGhosted || mOwnerObject->isClientObject();

      if ( renderMesh )
      {
         mSubMeshes.clear();
         for ( U32 n = 0; n < mMeshAsset->getMeshCount(); ++n )
         {
            SubMesh subMesh;
            subMesh.name = mMeshAsset->getNodeName(n);
            subMesh.isCollisionMesh = false;

            char meshName[256];
            dStrcpy(meshName, subMesh.name);

            char* pch = dStrtok(meshName, "_");
            while (pch != NULL)
            {
               if (dStrnicmp(pch, "collision", 9) == 0)
                  subMesh.isCollisionMesh = true;

               pch = dStrtok(NULL, "_");
            }

            if (subMesh.isCollisionMesh)
            {
               // Collision Box
               subMesh.renderData      = NULL;
               subMesh.collisionMesh   = Physics::createPhysicsMesh(Point3F(0.0f, 0.0f, 0.0f), Point3F(0.0f, 0.0f, 0.0f), Point3F(1.0f, 1.0f, 1.0f), *mMeshAsset->getMeshData(n), this);
               subMesh.collisionMesh->setStatic(true);
            }
            else {
               subMesh.renderData      = Rendering::createRenderData();
               subMesh.collisionMesh   = NULL;
            }

            mSubMeshes.push_back(subMesh);
         }
      }

      refresh();
   }

   void MeshComponent::onRemoveFromScene()
   {
      mAddedToScene = false;
      if (mMeshAsset.isNull()) return;

      bool renderMesh = false;
      if (mOwnerObject)
         renderMesh = !mOwnerObject->mGhosted || mOwnerObject->isClientObject();

      mMaterialAssets.clear();

      if (renderMesh)
      {
         for (S32 n = 0; n < mSubMeshes.size(); ++n)
         {
            SubMesh* subMesh = &mSubMeshes[n];

            if (subMesh->collisionMesh != NULL)
            {
               Physics::deletePhysicsObject(subMesh->collisionMesh);
            }

            if (subMesh->renderData != NULL)
            {
               subMesh->renderData->flags |= Rendering::RenderData::Deleted;
               subMesh->renderData = NULL;
            }
         }
         mSubMeshes.clear();
      }
   }

   bool MeshComponent::isSubMeshEnabled(U32 index)
   {
      char fieldName[32];
      dSprintf(fieldName, 32, "SubMesh%d", index);
      const char* fieldValue = getDataField(StringTable->insert(fieldName), NULL);
      if (dStrcmp(fieldValue, "false") == 0)
         return false;

      return true;
   }

   bool MeshComponent::raycast(const Point3F& start, const Point3F& end, Point3F& hitPoint)
   {
      if (mMeshAsset.isNull()) return false;

      // Transform the start and end points into object space.
      // TODO: Optimize this.
      F32 invTransformMtx[16];
      bx::mtxInverse(invTransformMtx, mTransformMatrix);

      Point3F transformedStart;
      bx::vec3MulMtxH(transformedStart, start, invTransformMtx);
      Point3F transformedEnd;
      bx::vec3MulMtxH(transformedEnd, end, invTransformMtx);

      // Early out from bounding box.
      if (!mBoundingBox.collideLine(transformedStart, transformedEnd))
         return false;

      // Get raycast results from mesh asset.
      bool result = mMeshAsset->raycast(transformedStart, transformedEnd, hitPoint);

      // If we hit something transform the point back to world space.
      if (result)
      {
         F32 _hitPoint[3] = { hitPoint.x, hitPoint.y, hitPoint.z };
         F32 _transformedHitPoint[3];
         bx::vec3MulMtxH(_transformedHitPoint, _hitPoint, mTransformMatrix);
         hitPoint.set(_transformedHitPoint[0], _transformedHitPoint[1], _transformedHitPoint[2]);
      }

      return result;
   }

   void MeshComponent::refresh()
   {
      Parent::refresh();

      // Sanity Checks.
      if ( mOwnerObject == NULL ) return;
      if ( mMeshAsset.isNull() ) return;

      refreshTransforms();
      refreshMaterials();

      bool defaultMaterial = (mMaterialAssets.size() < 1);
      for ( S32 n = 0; n < mSubMeshes.size(); ++n )
      {
         SubMesh* subMesh = &mSubMeshes[n];

         if (subMesh->renderData == NULL)
            continue;

         // Check if this SubMesh is disabled.
         subMesh->renderData->flags &= ~Rendering::RenderData::Hidden;
         if (!isSubMeshEnabled(n))
         {
            subMesh->renderData->flags |= Rendering::RenderData::Hidden;
            continue;
         }

         if (mOwnerObject->isStatic())
         {
            subMesh->renderData->flags |= Rendering::RenderData::Static;
         }

         if (mMeshAsset->isSkinned())
         {
            subMesh->renderData->flags |= Rendering::RenderData::Skinned;
         }

         // Buffers
         subMesh->renderData->flags |= Rendering::RenderData::CastShadow;
         subMesh->renderData->indexBuffer = mMeshAsset->getIndexBuffer(n);
         subMesh->renderData->vertexBuffer = mMeshAsset->getVertexBuffer(n);

         // Uniform/Texture Vectors (these are filled by applyMaterial)
         subMesh->uniforms.clear();
         subMesh->renderData->uniforms.uniforms = &subMesh->uniforms;
         subMesh->textures.clear();
         subMesh->renderData->textures = &subMesh->textures;

         // Apply Material
         if (!defaultMaterial)
         {
            U32 matIndex = mMeshAsset->getMaterialIndex(n);
            if (matIndex > ((U32)mMaterialAssets.size() - 1)) matIndex = 0;

            mMaterialAssets[matIndex]->applyMaterial(subMesh->renderData);
         }
         else {
            // Apply default material settings.
            subMesh->renderData->shader.idx = bgfx::invalidHandle;
         }

         // Merge in any component-wide uniforms.
         if ( !mUniforms.isEmpty() )
            subMesh->renderData->uniforms.addUniformSet(mUniforms);
      }
   }

   // By making this a separate function animations don't need to update everything.
   // That includes skipping getNearestLights call.
   void MeshComponent::refreshTransforms()
   {
      for ( S32 n = 0; n < mSubMeshes.size(); ++n )
      {
         SubMesh* subMesh = &mSubMeshes[n];

         if (subMesh->renderData == NULL)
            continue;

         // Base Component transform matrix is always slot 0 in the transform table.
         dMemcpy(mTransformTable[0], mTransformMatrix, sizeof(mTransformMatrix));
         if ( mTransformCount < 1 ) mTransformCount = 1;
         subMesh->renderData->transformTable = mTransformTable[0];
         subMesh->renderData->transformCount = mTransformCount;

         subMesh->renderData->flags |= Rendering::RenderData::HasBounds;
         subMesh->renderData->boundingBox = mMeshAsset->getMeshBoundingBox(n);
         subMesh->renderData->boundingBox.transform(mTransformMatrix);
         subMesh->renderData->boundingSphere = subMesh->renderData->boundingBox.getBoundingSphere();
      }

      // Bounding Box
      mBoundingBox = mMeshAsset->getBoundingBox();
      mBoundingBox.transform(mTransform);
   }

   void MeshComponent::refreshMaterials()
   {
      mMaterialAssets.clear();

      // Maximum of 32 materials (arbitrary)
      for (U32 n = 0; n < 32; ++n)
      {
         char mat_name[32];
         dSprintf(mat_name, 32, "MaterialAsset%d", n);
         const char* mat_asset_id = getDataField(StringTable->insert(mat_name), NULL);
         if (dStrlen(mat_asset_id) == 0 || dStrcmp(mat_asset_id, "Empty") == 0)
            continue;

         AssetPtr<MaterialAsset> mat;
         mat.setAssetId(mat_asset_id);
         if (!mat.isNull())
            mMaterialAssets.push_back(mat);
      }
   }
}
