#ifndef _PLUGINS_SHARED_H
#include <plugins/plugins_shared.h>
#endif

#ifndef _SIM_OBJECT_H_
#include <sim/simObject.h>
#endif

extern "C" 
{
   PLUGIN_FUNC void create(Plugins::PluginLink _link);
   PLUGIN_FUNC void render();
}

extern bool skyboxEnabled;
extern bgfx::TextureHandle skyboxTexture;
extern bgfx::ProgramHandle skyboxShader;
extern bgfx::UniformHandle skyboxMatrixUniform;

void enableSkybox(SimObject *obj, S32 argc, const char *argv[]);
void disableSkybox(SimObject *obj, S32 argc, const char *argv[]);