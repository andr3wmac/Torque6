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

extern "C" 
{
   PLUGIN_FUNC void create();
   PLUGIN_FUNC void processTick();
   PLUGIN_FUNC void render();
   PLUGIN_FUNC void preRender();
}

class EditorCamera : public Scene::SceneCamera
{
   private:
      typedef Scene::SceneCamera Parent;

      Point2I lastMousePosition;
      Point3F translateDirection;
      Point2F mouseDirection;

   public:
      EditorCamera();;

      void onMouseDownEvent(const GuiEvent &event);
      void onMouseDraggedEvent(const GuiEvent &event);

      DECLARE_PLUGIN_CONOBJECT(EditorCamera);
};

extern SimGroup*              sceneGroup;
extern EditorCamera           editorCamera;
extern bool                   editorOpen;

// Editor GUI
extern bool                   loadedGUI;
extern S32                    mainEditorArea;
extern S32                    sceneEditorArea;
extern S32                    sceneOverviewArea;
extern S32                    sceneOverviewList;
extern S32                    entityInspectorArea;
extern Vector<S32>            entityInspectorElements;
extern Scene::SceneEntity*    selectedEntity;

extern Rendering::RenderData* cubeRenderData;
extern F32                    cubeMtx[16];
extern bgfx::TextureHandle    cubeTexture;
extern Vector<Rendering::TextureData> cubeTextureData;

extern S32 myLabel;
extern U32 tickCount;
extern S32 frameCount;
extern char buf[256];

void refreshSelectionBox();
void loadGUI();
void openEditor(SimObject *obj, S32 argc, const char *argv[]);
void closeEditor(SimObject *obj, S32 argc, const char *argv[]);

void testButtonFunction();

void selectEntity(Scene::SceneEntity* entity);