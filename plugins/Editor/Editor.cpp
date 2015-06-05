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

#include "Editor.h"
#include <sim/simObject.h>
#include <plugins/plugins_shared.h>
#include <3d/rendering/common.h>
#include <graphics/core.h>
#include <bx/fpumath.h>

#include "SceneEditor/SceneEditor.h"
#include "MaterialEditor/MaterialEditor.h"

using namespace Plugins;

EditorCamera mCamera;
EditorAPI editorAPI;
Vector<EditorBase*> editorList;
S32 activeEditorIndex = -1;

bool editorOpen = false;
bool loadedGUI = false;

S32 mainEditorArea = -1;

void create()
{
   addEditor(&sceneEditor);
   addEditor(&matEditor);
   activeEditorIndex = 0;

   // Register Console Function
   Link.Con.addCommand("Editor", "open", openEditor, "", 1, 1);
   Link.Con.addCommand("Editor", "close", closeEditor, "", 1, 1);
   Link.Con.addCommand("Editor", "deleteKey", deleteKey, "", 1, 1);

   dStrcpy(editorAPI.pluginName, "Editor");
   editorAPI.addEditor = addEditor;
   Link.addPluginAPI(&editorAPI);

   mCamera.setBindMouse(true, false, true);
   Link.Scene.addCamera("EditorCamera", &mCamera);
}

void loadGUI()
{
   // Main Dialog
   mainEditorArea = Link.SysGUI.beginScrollArea("Torque 6 Editor", 5, 5, 300, 150);
   Link.SysGUI.separator();

   for ( U32 n = 0; n < editorList.size(); ++n )
   {
      Link.SysGUI.button(editorList[n]->name, "", switchEditor);
   }

   Link.SysGUI.separator();
   Link.SysGUI.endScrollArea();

   loadedGUI = true;
}

// Console Function
void openEditor(SimObject *obj, S32 argc, const char *argv[])
{
   if ( !loadedGUI ) loadGUI();

   editorOpen = true;
   Link.SysGUI.setElementHidden(mainEditorArea, false);

   Link.Physics.pause();
   editorList[activeEditorIndex]->enable();

   Scene::SceneCamera* activeCam = Link.Scene.getActiveCamera();
   mCamera.copy(activeCam);
   Link.Scene.pushActiveCamera("EditorCamera");
}

void closeEditor(SimObject *obj, S32 argc, const char *argv[])
{
   editorOpen = false;
   Link.SysGUI.setElementHidden(mainEditorArea, true);

   Link.Physics.resume();
   editorList[activeEditorIndex]->disable();
   Link.Scene.popActiveCamera();
}

void deleteKey(SimObject *obj, S32 argc, const char *argv[])
{
   if ( !editorOpen ) return;

   editorList[activeEditorIndex]->deleteKey();
}

// Call Every Tick.
void processTick()
{
   if ( !editorOpen ) return;
}

void render()
{
   if ( !editorOpen ) return;

   editorList[activeEditorIndex]->render();
}

void addEditor(EditorBase* editor)
{
   editorList.push_back(editor);
}

void switchEditor(S32 id)
{
   const char* editorName = Link.SysGUI.getLabelValue(id);
   for ( U32 n = 0; n < editorList.size(); ++n )
   {
      if ( dStrcmp(editorList[n]->name, editorName) == 0 )
      {
         activeEditorIndex = n;
         editorList[n]->enable();
      }
      else
         editorList[n]->disable();
   }
}

IMPLEMENT_PLUGIN_CONOBJECT(EditorCamera);

EditorCamera::EditorCamera()
{

}

void EditorCamera::onMouseMoveEvent(const GuiEvent &event)
{
   editorList[activeEditorIndex]->onMouseMoveEvent(event);
}

void EditorCamera::onMouseDownEvent(const GuiEvent &event)
{
   editorList[activeEditorIndex]->onMouseDownEvent(event);
}

void EditorCamera::onMouseDraggedEvent(const GuiEvent &event)
{
   editorList[activeEditorIndex]->onMouseDraggedEvent(event);
}