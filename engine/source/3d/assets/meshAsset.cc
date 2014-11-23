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
#include "graphics/vertexLayouts.h"
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
    mVertexBuffer.idx = bgfx::invalidHandle;
    mIndexBuffer.idx = bgfx::invalidHandle;

    // Zero out bone transforms.
    for ( U32 i = 0; i < 49; ++i )
    {
      for ( U32 j = 0; j < 16; ++j )
         mBoneTransforms[i][j] = 0.0f;
    }
}

//------------------------------------------------------------------------------

MeshAsset::~MeshAsset()
{
   if ( mVertexBuffer.idx != bgfx::invalidHandle )
      bgfx::destroyVertexBuffer(mVertexBuffer);

   if ( mIndexBuffer.idx != bgfx::invalidHandle )
      bgfx::destroyIndexBuffer(mIndexBuffer);

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
   // Use Assimp To Load Dwarf.x
   mScene = aiImportFile(mMeshFile, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
   if ( !mScene ) return;

   aiMatrix4x4t<F32> rootTransform = mScene->mRootNode->mTransformation;
   rootTransform.Inverse();
   mGlobalInverseTransform = rootTransform;

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

      // Default bone index/weight values.
      vert.m_boneindex[0] = 0;
      vert.m_boneindex[1] = 0;
      vert.m_boneindex[2] = 0;
      vert.m_boneindex[3] = 0;
      vert.m_boneweight[0] = 0.0f;
      vert.m_boneweight[1] = 0.0f;
      vert.m_boneweight[2] = 0.0f;
      vert.m_boneweight[3] = 0.0f;

      mRawVerts.push_back(vert);
   }

   // Process Bones/Nodes
   for ( U32 n = 0; n < mMeshData->mNumBones; ++n )
   {
      aiBone* boneData = mMeshData->mBones[n];

      // Store bone index by name, and store it's offset matrix.
      U32 boneIndex = 0;
      if ( mBoneMap.find(boneData->mName.C_Str()) == mBoneMap.end() )
      {
         mBoneMap.insert(boneData->mName.C_Str(), n);
         mBoneOffsets.push_back(boneData->mOffsetMatrix);
         boneIndex = mBoneOffsets.size() - 1;
      } else {
         boneIndex = mBoneMap[boneData->mName.C_Str()];
         mBoneOffsets[boneIndex] = boneData->mOffsetMatrix;
      }

      // 
      for ( U32 i = 0; i < boneData->mNumWeights; ++i )
      {
         if ( boneData->mWeights[i].mVertexId >= mRawVerts.size() ) continue;
         Graphics::PosTexcoordVertex* vert = &mRawVerts[boneData->mWeights[i].mVertexId];
         for ( U32 j = 0; j < 4; ++j )
         {
            if ( vert->m_boneindex[j] == 0 && vert->m_boneweight[j] == 0.0f )
            {
               if ( j > 0 ) 
                  Con::printf("WHAT THE BLOODY FUCK?");
               vert->m_boneindex[j] = boneIndex + 1;
               vert->m_boneweight[j] = boneData->mWeights[i].mWeight / (j + 1);

               //Con::printf("Vert ID: %d Bone Index: %d Bone Weight: %f", boneData->mWeights[i].mVertexId, n, boneData->mWeights[i].mWeight);

               // Rescale the previous vert weights.
               for ( U32 k = 0; k < j; ++k )
               {
                  Con::printf("Old Vert Weight: %f", vert->m_boneweight[k]);
                  vert->m_boneweight[k] = vert->m_boneweight[k] * (k + 1);
                  vert->m_boneweight[k] = vert->m_boneweight[k] / (j + 1);
                  Con::printf("New Vert Weight: %f", vert->m_boneweight[k]);
               }
               break;
            } else {
               Con::printf("VERT ALREADY HAS A BONE WEIGHT!");
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
         // Con::printf("[ASSIMP] Face: %d %d %d", face->mIndices[0], face->mIndices[1], face->mIndices[2]);
         mRawIndices.push_back(face->mIndices[0]);
         mRawIndices.push_back(face->mIndices[1]);
         mRawIndices.push_back(face->mIndices[2]);
      } else {
         Con::warnf("[ASSIMP] Non-Triangle Face Found.");
      }
   }

   // Check for animations
   if ( mScene->HasAnimations() )
   {
      for ( U32 n = 0; n < mScene->mNumAnimations; ++n)
      {
         aiAnimation* anim = mScene->mAnimations[n];
         Con::printf("Mesh Animation Name: %s", anim->mName.C_Str());
      }
   }

   // Load the verts and indices into bgfx buffers
	mVertexBuffer = bgfx::createVertexBuffer(
		   bgfx::makeRef(&mRawVerts[0], mRawVerts.size() * sizeof(Graphics::PosTexcoordVertex) ), 
         Graphics::PosTexcoordVertex::ms_decl
		);

	mIndexBuffer = bgfx::createIndexBuffer(
         bgfx::makeRef(&mRawIndices[0], mRawIndices.size() * sizeof(U16) )
		);

   BoneTransform(0.5f);
}

void MeshAsset::BoneTransform(F32 TimeInSeconds)
{
    MatrixF Identity;
    Identity.identity();

    F32 TicksPerSecond = mScene->mAnimations[0]->mTicksPerSecond != 0 ? 
                            (F32)mScene->mAnimations[0]->mTicksPerSecond : 25.0f;
    F32 TimeInTicks = TimeInSeconds * TicksPerSecond;
    F32 AnimationTime = (F32)fmod(TimeInTicks, mScene->mAnimations[0]->mDuration);

    ReadNodeHeirarchy(AnimationTime, mScene->mRootNode, Identity);

    //Transforms.resize(m_NumBones);

    for (U32 i = 0 ; i < 49 ; i++) {
       Con::printf("[%f %f %f %f] [%f %f %f %f] [%f %f %f %f] [%f %f %f %f]\n",
				mBoneTransforms[i][0], mBoneTransforms[i][1], mBoneTransforms[i][2], mBoneTransforms[i][3],
            mBoneTransforms[i][4], mBoneTransforms[i][5], mBoneTransforms[i][6], mBoneTransforms[i][7],
            mBoneTransforms[i][8], mBoneTransforms[i][9], mBoneTransforms[i][10], mBoneTransforms[i][11],
            mBoneTransforms[i][12], mBoneTransforms[i][13], mBoneTransforms[i][14], mBoneTransforms[i][15]);
       mBoneTransforms[i].transpose();
    }
    Con::printf("Done bone  transform.");
}

aiNodeAnim* MeshAsset::FindNodeAnim(const aiAnimation* pAnimation, const char* nodeName)
{
   for ( U32 n = 0; n < pAnimation->mNumChannels; ++n )
   {
      aiNodeAnim* node = pAnimation->mChannels[n];
      if ( dStrcmp(node->mNodeName.C_Str(), nodeName) == 0 )
         return node;
   }

   return NULL;
}

void MeshAsset::ReadNodeHeirarchy(F32 AnimationTime, const aiNode* pNode, MatrixF ParentTransform)
{ 
    const char* nodeName = pNode->mName.data;

    const aiAnimation* pAnimation = mScene->mAnimations[0];

    //F32 NodeTransformation[16];
    //aiMatrixToMatrix(pNode->mTransformation, NodeTransformation);
    MatrixF NodeTransformation(pNode->mTransformation);

    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, nodeName);

    if (pNodeAnim) {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        MatrixF ScalingM;
        ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim); 
        MatrixF RotationM = MatrixF(RotationQ.GetMatrix());

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
        MatrixF TranslationM;
        TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

        NodeTransformation = TranslationM * RotationM * ScalingM;
    }

    MatrixF GlobalTransformation = ParentTransform * NodeTransformation;

    if (mBoneMap.find(nodeName) != mBoneMap.end()) {
      U32 BoneIndex = mBoneMap[nodeName];
      mBoneTransforms[BoneIndex] = mGlobalInverseTransform * GlobalTransformation * mBoneOffsets[BoneIndex];
    }

    for (U32 i = 0 ; i < pNode->mNumChildren ; i++) {
        ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

void MeshAsset::CalcInterpolatedRotation(aiQuaternion& Out, F32 AnimationTime, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    U32 RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    U32 NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    F32 DeltaTime = (F32)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    F32 Factor = (AnimationTime - (F32)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}

U32 MeshAsset::FindRotation(F32 AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (U32 i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
        if (AnimationTime < (F32)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    // Need Error Handling
    return 0;
}

void MeshAsset::CalcInterpolatedScaling(aiVector3D& Out, F32 AnimationTime, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    U32 ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    U32 NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    F32 DeltaTime = (F32)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    F32 Factor = (AnimationTime - (F32)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

U32 MeshAsset::FindScaling(F32 AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (U32 i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
        if (AnimationTime < (F32)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    // Need Error Handling
    return 0;
}

void MeshAsset::CalcInterpolatedPosition(aiVector3D& Out, F32 AnimationTime, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    U32 PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    U32 NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    F32 DeltaTime = (F32)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
    F32 Factor = (AnimationTime - (F32)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

U32 MeshAsset::FindPosition(F32 AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumPositionKeys > 0);

    for (U32 i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    // Need Error Handling
    return 0;
}

void MeshAsset::aiQuatToMatrix(aiQuaternion in, F32* out)
{
   // Current Layout: 
   // X1  Y1  Z1
   // X2  Y2  Z2
   // X3  Y3  Z3

   // Goal:
   // X1 X2 X3 0
   // Y1 Y2 Y3 0
   // Z1 Z2 Z3 0
   // 0  0  0  1

   aiMatrix3x3t<F32> mtx = in.GetMatrix();
   out[0] = mtx.a1;
   out[1] = mtx.b1;
   out[2] = mtx.c1;
   out[3] = 0;

   out[4] = mtx.a2;
   out[5] = mtx.b2;
   out[6] = mtx.c2;
   out[7] = 0;

   out[8] = mtx.a3;
   out[9] = mtx.b3;
   out[10] = mtx.c3;
   out[11] = 0;

   out[12] = 0;
   out[13] = 0;
   out[14] = 0;
   out[15] = 1;
}

void MeshAsset::aiMatrixToMatrix(aiMatrix4x4t<F32> in, F32* out)
{
   // Current:
   // X1  Y1  Z1  T1
   // X2  Y2  Z2  T2
   // X3  Y3  Z3  T3
   // 0   0   0   1

   // Goal:
   // x.x x.y x.z 0
   // y.x y.y y.z 0
   // z.x z.y z.z 0
   // t.x t.y t.z 1

   out[0] = in.a1;
   out[1] = in.b1;
   out[2] = in.c1;
   out[3] = in.d1;

   out[4] = in.a2;
   out[5] = in.b2;
   out[6] = in.c2;
   out[7] = in.d2;

   out[8] = in.a3;
   out[9] = in.b3;
   out[10] = in.c3;
   out[11] = in.d3;

   out[12] = in.a4;
   out[13] = in.b4;
   out[14] = in.c4;
   out[15] = in.d4;
}