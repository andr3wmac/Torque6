#include "EditorDLL.h"
#include <sim/simObject.h>
#include <plugins/plugins_shared.h>
#include <3d/rendering/common.h>
#include <graphics/utilities.h>
#include <bx/fpumath.h>

using namespace Plugins;
PluginLink Plugins::Link;

SimGroup* sceneGroup = NULL;
Scene::SceneCamera* editorCamera = NULL;
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

S32 myLabel = 0;
S32 frameCount = 0;
U32 tickCount = 0;
char buf[256];

void create(PluginLink _link)
{
   Link = _link;

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
   editorCamera = Link.Scene.getCamera("EditorCamera");
   editorCamera->setBindMouse(true, false, true);

   cubeRenderData = Link.Rendering.createRenderData();
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
   Link.bgfx.setViewTransform(Graphics::ViewTable::RenderLayer4, Link.Rendering.viewMatrix, Link.Rendering.projectionMatrix, BGFX_VIEW_STEREO, NULL);
   Link.bgfx.setViewRect(Graphics::ViewTable::RenderLayer4, 0, 0, *Link.Rendering.canvasWidth, *Link.Rendering.canvasHeight);
}

// Per-Frame render function
void render()
{
   if ( !editorOpen ) return;

   frameCount++;

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

   Point3F boundingBoxSize = (entity->mBoundingBox.mMax - entity->mBoundingBox.mMin) / 2;

   F32 mtxWorldScale[16];
   bx::mtxScale(mtxWorldScale, 
      entity->mScale.x * boundingBoxSize.x, 
      entity->mScale.y * boundingBoxSize.y, 
      entity->mScale.z * boundingBoxSize.z
      );

   F32 mtxWorldRotate[16];
   bx::mtxRotateXYZ(mtxWorldRotate, entity->mRotation.x, entity->mRotation.y, entity->mRotation.z);

   bx::mtxMul(cubeMtx, mtxWorldScale, mtxWorldRotate );
   cubeMtx[12] = entity->mBoundingBox.mMin.x + boundingBoxSize.x;
   cubeMtx[13] = entity->mBoundingBox.mMin.y + boundingBoxSize.y;
   cubeMtx[14] = entity->mBoundingBox.mMin.z + boundingBoxSize.z;

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