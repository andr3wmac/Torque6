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

#include "OceanPlugin.h"
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

IMPLEMENT_PLUGIN_CONOBJECT(EndlessOcean);

EndlessOcean::EndlessOcean()
   : mEnabled(false)
{
   mName = "EndlessOcean";
   mPriority = 4200;

   // Load Shader
   mShader.idx = bgfx::invalidHandle;
   Graphics::ShaderAsset* oceanShaderAsset = Link.Graphics.getShaderAsset("Ocean:OceanShader");
   if (oceanShaderAsset)
      mShader = oceanShaderAsset->getProgram();

   mView = Link.Graphics.getView("Ocean", 4200);
   mHeightTexturePath = Link.StringTableLink->insert("");
   mHeightTexture.idx = bgfx::invalidHandle;
   mNormalTexturePath = Link.StringTableLink->insert("");
   mNormalTexture.idx = bgfx::invalidHandle;
}

void EndlessOcean::initPersistFields()
{
   // Call parent.
   Parent::initPersistFields();
   addProtectedField("HeightMap", Plugins::Link.Con.TypeAssetLooseFilePath, Offset(mHeightTexturePath, EndlessOcean), &EndlessOcean::setHeightTexture, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
   addProtectedField("NormalMap", Plugins::Link.Con.TypeAssetLooseFilePath, Offset(mNormalTexturePath, EndlessOcean), &EndlessOcean::setNormalTexture, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
}

void EndlessOcean::loadHeightTexture(StringTableEntry path)
{
   // Load skybox texture.
   TextureObject* texture_obj = Link.Graphics.loadTexture(path, TextureHandle::BitmapKeepTexture, BGFX_TEXTURE_NONE, false, false);
   if (texture_obj)
   {
      mHeightTexture = texture_obj->getBGFXTexture();
      mHeightTexturePath = path;
   }
}

void EndlessOcean::loadNormalTexture(StringTableEntry path)
{
   // Load skybox texture.
   TextureObject* texture_obj = Link.Graphics.loadTexture(path, TextureHandle::BitmapKeepTexture, BGFX_TEXTURE_NONE, false, false);
   if (texture_obj)
   {
      mNormalTexture = texture_obj->getBGFXTexture();
      mNormalTexturePath = path;
   }
}

void EndlessOcean::onActivate()
{
   Parent::onActivate();
   mEnabled = true;
}

void EndlessOcean::onDeactivate()
{
   Parent::onDeactivate();
   mEnabled = false;
}

void EndlessOcean::render()
{
   if (!mEnabled || !bgfx::isValid(mHeightTexture) || !bgfx::isValid(mNormalTexture) || !bgfx::isValid(mShader))
      return;

   F32 proj[16];
   bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
   Link.bgfx.setViewTransform(mView->id, NULL, proj, BGFX_VIEW_STEREO, NULL);
   Link.bgfx.setViewRect(mView->id, 0, 0, *Link.Rendering.canvasWidth, *Link.Rendering.canvasHeight);
   Link.bgfx.setViewFrameBuffer(mView->id, Link.Rendering.getPostTarget());

   Link.bgfx.setTextureA(0, Link.Graphics.getTextureUniform(0), Link.Rendering.getPostSource(), 0, UINT32_MAX);
   Link.bgfx.setTexture(1, Link.Graphics.getTextureUniform(1), Link.Rendering.getDepthTexture(), UINT32_MAX);
   Link.bgfx.setTexture(2, Link.Graphics.getTextureUniform(2), mHeightTexture, UINT32_MAX);
   Link.bgfx.setTexture(3, Link.Graphics.getTextureUniform(3), mNormalTexture, UINT32_MAX);

   Link.bgfx.setState(0
      | BGFX_STATE_RGB_WRITE
      | BGFX_STATE_ALPHA_WRITE
      | BGFX_STATE_DEPTH_TEST_LESS
      , 0);

   // Render skybox as fullscreen quad.
   Link.Graphics.fullScreenQuad(*Link.Rendering.canvasWidth, *Link.Rendering.canvasHeight, 0.0f);

   Link.bgfx.submit(mView->id, mShader, 0);
}