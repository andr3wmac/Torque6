#ifndef _PLUGINS_SHARED_H
#include <plugins/plugins_shared.h>
#endif

#ifndef _SIM_OBJECT_H_
#include <sim/simObject.h>
#endif

extern "C" 
{
   PLUGIN_FUNC void create(Plugins::PluginLink _link);
   PLUGIN_FUNC void processTick();
   PLUGIN_FUNC void render();
   PLUGIN_FUNC void preRender();
}

extern SimGroup*              sceneGroup;
extern Scene::SceneCamera*    editorCamera;
extern bool                   editorOpen;

// Editor GUI
extern bool                   loadedGUI;
extern S32                    mainEditorArea;
extern S32                    sceneEditorArea;
extern S32                    sceneOverviewArea;
extern S32                    sceneOverviewList;
extern S32                    entityInspectorArea;
extern Vector<S32>            entityInspectorElements;

extern Rendering::RenderData* cubeRenderData;
extern F32                    cubeMtx[16];
extern bgfx::TextureHandle    cubeTexture;
extern Vector<Rendering::TextureData> cubeTextureData;

extern S32 myLabel;
extern U32 tickCount;
extern S32 frameCount;
extern char buf[256];

void loadGUI();
void openEditor(SimObject *obj, S32 argc, const char *argv[]);
void closeEditor(SimObject *obj, S32 argc, const char *argv[]);

void testButtonFunction();

void selectEntity(Scene::SceneEntity* entity);