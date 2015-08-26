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

#ifndef _MESH_COMPONENT_H_
#define _MESH_COMPONENT_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _VERTEXLAYOUTS_H_
#include "graphics/core.h"
#endif

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

#ifndef _SHADERS_H_
#include "graphics/shaders.h"
#endif

#ifndef _MESH_ASSET_H_
#include "3d/entity/meshAsset.h"
#endif

#ifndef _MATERIAL_ASSET_H_
#include "3d/material/materialAsset.h"
#endif

#ifndef _RENDERINGCOMMON_H_
#include "3d/rendering/common.h"
#endif

#ifndef _BASE_COMPONENT_H_
#include "baseComponent.h"
#endif

namespace Scene 
{

   class MeshComponent : public BaseComponent
   {
      struct SubMesh
      {
         Rendering::RenderData*                       renderData;
         Vector<Rendering::UniformData>               uniforms;
         Vector<Rendering::TextureData>               textures;
         Vector<Rendering::InstanceData>              instances;
      };

      private:
         typedef BaseComponent Parent;
         StringTableEntry                             mMeshAssetId;
         AssetPtr<MeshAsset>                          mMeshAsset;
         Vector< AssetPtr<MaterialAsset> >            mMaterialAssets;
         Vector<SubMesh>                              mSubMeshes;

      public:
         // TODO: maybe not public?
         F32                              mTransformTable[60][16];
         U32                              mTransformCount;

         MeshComponent();
         ~MeshComponent();

         void onAddToScene();
         void onRemoveFromScene();
         void refresh();
         void refreshTransforms();
         void setMesh( const char* pMeshAssetId );
         AssetPtr<MeshAsset> getMesh() { return mMeshAsset; }

         static void initPersistFields();

         DECLARE_CONOBJECT(MeshComponent);

         static bool setMesh(void* obj, const char* data) { static_cast<MeshComponent*>(obj)->setMesh( data ); return false; }
   };
}

#endif _MESH_COMPONENT_H_