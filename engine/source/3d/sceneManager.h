#ifndef _SCENEMANAGER_H_
#define _SCENEMANAGER_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _SCENEENTITY_H_
#include "sceneEntity.h"
#endif

namespace Scene
{
   extern Vector<SceneEntity> entityList;

   void frame();
   void addEntity();
   void removeEntity();
}

#endif