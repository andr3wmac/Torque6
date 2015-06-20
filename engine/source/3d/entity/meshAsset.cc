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
#include "meshAsset.h"
#include "graphics/core.h"
#include "math/mMatrix.h"

// Script bindings.
#include "meshAsset_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

// bgfx/bx
#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

// Assimp - Asset Import Library
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

// Binary Mesh Version Number
U8 MeshAsset::BinVersion = 100;

MeshAsset* getMeshAsset(const char* id)
{
   AssetPtr<MeshAsset> result;
   StringTableEntry assetId = StringTable->insert(id);
   result.setAssetId(assetId);
   return result;
}

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
   mImportThread = NULL;
   mBoundingBox.minExtents.set(0, 0, 0);
   mBoundingBox.maxExtents.set(0, 0, 0);
   mIsAnimated = false;
   mIsLoaded = false;
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

   //Con::printf("MESH IMPORT THREAD CREATED!");
   //mImportThread = new MeshImportThread(this);
   //mImportThread->start();
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

   // Update.
   mMeshFile = getOwned() ? expandAssetFilePath( pMeshFile ) : StringTable->insert( pMeshFile );

   // Refresh the asset.
   refreshAsset();
}

void MeshAsset::loadMesh()
{
   if ( !loadBin() )
   {
      importMesh();
      processMesh();
      saveBin();
   } else {
      processMesh();
   }
   mIsLoaded = true;
}

void MeshAsset::importMesh()
{
   //U64 hpFreq = bx::getHPFrequency() / 1000000.0; // micro-seconds.
   //U64 startTime = bx::getHPCounter();

   // Use Assimp To Load Mesh
   mScene = mAssimpImporter.ReadFile(mMeshFile, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
   if ( !mScene ) return;

   //U64 endTime = bx::getHPCounter();
   //Con::printf("ASSIMP IMPORT TOOK: %d microseconds. (1 microsecond = 0.001 milliseconds)", (U32)((endTime - startTime) / hpFreq));

   mIsAnimated = mScene->HasAnimations();

   for( U32 m = 0; m < mScene->mNumMeshes; ++m )
   {
      aiMesh* mMeshData = mScene->mMeshes[m];
      SubMesh newSubMesh;
      mMeshList.push_back(newSubMesh);
      SubMesh* subMeshData = &mMeshList[mMeshList.size()-1];

      // Defaults
      subMeshData->mBoundingBox.minExtents.set(0, 0, 0);
      subMeshData->mBoundingBox.maxExtents.set(0, 0, 0);
      subMeshData->mVertexBuffer.idx = bgfx::invalidHandle;
      subMeshData->mIndexBuffer.idx = bgfx::invalidHandle;
      subMeshData->mMaterialIndex = mMeshData->mMaterialIndex;

      // Verts/UVs/Bones
      for ( U32 n = 0; n < mMeshData->mNumVertices; ++n)
      {
         Graphics::PosUVTBNBonesVertex vert;

         // Verts
         aiVector3D pt = mMeshData->mVertices[n];
         vert.m_x = pt.x;
         vert.m_y = pt.y;
         vert.m_z = pt.z;

         // Bounding Box
         if ( vert.m_x < subMeshData->mBoundingBox.minExtents.x )
            subMeshData->mBoundingBox.minExtents.x = vert.m_x;
         if ( vert.m_x > subMeshData->mBoundingBox.maxExtents.x )
            subMeshData->mBoundingBox.maxExtents.x = vert.m_x;

         if ( vert.m_y < subMeshData->mBoundingBox.minExtents.y )
            subMeshData->mBoundingBox.minExtents.y = vert.m_y;
         if ( vert.m_y > subMeshData->mBoundingBox.maxExtents.y )
            subMeshData->mBoundingBox.maxExtents.y = vert.m_y;

         if ( vert.m_z < subMeshData->mBoundingBox.minExtents.z )
            subMeshData->mBoundingBox.minExtents.z = vert.m_z;
         if ( vert.m_z > subMeshData->mBoundingBox.maxExtents.z )
            subMeshData->mBoundingBox.maxExtents.z = vert.m_z;

         // UVs
         if ( mMeshData->HasTextureCoords(0) )
         {
            vert.m_u = mMeshData->mTextureCoords[0][n].x;
            vert.m_v = mMeshData->mTextureCoords[0][n].y;
         }

         // Tangents & Bitangents
         if ( mMeshData->HasTangentsAndBitangents() )
         {
            vert.m_tangent_x = mMeshData->mTangents[n].x;
            vert.m_tangent_y = mMeshData->mTangents[n].y; 
            vert.m_tangent_z = mMeshData->mTangents[n].z; 
            vert.m_bitangent_x = mMeshData->mBitangents[n].x;
            vert.m_bitangent_y = mMeshData->mBitangents[n].y; 
            vert.m_bitangent_z = mMeshData->mBitangents[n].z; 
         } else {
            vert.m_tangent_x = 0;
            vert.m_tangent_y = 0; 
            vert.m_tangent_z = 0; 
            vert.m_bitangent_x = 0;
            vert.m_bitangent_y = 0; 
            vert.m_bitangent_z = 0; 
         }

         // Normals
         if ( mMeshData->HasNormals() )
         {
            vert.m_normal_x = mMeshData->mNormals[n].x;
            vert.m_normal_y = mMeshData->mNormals[n].y; 
            vert.m_normal_z = mMeshData->mNormals[n].z; 
         } else {
            // TODO: Better default than zero?
            vert.m_normal_x = 0;
            vert.m_normal_y = 0; 
            vert.m_normal_z = 0; 
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
            if ( boneData->mWeights[i].mVertexId >= (U32)subMeshData->mRawVerts.size() ) continue;
            Graphics::PosUVTBNBonesVertex* vert = &subMeshData->mRawVerts[boneData->mWeights[i].mVertexId];
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
         if ( face->mNumIndices == 2 )
         {
            subMeshData->mRawIndices.push_back(face->mIndices[0]);
            subMeshData->mRawIndices.push_back(face->mIndices[1]);
         }
         else if ( face->mNumIndices == 3 )
         {
            subMeshData->mRawIndices.push_back(face->mIndices[0]);
            subMeshData->mRawIndices.push_back(face->mIndices[1]);
            subMeshData->mRawIndices.push_back(face->mIndices[2]);
         } else {
            Con::warnf("[ASSIMP] Non-Triangle Face Found.");
         }
      }
   }
}

bool MeshAsset::loadBin()
{
   //U64 hpFreq = bx::getHPFrequency() / 1000000.0; // micro-seconds.
   //U64 startTime = bx::getHPCounter();

   char cachedFilename[256];
   dSprintf(cachedFilename, 256, "%s.bin", mMeshFile);

   FileStream stream;
   if ( stream.open(cachedFilename, FileStream::ReadWrite) )
   {
      // Check Version Number
      U8 binVersionNumber;
      stream.read(&binVersionNumber);
      
      if ( binVersionNumber != MeshAsset::BinVersion )
         return false;

      mMeshList.clear();
      U32 meshCount = 0;
      stream.read(&meshCount);

      for ( U32 n = 0; n < meshCount; ++n)
      {
         SubMesh newSubMesh;
         mMeshList.push_back(newSubMesh);
         SubMesh* subMeshData = &mMeshList[mMeshList.size()-1];

         // Bounding Box
         stream.read(&subMeshData->mBoundingBox.minExtents.x);
         stream.read(&subMeshData->mBoundingBox.minExtents.y);
         stream.read(&subMeshData->mBoundingBox.minExtents.z);
         stream.read(&subMeshData->mBoundingBox.maxExtents.x); 
         stream.read(&subMeshData->mBoundingBox.maxExtents.y); 
         stream.read(&subMeshData->mBoundingBox.maxExtents.z); 

         // Material
         stream.read(&subMeshData->mMaterialIndex);

         // Indices
         U32 indexCount = 0;
         stream.read(&indexCount);
         for ( U32 i = 0; i < indexCount; ++i )
         {
            U16 index = 0;
            stream.read(&index);
            subMeshData->mRawIndices.push_back(index);
         }
      
         // Vertices
         U32 vertexCount = 0;
         stream.read(&vertexCount);
         for ( U32 i = 0; i < vertexCount; ++i )
         {
            Graphics::PosUVTBNBonesVertex vert;
         
            // Position
            stream.read(&vert.m_x);
            stream.read(&vert.m_y);
            stream.read(&vert.m_z);

            // UV
            stream.read(&vert.m_u);
            stream.read(&vert.m_v);

            // Tangent & Bitangent
            stream.read(&vert.m_tangent_x);
            stream.read(&vert.m_tangent_y);
            stream.read(&vert.m_tangent_z);
            stream.read(&vert.m_bitangent_x);
            stream.read(&vert.m_bitangent_y);
            stream.read(&vert.m_bitangent_z);

            // Normals
            stream.read(&vert.m_normal_x);
            stream.read(&vert.m_normal_y);
            stream.read(&vert.m_normal_z);

            // Bone Information
            stream.read(&vert.m_boneindex[0]);
            stream.read(&vert.m_boneindex[1]);
            stream.read(&vert.m_boneindex[2]);
            stream.read(&vert.m_boneindex[3]);
            stream.read(&vert.m_boneweight[0]);
            stream.read(&vert.m_boneweight[1]);
            stream.read(&vert.m_boneweight[2]);
            stream.read(&vert.m_boneweight[3]);

            subMeshData->mRawVerts.push_back(vert);
         }
      }

      stream.close();

      //U64 endTime = bx::getHPCounter();
      //Con::printf("BINARY IMPORT TOOK: %d microseconds. (1 microsecond = 0.001 milliseconds)", (U32)((endTime - startTime) / hpFreq));
      return true;
   } 

   return false;
}

void MeshAsset::saveBin()
{
   // For now we can't save animations.
   if ( mIsAnimated )
      return;

   char cachedFilename[256];
   dSprintf(cachedFilename, 256, "%s.bin", mMeshFile);

   FileStream stream;
   stream.open(cachedFilename, FileStream::Write);

   // Bin Version
   stream.write(MeshAsset::BinVersion);

   U32 meshCount = mMeshList.size();
   stream.write(meshCount);

   for ( U32 n = 0; n < meshCount; ++n)
   {
      SubMesh* subMeshData = &mMeshList[n];

      // Bounding Box
      stream.write(subMeshData->mBoundingBox.minExtents.x);
      stream.write(subMeshData->mBoundingBox.minExtents.y);
      stream.write(subMeshData->mBoundingBox.minExtents.z);
      stream.write(subMeshData->mBoundingBox.maxExtents.x); 
      stream.write(subMeshData->mBoundingBox.maxExtents.y); 
      stream.write(subMeshData->mBoundingBox.maxExtents.z); 

      // Material
      stream.write(subMeshData->mMaterialIndex);

      // Indices
      U32 indexCount = subMeshData->mRawIndices.size();
      stream.write(indexCount);
      for ( U32 i = 0; i < indexCount; ++i )
         stream.write(subMeshData->mRawIndices[i]);
      
      // Vertices
      U32 vertexCount = subMeshData->mRawVerts.size();
      stream.write(vertexCount);
      for ( U32 i = 0; i < vertexCount; ++i )
      {
         Graphics::PosUVTBNBonesVertex* vert = &subMeshData->mRawVerts[i];
         
         // Position
         stream.write(vert->m_x);
         stream.write(vert->m_y);
         stream.write(vert->m_z);

         // UV
         stream.write(vert->m_u);
         stream.write(vert->m_v);

         // Tangent & Bitangent
         stream.write(vert->m_tangent_x);
         stream.write(vert->m_tangent_y);
         stream.write(vert->m_tangent_z);
         stream.write(vert->m_bitangent_x);
         stream.write(vert->m_bitangent_y);
         stream.write(vert->m_bitangent_z);

         // Normals
         stream.write(vert->m_normal_x);
         stream.write(vert->m_normal_y);
         stream.write(vert->m_normal_z);

         // Bone Information
         stream.write(vert->m_boneindex[0]);
         stream.write(vert->m_boneindex[1]);
         stream.write(vert->m_boneindex[2]);
         stream.write(vert->m_boneindex[3]);
         stream.write(vert->m_boneweight[0]);
         stream.write(vert->m_boneweight[1]);
         stream.write(vert->m_boneweight[2]);
         stream.write(vert->m_boneweight[3]);
      }
   }

   stream.close();
}

void MeshAsset::processMesh()
{
   //U64 hpFreq = bx::getHPFrequency() / 1000000.0; // micro-seconds.
   //U64 startTime = bx::getHPCounter();

   for ( S32 n = 0; n < mMeshList.size(); ++n)
   {
      SubMesh* subMeshData = &mMeshList[n];

      // Load the verts and indices into bgfx buffers
	   subMeshData->mVertexBuffer = bgfx::createVertexBuffer(
		      bgfx::makeRef(&subMeshData->mRawVerts[0], subMeshData->mRawVerts.size() * sizeof(Graphics::PosUVTBNBonesVertex) ), 
            Graphics::PosUVTBNBonesVertex::ms_decl
		   );

	   subMeshData->mIndexBuffer = bgfx::createIndexBuffer(
            bgfx::makeRef(&subMeshData->mRawIndices[0], subMeshData->mRawIndices.size() * sizeof(U16) )
		   );

      // Bounding Box
      mBoundingBox.intersect(subMeshData->mBoundingBox);
   }

   //U64 endTime = bx::getHPCounter();
   //Con::printf("PROCESS MESH TOOK: %d microseconds. (1 microsecond = 0.001 milliseconds)", (U32)((endTime - startTime) / hpFreq));
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

// Threaded Mesh Import
MeshImportThread::MeshImportThread(MeshAsset* _meshAsset)
{
   mMeshAsset = _meshAsset;
}

void MeshImportThread::run(void *arg)
{
   mMeshAsset->loadMesh();
   Con::printf("ASSET LOADED FROM THREAD!");
}
