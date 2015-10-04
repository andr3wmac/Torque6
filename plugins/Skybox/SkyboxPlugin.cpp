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

#include "SkyboxPlugin.h"
#include <plugins/plugins_shared.h>

#include <sim/simObject.h>
#include <3d/rendering/common.h>
#include <graphics/core.h>

#include <bx/fpumath.h>

using namespace Plugins;

// Called when the plugin is loaded.
void create()
{
   //
}

IMPLEMENT_PLUGIN_CONOBJECT(SimpleSkybox);

SimpleSkybox::SimpleSkybox()
   : mEnabled(false)
{
   mName = "SimpleSkybox";

   // Load Shader
   mShader = BGFX_INVALID_HANDLE;
   Graphics::ShaderAsset* skyboxShaderAsset = Link.Graphics.getShaderAsset("Skybox:skyboxShader");
   if (skyboxShaderAsset)
   {
      mShader = skyboxShaderAsset->getProgram();
      mMatrixUniform = Link.Graphics.getUniformMat4("u_mtx", 1);
   }

   mView = Link.Graphics.getView("RenderLayer1", 2000);
   mTexturePath = Link.StringTableLink->insert("");
   mTexture = BGFX_INVALID_HANDLE;
}

void SimpleSkybox::initPersistFields()
{
   // Call parent.
   Parent::initPersistFields();

   addProtectedField("Texture", Plugins::Link.Con.TypeAssetLooseFilePath, Offset(mTexturePath, SimpleSkybox), &SimpleSkybox::setTexture, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
}

void SimpleSkybox::loadTexture(StringTableEntry path)
{
   // Load skybox texture.
   TextureObject* texture_obj = Link.Graphics.loadTexture(path, TextureHandle::BitmapKeepTexture, BGFX_TEXTURE_NONE, false, false);
   if (texture_obj)
   {
      mTexture = texture_obj->getBGFXTexture();
      mTexturePath = path;
   }
}

void SimpleSkybox::onActivate()
{
   Parent::onActivate();
   mEnabled = true;
}

void SimpleSkybox::onDeactivate()
{
   Parent::onDeactivate();
   mEnabled = false;
}

void SimpleSkybox::preRender()
{

}

void SimpleSkybox::render()
{
   if (!mEnabled || !bgfx::isValid(mTexture) || !bgfx::isValid(mShader))
      return;

   F32 proj[16];
   bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1000.0f);
   Link.bgfx.setViewTransform(mView->id, NULL, proj, BGFX_VIEW_STEREO, NULL);
   Link.bgfx.setViewRect(mView->id, 0, 0, *Link.Rendering.canvasWidth, *Link.Rendering.canvasHeight);

   // Calculate view matrix based on current view matrix.
   float viewMtx[16];
   bx::mtxInverse(viewMtx, Link.Rendering.viewMatrix);
   Link.bgfx.setUniform(mMatrixUniform, viewMtx, 1);

   Link.bgfx.setTexture(0, Link.Graphics.getTextureUniform(0), mTexture, UINT32_MAX);
   Link.bgfx.setState(0
      | BGFX_STATE_RGB_WRITE
      | BGFX_STATE_ALPHA_WRITE
      | BGFX_STATE_DEPTH_TEST_LESS
      , 0);

   // Render skybox as fullscreen quad.
   Link.Graphics.fullScreenQuad(*Link.Rendering.canvasWidth, *Link.Rendering.canvasHeight, 999.999f);

   Link.bgfx.submit(mView->id, mShader, 0);
}

void SimpleSkybox::postRender()
{

}