#ifndef _SCENEMANAGER_H_
#define _SCENEMANAGER_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _SCENEENTITY_H_
#include "sceneEntity.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _MESH_ASSET_H_
#include "assets/meshAsset.h"
#endif

namespace Scene
{
   // Entity Management
   extern Vector<SceneEntity> entityList;
   void addEntity(SceneEntity entity);
   void removeEntity(SceneEntity* entity);

   // Camera/View Controls
   extern F32 viewMatrix[16];
   extern F32 projectionMatrix[16];
   void cameraLookAt(Point3F eye_position, Point3F look_at_position);

   // Init/Destroy
   void init();
   void destroy();

   // Process Frame
   void render(U32 width, U32 height, U32 clearColor = 0);
}

#endif