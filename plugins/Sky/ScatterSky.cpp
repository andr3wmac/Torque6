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

#include "ScatterSky.h"
#include <plugins/plugins_shared.h>

#include <sim/simObject.h>
#include <3d/rendering/common.h>
#include <graphics/core.h>

#include <bx/fpumath.h>

using namespace Plugins;

IMPLEMENT_PLUGIN_CONOBJECT(ScatterSky);

ScatterSky::ScatterSky()
   : mEnabled(false),
	  mSkyCubeReady(false)
{
   mName = "Scatter Sky";

   // Load Skybox Shader
   mShader.idx = bgfx::invalidHandle;
   Graphics::ShaderAsset* skyboxShaderAsset = Link.Graphics.getShaderAsset("Sky:skyboxShader");
   if (skyboxShaderAsset)
   {
      mShader = skyboxShaderAsset->getProgram();
      mMatrixUniform = Link.Graphics.getUniformMat4("u_mtx", 1);
   }

	// Load Scatter Sky Shader
	mGenerateSkyCubeShader.idx = bgfx::invalidHandle;
	Graphics::ShaderAsset* scatterSkyShaderAsset = Link.Graphics.getShaderAsset("Sky:scatterSkyShader");
	if (scatterSkyShaderAsset)
	{
		mGenerateSkyCubeShader = scatterSkyShaderAsset->getProgram();
		mCubeParamsUniform = Link.Graphics.getUniformMat4("u_cubeParams", 1);
      mSkyParams1Uniform = Link.Graphics.getUniformMat4("u_skyParams1", 1);
      mSkyParams2Uniform = Link.Graphics.getUniformMat4("u_skyParams2", 1);
      mSkyParams3Uniform = Link.Graphics.getUniformMat4("u_skyParams3", 1);
      mSkyParams4Uniform = Link.Graphics.getUniformMat4("u_skyParams4", 1);
      mSkyParams5Uniform = Link.Graphics.getUniformMat4("u_skyParams5", 1);
	}

   mView	   = Link.Graphics.getView("RenderLayer1", 2000);
	mTexture = Plugins::Link.bgfx.createTextureCube(512, 1, bgfx::TextureFormat::RGBA16, BGFX_TEXTURE_BLIT_DST, NULL);

   // Default Settings
   mIntensity                 = 1.0f;
   mSunBrightness             = 800.0f;
   mSurfaceHeight             = 0.994f;
   mScatterStrength           = 0.05f;
   mMieBrightness             = 0.1f;
   mMieDistribution           = 0.13f;
   mMieCollectionPower        = 0.6f;
   mMieStrength               = 0.01f;
   mRayleighBrightness        = 2.5f;
   mRayleighCollectionPower   = 0.2f;
   mRayleighStrength          = 0.3f;
   mStepCount                 = 6.0f;
   mAirColor.set(0.18867780436772762f, 0.4978442963618773f, 0.6616065586417131f);

	mGenerateSkyCube = true;

   mTempSkyCubeCopyView = NULL;
   for (U32 side = 0; side < 6; ++side)
   {
      mTempSkyCubeView[side] = NULL;
      mTempSkyCubeTextures[side].idx = bgfx::invalidHandle;
      mTempSkyCubeBuffers[side].idx = bgfx::invalidHandle;
   }
}

void ScatterSky::initPersistFields()
{
   // Call parent.
   Parent::initPersistFields();

   addGroup("Scatter Sky");

      addField("Intensity", Link.Con.TypeF32, Offset(mIntensity, ScatterSky), "");
      addField("SunBrightness", Link.Con.TypeF32, Offset(mSunBrightness, ScatterSky), "");
      addField("SurfaceHeight", Link.Con.TypeF32, Offset(mSurfaceHeight, ScatterSky), "");
      addField("ScatterStrength", Link.Con.TypeF32, Offset(mScatterStrength, ScatterSky), "");
      addField("MieBrightness", Link.Con.TypeF32, Offset(mMieBrightness, ScatterSky), "");
      addField("MieDistribution", Link.Con.TypeF32, Offset(mMieDistribution, ScatterSky), "");
      addField("MieCollectionPower", Link.Con.TypeF32, Offset(mMieCollectionPower, ScatterSky), "");
      addField("MieStrength", Link.Con.TypeF32, Offset(mMieStrength, ScatterSky), "");
      addField("RayleighBrightness", Link.Con.TypeF32, Offset(mRayleighBrightness, ScatterSky), "");
      addField("RayleighCollectionPower", Link.Con.TypeF32, Offset(mRayleighCollectionPower, ScatterSky), "");
      addField("RayleighStrength", Link.Con.TypeF32, Offset(mRayleighStrength, ScatterSky), "");
      addField("StepCount", Link.Con.TypeF32, Offset(mStepCount, ScatterSky), "");
      addField("AirColor", Link.Con.TypeColorF, Offset(mAirColor, ScatterSky), "");

   endGroup("Scatter Sky");
}

void ScatterSky::onActivate()
{
   Parent::onActivate();
   mEnabled = true;
}

void ScatterSky::onDeactivate()
{
   Parent::onDeactivate();
   mEnabled = false;
}

void ScatterSky::refresh()
{
   mGenerateSkyCube = true;
}

void ScatterSky::preRender()
{
   // Generate SkyCube Begin
   if (mGenerateSkyCube)
      generateSkyCubeBegin();
}

void ScatterSky::render()
{
   // Generate SkyCube
   if (mGenerateSkyCube)
      generateSkyCube();

   // Render SkyCube
   if (!mEnabled || !bgfx::isValid(mTexture) || !bgfx::isValid(mShader) || !mSkyCubeReady)
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
   Link.bgfx.setState(0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_DEPTH_TEST_LESS, 0);

   // Render skybox as fullscreen quad.
   Link.Graphics.fullScreenQuad(*Link.Rendering.canvasWidth, *Link.Rendering.canvasHeight, 999.999f);
   Link.bgfx.submit(mView->id, mShader, 0);
}

void ScatterSky::postRender()
{
   // Generate SkyCube End
   if (mGenerateSkyCube)
      generateSkyCubeEnd();
}

void ScatterSky::generateSkyCubeBegin()
{
   // Initialize temporary buffers to use to generate sky cube.
   mTempSkyCubeCopyView = Link.Graphics.getTemporaryView("CopySkyCube", 351);
   for (U32 side = 0; side < 6; ++side)
   {
      char viewName[64];
      dSprintf(viewName, 64, "GenerateSkyCubeSide%d", side);

      mTempSkyCubeView[side] = Link.Graphics.getTemporaryView(Link.StringTableLink->insert(viewName), 350);
      mTempSkyCubeTextures[side] = Link.bgfx.createTexture2D(512, 512, 1, bgfx::TextureFormat::RGBA16, BGFX_TEXTURE_RT, NULL);
      bgfx::TextureHandle fbtextures[] = { mTempSkyCubeTextures[side] };
      mTempSkyCubeBuffers[side] = Link.bgfx.createFrameBuffer(BX_COUNTOF(fbtextures), fbtextures, false);
   }
}

void ScatterSky::generateSkyCube()
{
   // Uniforms
   F32 skyParams1[4] = { mIntensity, mSunBrightness, mSurfaceHeight, mScatterStrength };
   Link.bgfx.setUniform(mSkyParams1Uniform, skyParams1, 1);
   F32 skyParams2[4] = { mMieBrightness, mMieDistribution, mMieCollectionPower, mMieStrength };
   Link.bgfx.setUniform(mSkyParams2Uniform, skyParams2, 1);
   F32 skyParams3[4] = { mRayleighBrightness, mRayleighCollectionPower, mRayleighStrength, 0.0f };
   Link.bgfx.setUniform(mSkyParams3Uniform, skyParams3, 1);
   F32 skyParams4[4] = { mStepCount, Link.Rendering.directionalLightDir->x, Link.Rendering.directionalLightDir->z, -1.0f * Link.Rendering.directionalLightDir->y };
   Link.bgfx.setUniform(mSkyParams4Uniform, skyParams4, 1);
   F32 skyParams5[4] = { mAirColor.red, mAirColor.green, mAirColor.blue, 0.0f };
   Link.bgfx.setUniform(mSkyParams5Uniform, skyParams5, 1);

   // Process
   for (U32 side = 0; side < 6; ++side)
   {
      F32 generateParams[4] = { (F32)side, 0.0f, 0.0f, 0.0f };
      Link.bgfx.setUniform(mCubeParamsUniform, generateParams, 1);

      // This projection matrix is used because its a full screen quad.
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
      Link.bgfx.setViewTransform(mTempSkyCubeView[side]->id, NULL, proj, BGFX_VIEW_STEREO, NULL);
      Link.bgfx.setViewRect(mTempSkyCubeView[side]->id, 0, 0, 512, 512);
      Link.bgfx.setViewFrameBuffer(mTempSkyCubeView[side]->id, mTempSkyCubeBuffers[side]);
      Link.bgfx.setState(0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE, 0);

      Link.Graphics.fullScreenQuad(512.0f, 512.0f, 0.0f);
      Link.bgfx.submit(mTempSkyCubeView[side]->id, mGenerateSkyCubeShader, 0);

      // Copy framebuffer into cubemap side.
      Link.bgfx.blitB(mTempSkyCubeCopyView->id, mTexture, 0, 0, 0, side, mTempSkyCubeTextures[side], 0, 0, 0, 0, 512, 512, 1);
      Link.bgfx.touch(mTempSkyCubeCopyView->id);
      Con::printf("[%d, %d] Blit Sky to: %d from %d", mTempSkyCubeCopyView->id, mTempSkyCubeView[side]->id, mTexture.idx, mTempSkyCubeTextures[side].idx);
   }

   mSkyCubeReady = true;
}

void ScatterSky::generateSkyCubeEnd()
{
   for (U32 side = 0; side < 6; ++side)
   {
      //if (bgfx::isValid(mTempSkyCubeTextures[side]))
      //   Link.bgfx.destroyTexture(mTempSkyCubeTextures[side]);
      //if (bgfx::isValid(mTempSkyCubeBuffers[side]))
       //  Link.bgfx.destroyFrameBuffer(mTempSkyCubeBuffers[side]);
   }

   mGenerateSkyCube = false;
}