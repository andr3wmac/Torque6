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

#include "EditorDLL.h"
#include <sim/simObject.h>
#include <plugins/plugins_shared.h>
#include <3d/rendering/common.h>
#include <graphics/utilities.h>
#include <bx/fpumath.h>

using namespace Plugins;

SimGroup* sceneGroup = NULL;
EditorCamera editorCamera;
bool editorOpen = false;
bool loadedGUI = false;

S32 mainEditorArea = -1;
S32 sceneEditorArea = -1;
S32 sceneOverviewArea = -1;
S32 sceneOverviewList = -1;
S32 entityInspectorArea = -1;
Vector<S32> entityInspectorElements;

Rendering::RenderData* cubeRenderData = NULL;
bgfx::TextureHandle cubeTexture = BGFX_INVALID_HANDLE;
Vector<Rendering::TextureData> cubeTextureData;
F32 cubeMtx[16];
Scene::SceneEntity* selectedEntity = NULL;

S32 myLabel = 0;
S32 frameCount = 0;
U32 tickCount = 0;
char buf[256];

IMPLEMENT_PLUGIN_CONOBJECT(EditorCamera);

EditorCamera::EditorCamera()
{
   Con::printf("EDITOR CAM CREATED!");
   lastMousePosition.set(0, 0);
   mouseDirection.set(0.0f, 0.0f);
   translateDirection.set(0.0f, 0.0f, 0.0f);
}

void EditorCamera::onMouseDownEvent(const GuiEvent &event)
{
   Point3F world_ray = Link.Rendering.screenToWorld(event.mousePoint);

   Scene::SceneEntity* hit_entity = NULL;
   if ( selectedEntity != NULL )
   {
      if ( selectedEntity->mBoundingBox.collideLine(mPosition, mPosition + (world_ray * 1000.0f)) )
         hit_entity = selectedEntity;
   } 

   if ( hit_entity == NULL )
      hit_entity = Link.Scene.raycast(mPosition, mPosition + (world_ray * 1000.0f));
   
   if ( hit_entity )
   {
      translateDirection.set(0,0,0);
      lastMousePosition = event.mousePoint;

      Con::printf("HIT ENTITY: %s", hit_entity->getName());
      if ( selectedEntity == hit_entity ) 
      {
         Point2I entityScreenPos = Link.Rendering.worldToScreen(selectedEntity->mPosition);
         Point2I redScreenPos = Link.Rendering.worldToScreen(selectedEntity->mPosition + Point3F(50, 0, 0));
         Point2I greenScreenPos = Link.Rendering.worldToScreen(selectedEntity->mPosition + Point3F(0, 50, 0));
         Point2I blueScreenPos = Link.Rendering.worldToScreen(selectedEntity->mPosition + Point3F(0, 0, 50));

         F32 entityDist = Point2I(entityScreenPos - event.mousePoint).len();
         F32 redDist = Point2I(redScreenPos - event.mousePoint).len();
         F32 greenDist = Point2I(greenScreenPos - event.mousePoint).len();
         F32 blueDist = Point2I(blueScreenPos - event.mousePoint).len();

         if ( redDist < greenDist && redDist < blueDist )
         {
            translateDirection.set(1.0f, 0.0f, 0.0f);
            mouseDirection.set(redScreenPos.x - entityScreenPos.x, redScreenPos.y - entityScreenPos.y);
            mouseDirection.normalize();
         }
         else if ( greenDist < redDist && greenDist < blueDist )
         {
            translateDirection.set(0.0f, 1.0f, 0.0f);
            mouseDirection.set(greenScreenPos.x - entityScreenPos.x, greenScreenPos.y - entityScreenPos.y);
            mouseDirection.normalize();
         }
         else if ( blueDist < redDist && blueDist < greenDist )
         {
            translateDirection.set(0.0f, 0.0f, 1.0f);
            mouseDirection.set(blueScreenPos.x - entityScreenPos.x, blueScreenPos.y - entityScreenPos.y);
            mouseDirection.normalize();
         }

         Link.Con.printf("Entity: %f R: %f G: %f B: %f", entityDist, redDist, greenDist, blueDist);
      } else {
         selectEntity(hit_entity);
      }
   }
}

void EditorCamera::onMouseDraggedEvent(const GuiEvent &event)
{
   Point2F newDirection(event.mousePoint.x - lastMousePosition.x, event.mousePoint.y - lastMousePosition.y);
   F32 mouseDist = newDirection.len();
   newDirection.normalize();
   //Con::printf("Target Dir: %f %f Current Dir: %f %f", mouseDirection.x, mouseDirection.y, newDirection.x, newDirection.y);
   F32 dir_dist = Point2F(mouseDirection - newDirection).len();
   
   F32 dist = 0.0f;
   if ( dir_dist < 0.3f )
      dist = mouseDist;
   if ( dir_dist > 1.7f )
      dist = -mouseDist;

   if ( selectedEntity != NULL )
   {
      selectedEntity->mPosition += translateDirection * (dist);
      selectedEntity->refresh();
      lastMousePosition = event.mousePoint;
      selectEntity(selectedEntity);
   }
}

void create()
{
   // Register Console Function
   Link.Con.addCommand("Editor", "open", openEditor, "", 1, 1);
   Link.Con.addCommand("Editor", "close", closeEditor, "", 1, 1);
}

void loadGUI()
{
   // Main Dialog
   mainEditorArea = Link.SysGUI.beginScrollArea("Torque 6 Editor", 5, 5, 200, 150);
   Link.SysGUI.separator();
   Link.SysGUI.button("Scene Editor", "", NULL);
   Link.SysGUI.button("Entity Editor", "", NULL);
   Link.SysGUI.button("Asset Browser", "", NULL);
   Link.SysGUI.separator();
   myLabel = Link.SysGUI.label("FPS: ");
   Link.SysGUI.endScrollArea();

   // Scene Editor
   sceneEditorArea = Link.SysGUI.beginScrollArea("Scene Editor", 5, 160, 200, 400);
   Link.SysGUI.separator();
   Link.SysGUI.button("Load Scene", "", testButtonFunction);
   Link.SysGUI.button("Save Scene", "", NULL);
   Link.SysGUI.separator();
   Link.SysGUI.label("Camera Settings");
   Link.SysGUI.separator();
   Link.SysGUI.slider("Speed", 50, 0, 100);
   Link.SysGUI.button("Return to Center", "", NULL);
   Link.SysGUI.separator();
   Link.SysGUI.endScrollArea();
   
   // Scene Overview
   sceneOverviewArea = Link.SysGUI.beginScrollArea("Scene Overview", 5, 5, 200, 300);
   Link.SysGUI.separator();
   sceneOverviewList = Link.SysGUI.list("", NULL);
   Link.SysGUI.endScrollArea();
   Link.SysGUI.alignRight(sceneOverviewArea);

   // Entity Inspector
   entityInspectorArea = Link.SysGUI.beginScrollArea("Entity Inspector", 5, 310, 200, 400);
   Link.SysGUI.alignRight(entityInspectorArea);
   Link.SysGUI.endScrollArea();

   // Editor Camera
   //editorCamera = Link.Scene.getCamera("EditorCamera");
   //editorCamera->setBindMouse(true, false, true);
   editorCamera.setBindMouse(true, false, true);
   Link.Scene.addCamera("EditorCamera", &editorCamera);

   //cubeRenderData = Link.Rendering.createRenderData();
   cubeRenderData = new Rendering::RenderData();
   cubeRenderData->indexBuffer = *Link.Graphics.cubeIB;
   cubeRenderData->vertexBuffer = *Link.Graphics.cubeVB;

   Graphics::ShaderAsset* boxShaderAsset = Link.Graphics.getShaderAsset("Editor:boxShader");
   if ( boxShaderAsset )
      cubeRenderData->shader = boxShaderAsset->getProgram();

   bx::mtxIdentity(cubeMtx);
   bx::mtxRotateX(cubeMtx, 0);
   cubeRenderData->transformTable = cubeMtx;
   cubeRenderData->transformCount = 1;
   cubeRenderData->view = Graphics::ViewTable::RenderLayer4;
   cubeRenderData->state = 0
			| BGFX_STATE_RGB_WRITE
			| BGFX_STATE_ALPHA_WRITE
         | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);

   loadedGUI = true;
}

// Console Function
void openEditor(SimObject *obj, S32 argc, const char *argv[])
{
   if ( !loadedGUI ) loadGUI();

   editorOpen = true;
   Link.SysGUI.setElementHidden(mainEditorArea, false);
   Link.SysGUI.setElementHidden(sceneEditorArea, false);
   Link.SysGUI.setElementHidden(sceneOverviewArea, false);
   Link.Scene.pushActiveCamera("EditorCamera");

   sceneGroup = Link.Scene.getEntityGroup();
}

void closeEditor(SimObject *obj, S32 argc, const char *argv[])
{
   editorOpen = false;
   Link.SysGUI.setElementHidden(mainEditorArea, true);
   Link.SysGUI.setElementHidden(sceneEditorArea, true);
   Link.SysGUI.setElementHidden(sceneOverviewArea, true);
   Link.Scene.popActiveCamera();
}

// Call Every Tick.
void processTick()
{
   if ( !editorOpen ) return;

   tickCount++;
   if ( tickCount > 31 )
   {
      dSprintf(buf, 256, "FPS: %d", frameCount);
      Link.SysGUI.setLabelValue(myLabel, buf);
      frameCount = 0;
      tickCount = 0;
   }

   if ( sceneGroup != NULL )
   {
      Link.SysGUI.clearList(sceneOverviewList);
      for(U32 n = 0; n < sceneGroup->size(); ++n)
      {
         SimObject* obj = sceneGroup->at(n);
         Link.SysGUI.addListValue(sceneOverviewList, obj->getName(), "", testButtonFunction);
      }
   }
}

void preRender()
{
   refreshSelectionBox();
   //Link.bgfx.setViewTransform(Graphics::ViewTable::RenderLayer4, Link.Rendering.viewMatrix, Link.Rendering.projectionMatrix, BGFX_VIEW_STEREO, NULL);
   //Link.bgfx.setViewRect(Graphics::ViewTable::RenderLayer4, 0, 0, *Link.Rendering.canvasWidth, *Link.Rendering.canvasHeight);
}

// Per-Frame render function
void render()
{
   if ( !editorOpen ) return;

   frameCount++;

   if ( selectedEntity != NULL )
   {
      Link.Graphics.drawLine3D(selectedEntity->mPosition, selectedEntity->mPosition + Point3F(0, 50, 0), ColorI(0, 255, 0, 255), 5.0f);
      Link.Graphics.drawLine3D(selectedEntity->mPosition, selectedEntity->mPosition + Point3F(50, 0, 0), ColorI(255, 0, 0, 255), 5.0f);
      Link.Graphics.drawLine3D(selectedEntity->mPosition, selectedEntity->mPosition + Point3F(0, 0, 50), ColorI(0, 0, 255, 255), 5.0f);
   
      Link.Graphics.drawBox3D(selectedEntity->mBoundingBox, ColorI(255, 255, 255, 255), 2.0f);
   }

   // Calls straight to bgfx
   //Link.bgfx.dbgTextClear(0, false);
   //Link.bgfx.dbgTextPrintf(1, 1, 0x4f, buf);
}

void testButtonFunction()
{
   Link.Con.printf("Button Pressed!");

   S32 selected_item = Link.SysGUI.getListSelected(sceneOverviewList);
   if ( selected_item > -1 )
   {
      Scene::SceneEntity* entity = dynamic_cast<Scene::SceneEntity*>(sceneGroup->at(selected_item));
      if ( entity )
         selectEntity(entity);
   }
}

void selectEntity(Scene::SceneEntity* entity)
{
   Link.Con.printf("Selected Scene Entity: %s", entity->getName());

   selectedEntity = entity;

   Link.SysGUI.clearScrollArea(entityInspectorArea);
   Link.SysGUI.seek(entityInspectorArea);

   AbstractClassRep::FieldList fieldList = entity->getFieldList();
   for(Vector<AbstractClassRep::Field>::iterator itr = fieldList.begin(); itr != fieldList.end(); itr++)
   {
      const AbstractClassRep::Field* f = itr;
      if( f->type == AbstractClassRep::DepricatedFieldType ||
            f->type == AbstractClassRep::StartGroupFieldType ||
            f->type == AbstractClassRep::EndGroupFieldType) 
         continue;

      for(U32 j = 0; S32(j) < f->elementCount; j++)
      {                      
         const char *val = (*f->getDataFn)( entity, Link.Con.getData(f->type, (void *) (((const char *)entity) + f->offset), j, f->table, f->flag) );

         if( !val )
            continue;

         Link.Con.printf("Name: %s Value: %s", f->pFieldname, val);

         if ( f->type ==  Link.Con.TypeBool )
            Link.SysGUI.checkBox(f->pFieldname, dAtob(val));
         else if ( f->type ==  Link.Con.TypeString )
            Link.SysGUI.textInput(f->pFieldname, val);
         else if ( f->type ==  Link.Con.TypePoint3F )
            Link.SysGUI.textInput(f->pFieldname, val);
         //else
         //   Link.SysGUI.label(f->pFieldname);
      }
   }

   Link.SysGUI.clearSeek();
}

void refreshSelectionBox()
{
   if ( !selectedEntity ) return;

   Point3F boundingBoxSize = (selectedEntity->mBoundingBox.maxExtents - selectedEntity->mBoundingBox.minExtents) / 2;

   F32 mtxWorldScale[16];
   bx::mtxScale(mtxWorldScale, 
      selectedEntity->mScale.x * boundingBoxSize.x, 
      selectedEntity->mScale.y * boundingBoxSize.y, 
      selectedEntity->mScale.z * boundingBoxSize.z
      );

   F32 mtxWorldRotate[16];
   bx::mtxRotateXYZ(mtxWorldRotate, selectedEntity->mRotation.x, selectedEntity->mRotation.y, selectedEntity->mRotation.z);

   bx::mtxMul(cubeMtx, mtxWorldScale, mtxWorldRotate );
   cubeMtx[12] = selectedEntity->mBoundingBox.minExtents.x + boundingBoxSize.x;
   cubeMtx[13] = selectedEntity->mBoundingBox.minExtents.y + boundingBoxSize.y;
   cubeMtx[14] = selectedEntity->mBoundingBox.minExtents.z + boundingBoxSize.z;
}