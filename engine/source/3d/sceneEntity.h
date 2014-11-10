#ifndef _SCENEENTITY_H_
#define _SCENEENTITY_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _MESH_ASSET_H_
#include "assets/meshAsset.h"
#endif


namespace Scene 
{
   class SceneEntity
   {
      private:
         AssetPtr<MeshAsset> mMeshAsset;
   };
}

#endif