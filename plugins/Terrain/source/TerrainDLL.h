#ifndef _PLUGINS_SHARED_H
#include <plugins/plugins_shared.h>
#endif

#ifndef _SIM_OBJECT_H_
#include <sim/simObject.h>
#endif

#include "TerrainBuilder.h"

extern "C" 
{
   PLUGIN_FUNC void create(Plugins::PluginLink _link);
   PLUGIN_FUNC void destroy();
}

extern bool                      terrainEnabled;
extern bgfx::ProgramHandle       terrainShader;
extern Rendering::RenderData*    terrainRenderData;
extern bgfx::VertexBufferHandle  terrainVB;
extern bgfx::IndexBufferHandle   terrainIB;
extern TerrainBuilder*           terrainBuilder;

void loadHeightMap(SimObject *obj, S32 argc, const char *argv[]);
void enableTerrain(SimObject *obj, S32 argc, const char *argv[]);
void disableTerrain(SimObject *obj, S32 argc, const char *argv[]);
void refresh();