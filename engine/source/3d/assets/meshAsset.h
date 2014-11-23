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

#ifndef _MESH_ASSET_H_
#define _MESH_ASSET_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _VERTEXLAYOUTS_H_
#include "graphics/vertexLayouts.h"
#endif

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

#ifndef _SHADERS_H_
#include "graphics/shaders.h"
#endif

#ifndef __AI_SCENE_H_INC__
#include <assimp/scene.h>
#endif

#ifndef AI_QUATERNION_H_INC
#include <assimp/quaternion.h>
#endif

#ifndef AI_ANIM_H_INC
#include <assimp/anim.h>
#endif

#ifndef _HASHTABLE_H
#include "collection/hashTable.h"
#endif

//-----------------------------------------------------------------------------

DefineConsoleType( TypeMeshAssetPtr )

//-----------------------------------------------------------------------------

class MeshAsset : public AssetBase
{

private:
   typedef AssetBase  Parent;

   const aiScene*                         mScene;
   MatrixF                                mGlobalInverseTransform;
   HashMap<const char*, U32>              mBoneMap;
   Vector<MatrixF>                        mBoneOffsets;
   StringTableEntry                       mMeshFile;
   Vector<Graphics::PosTexcoordVertex>    mRawVerts;
   Vector<U16>                            mRawIndices;
   bgfx::VertexBufferHandle               mVertexBuffer;
   bgfx::IndexBufferHandle                mIndexBuffer;

public:
   MatrixF                                mBoneTransforms[49]; // Temp, figure this out later.

    MeshAsset();
    virtual ~MeshAsset();

    static void initPersistFields();
    virtual bool onAdd();
    virtual void onRemove();
    virtual void copyTo(SimObject* object);

    // Asset validation.
    virtual bool isAssetValid( void ) const;

    void                    setMeshFile( const char* pMeshFile );
    inline StringTableEntry getMeshFile( void ) const { return mMeshFile; };
    void loadMesh();

    // Animation Functions
    void BoneTransform(F32 TimeInSeconds);
    void ReadNodeHeirarchy(F32 AnimationTime, const aiNode* pNode, MatrixF ParentTransform);
    aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const char* nodeName);
    void CalcInterpolatedRotation(aiQuaternion& Out, F32 AnimationTime, const aiNodeAnim* pNodeAnim);
    U32 FindRotation(F32 AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedScaling(aiVector3D& Out, F32 AnimationTime, const aiNodeAnim* pNodeAnim);
    U32 FindScaling(F32 AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, F32 AnimationTime, const aiNodeAnim* pNodeAnim);
    U32 FindPosition(F32 AnimationTime, const aiNodeAnim* pNodeAnim);
    void aiMatrixToMatrix(aiMatrix4x4t<F32> in, F32* out);
    void aiQuatToMatrix(aiQuaternion in, F32* out);

    bgfx::VertexBufferHandle  getVertexBuffer() { return mVertexBuffer; }
    bgfx::IndexBufferHandle   getIndexBuffer() { return mIndexBuffer; }

    /// Declare Console Object.
    DECLARE_CONOBJECT(MeshAsset);

protected:
    virtual void initializeAsset( void );
    virtual void onAssetRefresh( void );

    static bool setMeshFile( void* obj, const char* data )                 { static_cast<MeshAsset*>(obj)->setMeshFile(data); return false; }
    static const char* getMeshFile(void* obj, const char* data)            { return static_cast<MeshAsset*>(obj)->getMeshFile(); }
};

#endif // _MESH_ASSET_H_