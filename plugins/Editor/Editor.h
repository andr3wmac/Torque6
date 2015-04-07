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

#ifndef _PLUGINS_SHARED_H
#include <plugins/plugins_shared.h>
#endif

#ifndef _SIM_OBJECT_H_
#include <sim/simObject.h>
#endif

#ifndef _EDITOR_PLUGIN_API_
#include "Editor_pluginAPI.h"
#endif

extern "C" 
{
   PLUGIN_FUNC void create();
   PLUGIN_FUNC void processTick();
   PLUGIN_FUNC void render();
}

// Editor API lets other plugins use this plugin.
extern EditorAPI editorAPI;
extern Vector<EditorBase*> editorList;
extern S32 activeEditorIndex;
void addEditor(EditorBase* editor);
void switchEditor(S32 id);

extern SimGroup*              sceneGroup;
extern bool                   editorOpen;

// Editor Camera
class EditorCamera : public Scene::SceneCamera
{
   private:
      typedef Scene::SceneCamera Parent;

   public:
      EditorCamera();

      void onMouseMoveEvent(const GuiEvent &event);
      void onMouseDownEvent(const GuiEvent &event);
      void onMouseDraggedEvent(const GuiEvent &event);

      DECLARE_PLUGIN_CONOBJECT(EditorCamera);
};
extern EditorCamera mCamera;

// Editor GUI
extern bool                   loadedGUI;
extern S32                    mainEditorArea;

void loadGUI();
void openEditor(SimObject *obj, S32 argc, const char *argv[]);
void closeEditor(SimObject *obj, S32 argc, const char *argv[]);
void deleteKey(SimObject *obj, S32 argc, const char *argv[]);