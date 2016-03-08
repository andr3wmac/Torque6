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

#include "frustumCulling.h"

#include "console/consoleInternal.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "rendering/rendering.h"
#include "scene/scene.h"
#include "scene/components/cameraComponent.h"
#include "sysgui/sysgui.h"

#include <bgfx/bgfx.h>
#include <debugdraw/debugdraw.h>
#include <bounds.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Scene
{
   IMPLEMENT_CONOBJECT(FrustumCulling);

   FrustumCulling::FrustumCulling()
   {
      mPriority = 5000;
      mDebugger = dynamic_cast<FrustumCullingDebugger*>(Debug::getDebugMode("FrustumCulling"));
   }

   FrustumCulling::~FrustumCulling()
   {

   }

   void FrustumCulling::onAddToScene()
   {
      CameraComponent* camera = mOwnerObject->findComponentByType<CameraComponent>();
      if (!camera)
         return;

      mCamera = camera->getRenderCamera();
      if (mCamera)
         mCamera->addRenderFilter(this);

      // Reset plane cache.
      for (U32 n = 0; n < TORQUE_MAX_RENDER_DATA; ++n)
         mPlaneCache[n] = -1;
   }

   void FrustumCulling::onRemoveFromScene()
   {
      if (mCamera)
         mCamera->removeRenderFilter(this);
   }

   void FrustumCulling::onAddToCamera()
   {
      
   }

   void FrustumCulling::onRemoveFromCamera()
   {
      
   }

   void FrustumCulling::execute()
   {
      Rendering::RenderData* renderData = Rendering::getRenderDataList();

      // Get ViewProj Matrix
      float viewProjMtx[16];
      bx::mtxMul(viewProjMtx, mCamera->viewMatrix, mCamera->projectionMatrix);

      // Get Frustum Planes
      Plane planes[6];
      buildFrustumPlanes(planes, viewProjMtx);

      U32 totalObjects = Rendering::getRenderDataCount();
      U32 objectsCulled = 0;
      U32 objectsCacheCulled = 0;

      // Loop through each render data and cull it.
      for (U32 n = 0; n < Rendering::getRenderDataCount(); ++n, ++renderData)
      {
         // Items without bounds are not subject to frustum culling.
         if (!(renderData->flags & Rendering::RenderData::HasBounds))
            continue;

         // We cache the last plane this item failed on as it's most likely the one it will fail on again.
         if (mPlaneCache[n] != -1)
         {
            F32 distance = planes[mPlaneCache[n]].m_normal[0] * renderData->boundingSphere.center.x +
                           planes[mPlaneCache[n]].m_normal[1] * renderData->boundingSphere.center.y +
                           planes[mPlaneCache[n]].m_normal[2] * renderData->boundingSphere.center.z + planes[mPlaneCache[n]].m_dist;

            if (distance + renderData->boundingSphere.radius < 0.0)
            {
               renderData->flags |= Rendering::RenderData::Filtered;
               objectsCacheCulled++;
               continue;
            }
         }

         // Check the bounding sphere against each frustum plane.
         for (U8 i = 0; i < 6; ++i)
         {
            if (i == mPlaneCache[n])
               continue;

            F32 distance = planes[i].m_normal[0] * renderData->boundingSphere.center.x + 
                           planes[i].m_normal[1] * renderData->boundingSphere.center.y +
                           planes[i].m_normal[2] * renderData->boundingSphere.center.z + planes[i].m_dist;

            if (distance + renderData->boundingSphere.radius < 0.0) 
            {
               renderData->flags |= Rendering::RenderData::Filtered;
               mPlaneCache[n] = i;
               objectsCulled++;
               break;
            }
         }
      }

      // Update DebugMode Stats
      mDebugger->updateStats(totalObjects, objectsCulled, objectsCacheCulled);
   }

   // ----------------------------------------
   //   FrustumCulling Debugger : Displays the bounding spheres that are being tested.
   // ----------------------------------------

   IMPLEMENT_DEBUG_MODE("FrustumCulling", FrustumCullingDebugger);

   void FrustumCullingDebugger::onEnable()
   {
      SysGUI::beginScrollArea("Frustum Culling", 10, 10, 200, 100);

      mTotalObjectsLbl        = SysGUI::label("Total Objects: 0");
      mObjectsCulledLbl       = SysGUI::label("Objects Culled: 0");
      mObjectsCacheCulledLbl  = SysGUI::label("Objects Cache Culled: 0");

      SysGUI::endScrollArea();
      SysGUI::setEnabled(true);
   }

   void FrustumCullingDebugger::onDisable()
   {

   }

   void FrustumCullingDebugger::updateStats(U32 totalObjects, U32 objectsCulled, U32 objectsCacheCulled)
   {
      if (!mEnabled)
         return;

      char buffer[256];

      dSprintf(buffer, 256, "Total Objects: %d", totalObjects);
      SysGUI::setLabelValue(mTotalObjectsLbl, buffer);

      dSprintf(buffer, 256, "Objects Culled: %d", objectsCulled);
      SysGUI::setLabelValue(mObjectsCulledLbl, buffer);

      dSprintf(buffer, 256, "Objects Cache Culled: %d", objectsCacheCulled);
      SysGUI::setLabelValue(mObjectsCacheCulledLbl, buffer);
   }

   void FrustumCullingDebugger::render(Rendering::RenderCamera* camera)
   {
      Rendering::RenderData* renderData = Rendering::getRenderDataList();

      ddSetColor(BGFXCOLOR_RGBA(0, 255, 0, 128));
      ddSetWireframe(true);
      ddSetState(true, true, true);

      for (U32 n = 0; n < Rendering::getRenderDataCount(); ++n, ++renderData)
      {
         // Items without bounds are not subject to frustum culling.
         if (!(renderData->flags & Rendering::RenderData::HasBounds))
            continue;

         Sphere boundingSphere;
         boundingSphere.m_center[0] = renderData->boundingSphere.center.x;
         boundingSphere.m_center[1] = renderData->boundingSphere.center.y;
         boundingSphere.m_center[2] = renderData->boundingSphere.center.z;
         boundingSphere.m_radius = renderData->boundingSphere.radius;
         ddDraw(boundingSphere);
      }
   }
}
