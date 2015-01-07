#include "SkyboxDLL.h"
#include <plugins/plugins_shared.h>

#include <sim/simObject.h>
#include <3d/rendering/common.h>
#include <graphics/utilities.h>

#include <bx/fpumath.h>

using namespace Plugins;
PluginLink Plugins::Link;

bool skyboxEnabled = false;
bgfx::TextureHandle skyboxTexture = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle skyboxShader = BGFX_INVALID_HANDLE;
bgfx::UniformHandle skyboxMatrixUniform = BGFX_INVALID_HANDLE;

S32 frameCount = 0;
U32 tickCount = 0;
char fpsStr[32];
char mspfStr[32];

// Called when the plugin is loaded.
void create(PluginLink _link)
{
   Link = _link;

   // Register Console Functions
   Link.Con.addCommand("Skybox", "enable", enableSkybox, "", 1, 1);
   Link.Con.addCommand("Skybox", "disable", disableSkybox, "", 1, 1);

   // Load skybox texture.
   TextureObject* texture_obj = Link.Graphics.loadTexture("textures/desertSky.dds", TextureHandle::TextureHandleType::BitmapKeepTexture, false, false, false);
   if ( texture_obj )
      skyboxTexture = texture_obj->getBGFXTexture();

   // Load skybox shader and uniform.
   Graphics::Shader* shader = Link.Graphics.getShader("shaders/skybox_vs.sc", "shaders/skybox_fs.sc");
   if ( shader )
   {
      skyboxShader = shader->mProgram;
      skyboxMatrixUniform = Link.Graphics.getUniformMatrix("u_mtx");
   }
}

// Console Functions
void enableSkybox(SimObject *obj, S32 argc, const char *argv[])
{
   skyboxEnabled = true;
}

void disableSkybox(SimObject *obj, S32 argc, const char *argv[])
{
   skyboxEnabled = false;
}

// FPS Counter
void processTick()
{
   tickCount++;
   if ( tickCount > 31 )
   {
      dSprintf(fpsStr, 256, "FPS: %d", frameCount);
      dSprintf(mspfStr, 256, "mspf: %f", 1000.0f / frameCount);
      frameCount = 0;
      tickCount = 0;
   }
}

// Per-Frame render function
void render()
{
   // FPS Counter
   frameCount++;
   Link.bgfx.dbgTextClear(0, false);
   Link.bgfx.dbgTextPrintf(1, 1, 0x4f, fpsStr);
   Link.bgfx.dbgTextPrintf(1, 2, 0x4f, mspfStr);

   if ( !skyboxEnabled || !bgfx::isValid(skyboxTexture) || !bgfx::isValid(skyboxShader) ) 
      return;

   // Deferred + Forward outputs to RenderLayer2 so RenderLayer 1 is under it which
   // is where we want our skybox to render.
   Link.bgfx.setViewClear(Graphics::ViewTable::RenderLayer1,
      BGFX_CLEAR_COLOR_BIT,
      0x0000ffff, // Blue for debugging.
      1.0f, 
      0);

   F32 proj[16];
   bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
   Link.bgfx.setViewTransform(Graphics::ViewTable::RenderLayer1, NULL, proj, BGFX_VIEW_STEREO, NULL);
   Link.bgfx.setViewRect(Graphics::ViewTable::RenderLayer1, 0, 0, *Link.Rendering.canvasWidth, *Link.Rendering.canvasHeight);

   // Calculate view matrix based on current view matrix.
   float viewMtx[16];
   bx::mtxInverse(viewMtx, Link.Rendering.viewMatrix);

   float rotMtx[16];
   bx::mtxRotateZ(rotMtx, 3.14f);

   float finalMtx[16];
   bx::mtxMul(finalMtx, viewMtx, rotMtx);

   finalMtx[0] = finalMtx[0] * -1;
   finalMtx[1] = finalMtx[1] * -1;
   finalMtx[2] = finalMtx[2] * -1;

   finalMtx[8] = finalMtx[8] * -1;
   finalMtx[9] = finalMtx[9] * -1;
   finalMtx[10] = finalMtx[10] * -1;

   Link.bgfx.setUniform(skyboxMatrixUniform, finalMtx, 1);

   Link.bgfx.setTexture(0, Link.Graphics.getTextureUniform(0), skyboxTexture, UINT32_MAX);
   Link.bgfx.setProgram(skyboxShader);
   Link.bgfx.setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE, 0);

   // Render skybox as fullscreen quad.
   Link.Graphics.fullScreenQuad(*Link.Rendering.canvasWidth, *Link.Rendering.canvasHeight);

   Link.bgfx.submit(Graphics::ViewTable::RenderLayer1, 0);
}