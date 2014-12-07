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
#include "meshAsset.h"
#include "graphics/utilities.h"
#include "math/mMatrix.h"

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
   mMeshFile(StringTable->EmptyString),
   mScene ( NULL )
{

}

//------------------------------------------------------------------------------

MeshAsset::~MeshAsset()
{
   for ( S32 m = 0; m < mMeshList.size(); ++m )
   {
      if ( mMeshList[m].mVertexBuffer.idx != bgfx::invalidHandle )
         bgfx::destroyVertexBuffer(mMeshList[m].mVertexBuffer);

      if ( mMeshList[m].mIndexBuffer.idx != bgfx::invalidHandle )
         bgfx::destroyIndexBuffer(mMeshList[m].mIndexBuffer);
   }

   // Clean up.
   if ( mScene )
      aiReleaseImport(mScene);
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

   mMeshFile = expandAssetFilePath( mMeshFile );

   loadMesh();
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

   // Refresh the asset.
   refreshAsset();
}

void MeshAsset::loadMesh()
{
   // Use Assimp To Load Mesh
   mScene = aiImportFile(mMeshFile, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
   if ( !mScene ) return;

   // TODO: Replace with loading all meshes.
   for( U32 m = 0; m < mScene->mNumMeshes; ++m )
   {
      aiMesh* mMeshData = mScene->mMeshes[m];
      SubMesh newSubMesh;
      mMeshList.push_back(newSubMesh);
      SubMesh* subMeshData = &mMeshList[mMeshList.size()-1];

      subMeshData->mVertexBuffer.idx = bgfx::invalidHandle;
      subMeshData->mIndexBuffer.idx = bgfx::invalidHandle;
         
      // Verts/UVs/Bones
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

         // Default bone index/weight values.
         vert.m_boneindex[0] = 0;
         vert.m_boneindex[1] = 0;
         vert.m_boneindex[2] = 0;
         vert.m_boneindex[3] = 0;
         vert.m_boneweight[0] = 0.0f;
         vert.m_boneweight[1] = 0.0f;
         vert.m_boneweight[2] = 0.0f;
         vert.m_boneweight[3] = 0.0f;

         subMeshData->mRawVerts.push_back(vert);
      }

      // Process Bones/Nodes
      for ( U32 n = 0; n < mMeshData->mNumBones; ++n )
      {
         aiBone* boneData = mMeshData->mBones[n];

         // Store bone index by name, and store it's offset matrix.
         U32 boneIndex = 0;
         if ( mBoneMap.find(boneData->mName.C_Str()) == mBoneMap.end() )
         {
            boneIndex = mBoneOffsets.size();
            mBoneMap.insert(boneData->mName.C_Str(), boneIndex);
            mBoneOffsets.push_back(boneData->mOffsetMatrix);
         } else {
            boneIndex = mBoneMap[boneData->mName.C_Str()];
            mBoneOffsets[boneIndex] = boneData->mOffsetMatrix;
         }

         // Store the bone indices and weights in the vert data.
         for ( U32 i = 0; i < boneData->mNumWeights; ++i )
         {
            if ( boneData->mWeights[i].mVertexId >= subMeshData->mRawVerts.size() ) continue;
            Graphics::PosTexcoordVertex* vert = &subMeshData->mRawVerts[boneData->mWeights[i].mVertexId];
            for ( U32 j = 0; j < 4; ++j )
            {
               if ( vert->m_boneindex[j] == 0 && vert->m_boneweight[j] == 0.0f )
               {
                  // TODO: This + 1 is there because we know 0 in the transform table
                  // is the main transformation. Maybe this should be done in the 
                  // vertex shader instead?
                  vert->m_boneindex[j] = boneIndex + 1;
                  vert->m_boneweight[j] = boneData->mWeights[i].mWeight / (j + 1);

                  // Rescale the previous vert weights.
                  for ( U32 k = 0; k < j; ++k )
                  {
                     vert->m_boneweight[k] = vert->m_boneweight[k] * (k + 1);
                     vert->m_boneweight[k] = vert->m_boneweight[k] / (j + 1);
                  }
                  break;
               }
            }
         }
      }
         
      // Faces
      for ( U32 n = 0; n < mMeshData->mNumFaces; ++n)
      {
         const struct aiFace* face = &mMeshData->mFaces[n];
         if ( face->mNumIndices == 3 )
         {
            subMeshData->mRawIndices.push_back(face->mIndices[0]);
            subMeshData->mRawIndices.push_back(face->mIndices[1]);
            subMeshData->mRawIndices.push_back(face->mIndices[2]);
         } else {
            Con::warnf("[ASSIMP] Non-Triangle Face Found.");
         }
      }

      // Load the verts and indices into bgfx buffers
	   subMeshData->mVertexBuffer = bgfx::createVertexBuffer(
		      bgfx::makeRef(&subMeshData->mRawVerts[0], subMeshData->mRawVerts.size() * sizeof(Graphics::PosTexcoordVertex) ), 
            Graphics::PosTexcoordVertex::ms_decl
		   );

	   subMeshData->mIndexBuffer = bgfx::createIndexBuffer(
            bgfx::makeRef(&subMeshData->mRawIndices[0], subMeshData->mRawIndices.size() * sizeof(U16) )
		   );
   }
}

// Returns the number of transformations loaded into transformsOut.
U32 MeshAsset::getAnimatedTransforms(F64 TimeInSeconds, F32* transformsOut)
{
   if ( !mScene ) return 0;

   MatrixF Identity;
   Identity.identity();

   aiMatrix4x4t<F32> rootTransform = mScene->mRootNode->mTransformation;
   rootTransform.Inverse();
   MatrixF GlobalInverseTransform = rootTransform;

   F64 TicksPerSecond = mScene->mAnimations[0]->mTicksPerSecond != 0 ? 
                           mScene->mAnimations[0]->mTicksPerSecond : 25.0f;
   F64 TimeInTicks = TimeInSeconds * TicksPerSecond;
   F64 AnimationTime = fmod(TimeInTicks, mScene->mAnimations[0]->mDuration);

   return _readNodeHeirarchy(AnimationTime, mScene->mRootNode, Identity, GlobalInverseTransform, transformsOut);
}

U32 MeshAsset::_readNodeHeirarchy(F64 AnimationTime, const aiNode* pNode, 
                                  MatrixF ParentTransform, MatrixF GlobalInverseTransform, F32* transformsOut)
{ 
   U32 xfrmCount = 0;
   const char* nodeName = pNode->mName.data;
   const aiAnimation* pAnimation = mScene->mAnimations[0];
   const aiNodeAnim* pNodeAnim = _findNodeAnim(pAnimation, nodeName);
   MatrixF NodeTransformation(pNode->mTransformation);

   if ( pNodeAnim ) 
   {
      // Interpolate scaling and generate scaling transformation matrix
      aiVector3D Scaling;
      _calcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
      MatrixF ScalingM;
      ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

      // Interpolate rotation and generate rotation transformation matrix
      aiQuaternion RotationQ;
      _calcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim); 
      MatrixF RotationM = MatrixF(RotationQ.GetMatrix());

      // Interpolate translation and generate translation transformation matrix
      aiVector3D Translation;
      _calcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
      MatrixF TranslationM;
      TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

      NodeTransformation = TranslationM * RotationM * ScalingM;
   }

   MatrixF GlobalTransformation = ParentTransform * NodeTransformation;

   if ( mBoneMap.find(nodeName) != mBoneMap.end() ) 
   {
      U32 BoneIndex = mBoneMap[nodeName];
      xfrmCount = BoneIndex + 1;

      MatrixF boneTransform = GlobalInverseTransform * GlobalTransformation * mBoneOffsets[BoneIndex];
      // Assimp matricies are row-major, we need to transpose to column-major.
      boneTransform.transpose();
      dMemcpy(&transformsOut[BoneIndex * 16], boneTransform, sizeof(F32) * 16); 
   }

   for ( U32 i = 0 ; i < pNode->mNumChildren ; i++ ) 
   {
      U32 childXfrmCount = _readNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation, GlobalInverseTransform, transformsOut);
      if ( childXfrmCount > xfrmCount )
         xfrmCount = childXfrmCount;
   }

   return xfrmCount;
}

aiNodeAnim* MeshAsset::_findNodeAnim(const aiAnimation* pAnimation, const char* nodeName)
{
   for ( U32 n = 0; n < pAnimation->mNumChannels; ++n )
   {
      aiNodeAnim* node = pAnimation->mChannels[n];
      if ( dStrcmp(node->mNodeName.C_Str(), nodeName) == 0 )
         return node;
   }

   return NULL;
}

void MeshAsset::_calcInterpolatedRotation(aiQuaternion& Out, F64 AnimationTime, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    U32 RotationIndex = _findRotation(AnimationTime, pNodeAnim);
    U32 NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    F64 DeltaTime = (pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    F64 Factor = (AnimationTime - pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, (F32)Factor);
    Out = Out.Normalize();
}

U32 MeshAsset::_findRotation(F64 AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (U32 i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
        if (AnimationTime < (F32)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    // TODO: Need Error Handling
    return 0;
}

void MeshAsset::_calcInterpolatedScaling(aiVector3D& Out, F64 AnimationTime, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    U32 ScalingIndex = _findScaling(AnimationTime, pNodeAnim);
    U32 NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    F64 DeltaTime = (pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    F64 Factor = (AnimationTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + (F32)Factor * Delta;
}

U32 MeshAsset::_findScaling(F64 AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (U32 i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
        if (AnimationTime < (F32)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    // TODO: Need Error Handling
    return 0;
}

void MeshAsset::_calcInterpolatedPosition(aiVector3D& Out, F64 AnimationTime, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    U32 PositionIndex = _findPosition(AnimationTime, pNodeAnim);
    U32 NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    F64 DeltaTime = (pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
    F64 Factor = (AnimationTime - pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + (F32)Factor * Delta;
}

U32 MeshAsset::_findPosition(F64 AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumPositionKeys > 0);

    for (U32 i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    // TODO: Need Error Handling
    return 0;
}