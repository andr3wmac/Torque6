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
#include <rendering/rendering.h>
#include <graphics/core.h>

#include <bx/fpumath.h>

IMPLEMENT_PLUGIN_CONOBJECT(ScatterSky);

ScatterSky::ScatterSky()
   : mEnabled(false),
	  mSkyCubeReady(false)
{
   //mName = "Scatter Sky";

   // Load Skybox Shader
   mShader.idx = bgfx::invalidHandle;
   Graphics::ShaderAsset* skyboxShaderAsset = Torque::Graphics.getShaderAsset("Sky:skyboxShader");
   if (skyboxShaderAsset)
   {
      mShader = skyboxShaderAsset->getProgram();
      mMatrixUniform = Torque::Graphics.getUniformMat4("u_mtx", 1);
   }

	// Load Scatter Sky Shader
	mGenerateSkyCubeShader.idx = bgfx::invalidHandle;
	Graphics::ShaderAsset* scatterSkyShaderAsset = Torque::Graphics.getShaderAsset("Sky:scatterSkyShader");
	if (scatterSkyShaderAsset)
	{
		mGenerateSkyCubeShader = scatterSkyShaderAsset->getProgram();
		mCubeParamsUniform = Torque::Graphics.getUniformMat4("u_cubeParams", 1);
      mSkyParams1Uniform = Torque::Graphics.getUniformMat4("u_skyParams1", 1);
      mSkyParams2Uniform = Torque::Graphics.getUniformMat4("u_skyParams2", 1);
      mSkyParams3Uniform = Torque::Graphics.getUniformMat4("u_skyParams3", 1);
      mSkyParams4Uniform = Torque::Graphics.getUniformMat4("u_skyParams4", 1);
      mSkyParams5Uniform = Torque::Graphics.getUniformMat4("u_skyParams5", 1);
	}

   mView	   = Torque::Graphics.getView("RenderLayer0", 2000);
	mTexture = Torque::bgfx.createTextureCube(512, 1, bgfx::TextureFormat::RGBA16, BGFX_TEXTURE_BLIT_DST, NULL);

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

      addField("Intensity", Torque::Con.TypeF32, Offset(mIntensity, ScatterSky), "");
      addField("SunBrightness", Torque::Con.TypeF32, Offset(mSunBrightness, ScatterSky), "");
      addField("SurfaceHeight", Torque::Con.TypeF32, Offset(mSurfaceHeight, ScatterSky), "");
      addField("ScatterStrength", Torque::Con.TypeF32, Offset(mScatterStrength, ScatterSky), "");
      addField("MieBrightness", Torque::Con.TypeF32, Offset(mMieBrightness, ScatterSky), "");
      addField("MieDistribution", Torque::Con.TypeF32, Offset(mMieDistribution, ScatterSky), "");
      addField("MieCollectionPower", Torque::Con.TypeF32, Offset(mMieCollectionPower, ScatterSky), "");
      addField("MieStrength", Torque::Con.TypeF32, Offset(mMieStrength, ScatterSky), "");
      addField("RayleighBrightness", Torque::Con.TypeF32, Offset(mRayleighBrightness, ScatterSky), "");
      addField("RayleighCollectionPower", Torque::Con.TypeF32, Offset(mRayleighCollectionPower, ScatterSky), "");
      addField("RayleighStrength", Torque::Con.TypeF32, Offset(mRayleighStrength, ScatterSky), "");
      addField("StepCount", Torque::Con.TypeF32, Offset(mStepCount, ScatterSky), "");
      addField("AirColor", Torque::Con.TypeColorF, Offset(mAirColor, ScatterSky), "");

   endGroup("Scatter Sky");
}

void ScatterSky::onAddToScene()
{
   mEnabled = true;
}

void ScatterSky::onRemoveFromScene()
{
   mEnabled = false;
}

void ScatterSky::refresh()
{
   mGenerateSkyCube = true;
}

void ScatterSky::onAddToCamera()
{

}

void ScatterSky::onRemoveFromCamera()
{

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
   Torque::bgfx.setViewTransform(mView->id, NULL, proj, BGFX_VIEW_STEREO, NULL);
   Torque::bgfx.setViewRect(mView->id, 0, 0, (U16)(*Torque::Rendering.canvasWidth), (U16)(*Torque::Rendering.canvasHeight));

   // Calculate view matrix based on current view matrix.
   float viewMtx[16];
   bx::mtxInverse(viewMtx, mCamera->viewMatrix);

   Torque::bgfx.setUniform(mMatrixUniform, viewMtx, 1);
   Torque::bgfx.setTexture(0, Torque::Graphics.getTextureUniform(0), mTexture, UINT32_MAX);
   Torque::bgfx.setState(0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_DEPTH_TEST_LESS, 0);

   // Render skybox as fullscreen quad.
   Torque::Graphics.fullScreenQuad((F32)(*Torque::Rendering.canvasWidth), (F32)(*Torque::Rendering.canvasHeight), 999.999f);
   Torque::bgfx.submit(mView->id, mShader, 0);
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
   mTempSkyCubeCopyView = Torque::Graphics.getTemporaryView("CopySkyCube", 351);
   for (U32 side = 0; side < 6; ++side)
   {
      char viewName[64];
      dSprintf(viewName, 64, "GenerateSkyCubeSide%d", side);

      mTempSkyCubeView[side] = Torque::Graphics.getTemporaryView(Torque::StringTableLink->insert(viewName), 350);
      mTempSkyCubeTextures[side] = Torque::bgfx.createTexture2D(512, 512, 1, bgfx::TextureFormat::RGBA16, BGFX_TEXTURE_RT, NULL);
      bgfx::TextureHandle fbtextures[] = { mTempSkyCubeTextures[side] };
      mTempSkyCubeBuffers[side] = Torque::bgfx.createFrameBuffer(BX_COUNTOF(fbtextures), fbtextures, false);
   }
}

void ScatterSky::generateSkyCube()
{
   // Uniforms
   F32 skyParams1[4] = { mIntensity, mSunBrightness, mSurfaceHeight, mScatterStrength };
   Torque::bgfx.setUniform(mSkyParams1Uniform, skyParams1, 1);
   F32 skyParams2[4] = { mMieBrightness, mMieDistribution, mMieCollectionPower, mMieStrength };
   Torque::bgfx.setUniform(mSkyParams2Uniform, skyParams2, 1);
   F32 skyParams3[4] = { mRayleighBrightness, mRayleighCollectionPower, mRayleighStrength, 0.0f };
   Torque::bgfx.setUniform(mSkyParams3Uniform, skyParams3, 1);
   F32 skyParams4[4] = { mStepCount, Torque::Rendering.directionalLightDir->x, Torque::Rendering.directionalLightDir->z, -1.0f * Torque::Rendering.directionalLightDir->y };
   Torque::bgfx.setUniform(mSkyParams4Uniform, skyParams4, 1);
   F32 skyParams5[4] = { mAirColor.red, mAirColor.green, mAirColor.blue, 0.0f };
   Torque::bgfx.setUniform(mSkyParams5Uniform, skyParams5, 1);

   // Process
   for (U16 side = 0; side < 6; ++side)
   {
      F32 generateParams[4] = { (F32)side, 0.0f, 0.0f, 0.0f };
      Torque::bgfx.setUniform(mCubeParamsUniform, generateParams, 1);

      // This projection matrix is used because its a full screen quad.
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
      Torque::bgfx.setViewTransform(mTempSkyCubeView[side]->id, NULL, proj, BGFX_VIEW_STEREO, NULL);
      Torque::bgfx.setViewRect(mTempSkyCubeView[side]->id, 0, 0, 512, 512);
      Torque::bgfx.setViewFrameBuffer(mTempSkyCubeView[side]->id, mTempSkyCubeBuffers[side]);
      Torque::bgfx.setState(0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE, 0);

      Torque::Graphics.fullScreenQuad(512.0f, 512.0f, 0.0f);
      Torque::bgfx.submit(mTempSkyCubeView[side]->id, mGenerateSkyCubeShader, 0);

      // Copy framebuffer into cubemap side.
      Torque::bgfx.blitB(mTempSkyCubeCopyView->id, mTexture, 0, 0, 0, side, mTempSkyCubeTextures[side], 0, 0, 0, 0, 512, 512, 1);
      Torque::bgfx.touch(mTempSkyCubeCopyView->id);
      Con::printf("[%d, %d] Blit Sky to: %d from %d", mTempSkyCubeCopyView->id, mTempSkyCubeView[side]->id, mTexture.idx, mTempSkyCubeTextures[side].idx);
   }

   mSkyCubeReady = true;
}

void ScatterSky::generateSkyCubeEnd()
{
   for (U32 side = 0; side < 6; ++side)
   {
      //if (bgfx::isValid(mTempSkyCubeTextures[side]))
      //   Torque::bgfx.destroyTexture(mTempSkyCubeTextures[side]);
      //if (bgfx::isValid(mTempSkyCubeBuffers[side]))
       //  Torque::bgfx.destroyFrameBuffer(mTempSkyCubeBuffers[side]);
   }

   mGenerateSkyCube = false;
}