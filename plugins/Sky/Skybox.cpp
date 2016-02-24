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

#include "Skybox.h"
#include "plugins/plugins_shared.h"

#include "sim/simObject.h"
#include "rendering/rendering.h"
#include "rendering/renderCamera.h"
#include "graphics/core.h"
#include "scene/components/cameraComponent.h"

#include <bx/fpumath.h>

// Called when the plugin is loaded.
void create()
{
   //
}

IMPLEMENT_PLUGIN_CONOBJECT(Skybox);

Skybox::Skybox()
   : mEnabled(false)
{
   //mName = "Skybox";

   // Load Shader
   mShader.idx = bgfx::invalidHandle;
   Graphics::ShaderAsset* skyboxShaderAsset = Torque::Graphics.getShaderAsset("Sky:skyboxShader");
   if (skyboxShaderAsset)
   {
      mShader        = skyboxShaderAsset->getProgram();
      mMatrixUniform = Torque::Graphics.getUniformMat4("u_mtx", 1);
   }

   mView          = NULL;
   mTexturePath   = Torque::StringTableLink->insert("");
   mTexture.idx   = bgfx::invalidHandle;
}

void Skybox::initPersistFields()
{
   // Call parent.
   Parent::initPersistFields();

   addProtectedField("Texture", Torque::Con.TypeAssetLooseFilePath, Offset(mTexturePath, Skybox), &Skybox::setTexture, &defaultProtectedGetFn, &defaultProtectedWriteFn, "", AbstractClassRep::TextureAssetField);
}

void Skybox::loadTexture(StringTableEntry path)
{
   // Load skybox texture.
   TextureObject* texture_obj = Torque::Graphics.loadTexture(path, TextureHandle::BitmapKeepTexture, BGFX_TEXTURE_NONE, false, false);
   if (texture_obj)
   {
      mTexture = texture_obj->getBGFXTexture();
      mTexturePath = path;
   }
}

void Skybox::onAddToScene()
{
   Scene::CameraComponent* camera = mOwnerObject->findComponentByType<Scene::CameraComponent>();
   if (!camera)
      return;

   Torque::Rendering.addRenderHook(this);
   mEnabled = true;  
}

void Skybox::onRemoveFromScene()
{
   mEnabled = false;
   Torque::Rendering.removeRenderHook(this);
}

void Skybox::preRender(Rendering::RenderCamera* camera)
{
   mView = Torque::Graphics.getView("Skybox", 2010, camera);
}

void Skybox::render(Rendering::RenderCamera* camera)
{
   if (!mEnabled || !bgfx::isValid(mTexture) || !bgfx::isValid(mShader))
      return;

   F32 proj[16];
   bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1000.0f);
   Torque::bgfx.setViewFrameBuffer(mView->id, camera->getBackBuffer());
   Torque::bgfx.setViewTransform(mView->id, NULL, proj, BGFX_VIEW_STEREO, NULL);
   Torque::bgfx.setViewRect(mView->id, 0, 0, (U16)(*Torque::Rendering.canvasWidth), (U16)(*Torque::Rendering.canvasHeight));

   // Calculate view matrix based on current view matrix.
   float viewMtx[16];
   bx::mtxInverse(viewMtx, camera->viewMatrix);
   Torque::bgfx.setUniform(mMatrixUniform, viewMtx, 1);

   Torque::bgfx.setTexture(0, Torque::Graphics.getTextureUniform(0), mTexture, UINT32_MAX);
   Torque::bgfx.setState(0
      | BGFX_STATE_RGB_WRITE
      | BGFX_STATE_ALPHA_WRITE
      | BGFX_STATE_DEPTH_TEST_LESS
      , 0);

   // Render skybox as fullscreen quad.
   Torque::Graphics.fullScreenQuad((F32)(*Torque::Rendering.canvasWidth), (F32)(*Torque::Rendering.canvasHeight), 999.999f);

   Torque::bgfx.submit(mView->id, mShader, 0);
}

void Skybox::postRender(Rendering::RenderCamera* camera)
{

}