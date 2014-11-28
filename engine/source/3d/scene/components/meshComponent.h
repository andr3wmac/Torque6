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

#ifndef _MESH_COMPONENT_H_
#define _MESH_COMPONENT_H_

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

#ifndef _MESH_ASSET_H_
#include "3d/assets/meshAsset.h"
#endif

#ifndef _FORWARD_MATERIAL_ASSET_H_
#include "3d/assets/forwardMaterialAsset.h"
#endif

#ifndef _SHADER_ASSET_H_
#include "3d/assets/shaderAsset.h"
#endif

#ifndef _FORWARDRENDERING_H_
#include "3d/scene/rendering/forwardRendering.h"
#endif

#ifndef _BASE_COMPONENT_H_
#include "baseComponent.h"
#endif

namespace Scene 
{
   class MeshComponent : public BaseComponent
   {
      private:
         typedef BaseComponent Parent;
         StringTableEntry                             mMeshAssetId;
         AssetPtr<MeshAsset>                          mMeshAsset;
         Vector<Scene::ForwardRenderData*>            mRenderDataList;
         Vector<AssetPtr<ForwardMaterialAsset>>       mMaterialAssets;

      public:
         // TODO: maybe not public?
         F32                              mTransformTable[50][16];
         U32                              mTransformCount;

         MeshComponent();

         void onAddToScene();
         void refresh();
         void setMesh( const char* pMeshAssetId );

         static void initPersistFields();

         DECLARE_CONOBJECT(MeshComponent);

         static bool setMesh(void* obj, const char* data) { static_cast<MeshComponent*>(obj)->setMesh( data ); return false; }
   };
}

#endif _MESH_COMPONENT_H_