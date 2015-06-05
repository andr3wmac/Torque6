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

#include "Gizmo.h"
#include <sim/simObject.h>
#include <plugins/plugins_shared.h>
#include <3d/rendering/common.h>
#include <graphics/core.h>
#include <bx/fpumath.h>

using namespace Plugins;

Gizmo::Gizmo()
{
   lastMousePosition.set(0, 0);
   mouseDirection.set(0.0f, 0.0f);
   translateDirection.set(0.0f, 0.0f, 0.0f);
   mSelectedEntity = NULL;
}

Gizmo::~Gizmo()
{

}

void Gizmo::selectEntity(Scene::SceneEntity* entity)
{
   mSelectedEntity = entity;
}

void Gizmo::render()
{
   if ( mSelectedEntity != NULL )
   {
      Link.Graphics.drawLine3D(mSelectedEntity->mPosition, mSelectedEntity->mPosition + Point3F(0, 50, 0), ColorI(0, 200, 0, 255), 5.0f);
      Link.Graphics.drawLine3D(mSelectedEntity->mPosition, mSelectedEntity->mPosition + Point3F(50, 0, 0), ColorI(200, 0, 0, 255), 5.0f);
      Link.Graphics.drawLine3D(mSelectedEntity->mPosition, mSelectedEntity->mPosition + Point3F(0, 0, 50), ColorI(0, 0, 200, 255), 5.0f);
   }
}

void Gizmo::onMouseDownEvent(const GuiEvent &event)
{
   if ( mSelectedEntity == NULL ) return;

   translateDirection.set(0,0,0);
   lastMousePosition = event.mousePoint;

   Point2I entityScreenPos = Link.Rendering.worldToScreen(mSelectedEntity->mPosition);
   Point2I redScreenPos = Link.Rendering.worldToScreen(mSelectedEntity->mPosition + Point3F(5, 0, 0));
   Point2I greenScreenPos = Link.Rendering.worldToScreen(mSelectedEntity->mPosition + Point3F(0, 5, 0));
   Point2I blueScreenPos = Link.Rendering.worldToScreen(mSelectedEntity->mPosition + Point3F(0, 0, 5));
   Point2I redScreenPosEnd = Link.Rendering.worldToScreen(mSelectedEntity->mPosition + Point3F(50, 0, 0));
   Point2I greenScreenPosEnd = Link.Rendering.worldToScreen(mSelectedEntity->mPosition + Point3F(0, 50, 0));
   Point2I blueScreenPosEnd = Link.Rendering.worldToScreen(mSelectedEntity->mPosition + Point3F(0, 0, 50));

   gizmoVisible = true;

   gizmoMinCoord = entityScreenPos;
   gizmoMinCoord.setMin(redScreenPosEnd);
   gizmoMinCoord.setMin(greenScreenPosEnd);
   gizmoMinCoord.setMin(blueScreenPosEnd);

   gizmoMaxCoord = entityScreenPos;
   gizmoMaxCoord.setMax(redScreenPosEnd);
   gizmoMaxCoord.setMax(greenScreenPosEnd);
   gizmoMaxCoord.setMax(blueScreenPosEnd);

   F32 entityDist = Point2I(entityScreenPos - event.mousePoint).len();
   F32 redDist = Point2I(redScreenPos - event.mousePoint).len();
   F32 greenDist = Point2I(greenScreenPos - event.mousePoint).len();
   F32 blueDist = Point2I(blueScreenPos - event.mousePoint).len();
   F32 redDistEnd = Point2I(redScreenPosEnd - event.mousePoint).len();
   F32 greenDistEnd = Point2I(greenScreenPosEnd - event.mousePoint).len();
   F32 blueDistEnd = Point2I(blueScreenPosEnd - event.mousePoint).len();

   if (( redDist < greenDist && redDist < blueDist ) || ( redDistEnd < greenDist && redDistEnd < blueDist ))
   {
      translateDirection.set(1.0f, 0.0f, 0.0f);
      mouseDirection.set(redScreenPos.x - entityScreenPos.x, redScreenPos.y - entityScreenPos.y);
      mouseDirection.normalize();
   }
   else if (( greenDist < redDist && greenDist < blueDist ) || ( greenDistEnd < redDist && greenDistEnd < blueDist ))
   {
      translateDirection.set(0.0f, 1.0f, 0.0f);
      mouseDirection.set(greenScreenPos.x - entityScreenPos.x, greenScreenPos.y - entityScreenPos.y);
      mouseDirection.normalize();
   }
   else if (( blueDist < redDist && blueDist < greenDist ) || ( blueDistEnd < redDist && blueDistEnd < greenDist ))
   {
      translateDirection.set(0.0f, 0.0f, 1.0f);
      mouseDirection.set(blueScreenPos.x - entityScreenPos.x, blueScreenPos.y - entityScreenPos.y);
      mouseDirection.normalize();
   }
}

void Gizmo::onMouseDraggedEvent(const GuiEvent &event)
{
   Point2F newDirection(event.mousePoint.x - lastMousePosition.x, event.mousePoint.y - lastMousePosition.y);
   F32 mouseDist = newDirection.len();
   newDirection.normalize();
   F32 dir_dist = Point2F(mouseDirection - newDirection).len();
   
   F32 dist = 0.0f;
   if ( dir_dist < 0.3f )
      dist = mouseDist;
   if ( dir_dist > 1.7f )
      dist = -mouseDist;

   if ( mSelectedEntity != NULL )
   {
      mSelectedEntity->mPosition += translateDirection * (dist);
      mSelectedEntity->refresh();
      lastMousePosition = event.mousePoint;
      //sceneEditor.selectEntity(mSelectedEntity);
   }
}