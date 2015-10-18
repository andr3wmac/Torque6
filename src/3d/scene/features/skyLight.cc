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

#include "skyLight.h"
#include "console/consoleInternal.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "3d/scene/core.h"
#include "3d/rendering/common.h"
#include "3d/rendering/deferredRendering.h"

namespace Scene
{
   IMPLEMENT_CONOBJECT(SkyLight);

   SkyLight::SkyLight()
   {
      mName = "Sky Light";

      // Input skybox cubemap
      mSkyCubePath = StringTable->EmptyString;
      mSkyCubemap = BGFX_INVALID_HANDLE;
      mSkyCubeUniform = Graphics::Shader::getUniform("u_skyCube", bgfx::UniformType::Int1);

      // Shared
      mDeferredAmbientView = Graphics::getView("DeferredAmbient", 1600);
      mShader              = Graphics::getDefaultShader("features/skyLight/skyLight_vs.sc", "features/skyLight/skyLight_fs.sc");
      mCubeParamsUniform   = Graphics::Shader::getUniform("u_cubeParams", bgfx::UniformType::Vec4);

      // Radiance Generation 512x512
      // 6 Mip Levels: 512, 256, 128, 64, 32, 16
      U16 radianceSize = 512;
      mGenerateRadianceShader = Graphics::getDefaultShader("features/skyLight/generateRad_vs.sc", "features/skyLight/generateRad_fs.sc");
      mCopyRadianceView       = Graphics::getView("CopyRadiance", 199);
      mRadianceCubemap        = bgfx::createTextureCube(radianceSize, 6, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MAG_POINT | BGFX_TEXTURE_MIP_POINT);
      mRadianceCubeUniform    = Graphics::Shader::getUniform("u_radianceCube", bgfx::UniformType::Int1);
      for (U32 mip = 0; mip < 6; ++mip)
      {
         for (U32 side = 0; side < 6; ++side)
         {
            char viewName[64];
            dSprintf(viewName, 64, "GenerateRadianceCubeMip%dSide%d", mip, side);

            mGenerateRadianceView[mip][side]       = Graphics::getView(StringTable->insert(viewName), 100);
            mGenerateRadianceTextures[mip][side]   = bgfx::createTexture2D(radianceSize, radianceSize, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT);
            bgfx::TextureHandle fbtextures[]       = { mGenerateRadianceTextures[mip][side] };
            mGenerateRadianceBuffers[mip][side]    = bgfx::createFrameBuffer(BX_COUNTOF(fbtextures), fbtextures);
         }

         radianceSize = radianceSize / 2;
      }

      // Irradiance Generation 128x128
      mGenerateIrradianceShader  = Graphics::getDefaultShader("features/skyLight/generateIrr_vs.sc", "features/skyLight/generateIrr_fs.sc");
      mCopyIrradianceView        = Graphics::getView("CopyIrradiance", 249);
      mIrradianceCubemap         = bgfx::createTextureCube(128, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MAG_POINT | BGFX_TEXTURE_MIP_POINT);
      mIrradianceCubeUniform     = Graphics::Shader::getUniform("u_irradianceCube", bgfx::UniformType::Int1);

      // Generate buffers and views for all 6 sides of irradiance
      for (U32 side = 0; side < 6; ++side)
      {
         char viewName[64];
         dSprintf(viewName, 64, "GenerateIrradianceCubeSide%d", side);

         mGenerateIrradianceView[side]       = Graphics::getView(StringTable->insert(viewName), 200);
         mGenerateIrradianceTextures[side]   = bgfx::createTexture2D(128, 128, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT);
         bgfx::TextureHandle fbtextures[]    = { mGenerateIrradianceTextures[side] };
         mGenerateIrradianceBuffers[side]    = bgfx::createFrameBuffer(BX_COUNTOF(fbtextures), fbtextures);
      }

      mGenerateRadiance    = false;
      mRadianceReady       = false;
      mGenerateIrradiance  = false;
      mIrradianceReady     = false;
   }

   SkyLight::~SkyLight()
   {
      // Radiance
      if (bgfx::isValid(mRadianceCubemap))
         bgfx::destroyTexture(mRadianceCubemap);
      for (U32 mip = 0; mip < 6; ++mip)
      {
         for (U32 side = 0; side < 6; ++side)
         {
            if (bgfx::isValid(mGenerateRadianceTextures[mip][side]))
               bgfx::destroyTexture(mGenerateRadianceTextures[mip][side]);
            if (bgfx::isValid(mGenerateRadianceBuffers[mip][side]))
               bgfx::destroyFrameBuffer(mGenerateRadianceBuffers[mip][side]);
         }
      }

      // Irradiance
      if (bgfx::isValid(mIrradianceCubemap))
         bgfx::destroyTexture(mIrradianceCubemap);
      for (U32 side = 0; side < 6; ++side)
      {
         if (bgfx::isValid(mGenerateIrradianceTextures[side]))
            bgfx::destroyTexture(mGenerateIrradianceTextures[side]);
         if (bgfx::isValid(mGenerateIrradianceBuffers[side]))
            bgfx::destroyFrameBuffer(mGenerateIrradianceBuffers[side]);
      }

      destroyBuffers();
   }

   void SkyLight::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addProtectedField("SkyCube", TypeAssetLooseFilePath, Offset(mSkyCubePath, SkyLight), &SkyLight::setSkyCube, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
   }

   void SkyLight::resize()
   {
      initBuffers();
      refresh();
   }

   void SkyLight::onActivate()
   {
      setRendering(true);
   }

   void SkyLight::onDeactivate()
   {
      setRendering(false);
   }

   void SkyLight::initBuffers()
   {
      destroyBuffers();
   }

   void SkyLight::destroyBuffers()
   {

   }

   void SkyLight::loadSkyCubeTexture(StringTableEntry path)
   {
      mSkyCubePath = path;
      TextureObject* ambientCubemapTex = TextureManager::loadTexture(mSkyCubePath, TextureHandle::BitmapKeepTexture, false);
      if (ambientCubemapTex != NULL)
      {
         mSkyCubemap = ambientCubemapTex->getBGFXTexture();
         mGenerateRadiance = true;
      }
   }

   void SkyLight::generateRadianceCubeTexture()
   {
      U16 radianceSize = 512;
      for (U32 mip = 0; mip < 6; ++mip)
      {
         for (U32 side = 0; side < 6; ++side)
         {
            F32 cubeParams[4] = { (F32)side, (F32)mip, 0.0f, 0.0f };
            bgfx::setUniform(mCubeParamsUniform, cubeParams);

            // This projection matrix is used because its a full screen quad.
            F32 proj[16];
            bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
            bgfx::setViewTransform(mGenerateRadianceView[mip][side]->id, NULL, proj);
            bgfx::setViewRect(mGenerateRadianceView[mip][side]->id, 0, 0, radianceSize, radianceSize);
            bgfx::setViewFrameBuffer(mGenerateRadianceView[mip][side]->id, mGenerateRadianceBuffers[mip][side]);

            // Setup textures
            bgfx::setTexture(0, mSkyCubeUniform, mSkyCubemap);

            bgfx::setState(0
               | BGFX_STATE_RGB_WRITE
               | BGFX_STATE_ALPHA_WRITE
               );

            fullScreenQuad(radianceSize, radianceSize);
            bgfx::submit(mGenerateRadianceView[mip][side]->id, mGenerateRadianceShader->mProgram);

            // Copy framebuffer into cubemap side.
            bgfx::blit(mCopyRadianceView->id, mRadianceCubemap, mip, 0, 0, side, mGenerateRadianceTextures[mip][side], 0, 0, 0, 0, radianceSize, radianceSize, 1);
         }
         radianceSize = radianceSize / 2;
      }

      mGenerateRadiance = false;
      mRadianceReady = true;
      mGenerateIrradiance = true;
   }

   void SkyLight::generateIrradianceCubeTexture()
   {
      for (U32 side = 0; side < 6; ++side)
      {
         F32 cubeParams[4] = { (F32)side, 0.0f, 0.0f, 0.0f };
         bgfx::setUniform(mCubeParamsUniform, cubeParams);

         // This projection matrix is used because its a full screen quad.
         F32 proj[16];
         bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
         bgfx::setViewTransform(mGenerateIrradianceView[side]->id, NULL, proj);
         bgfx::setViewRect(mGenerateIrradianceView[side]->id, 0, 0, 128, 128);
         bgfx::setViewFrameBuffer(mGenerateIrradianceView[side]->id, mGenerateIrradianceBuffers[side]);

         // Setup textures
         bgfx::setTexture(0, mSkyCubeUniform, mSkyCubemap);

         bgfx::setState(0
            | BGFX_STATE_RGB_WRITE
            | BGFX_STATE_ALPHA_WRITE
            );

         fullScreenQuad(128, 128);
         bgfx::submit(mGenerateIrradianceView[side]->id, mGenerateIrradianceShader->mProgram);

         // Copy framebuffer into cubemap side.
         bgfx::blit(mCopyIrradianceView->id, mIrradianceCubemap, 0, 0, 0, side, mGenerateIrradianceTextures[side], 0, 0, 0, 0, 128, 128, 1);
      }

      mGenerateIrradiance = false;
      mIrradianceReady = true;
   }

   void SkyLight::refresh()
   {

   }

   void SkyLight::preRender()
   {
      if (mGenerateRadiance)
         generateRadianceCubeTexture();

      if (mGenerateIrradiance)
         generateIrradianceCubeTexture();
   }

   void SkyLight::render()
   {
      if (!mRadianceReady || !mIrradianceReady) return;

      // This projection matrix is used because its a full screen quad.
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
      bgfx::setViewTransform(mDeferredAmbientView->id, NULL, proj);
      bgfx::setViewRect(mDeferredAmbientView->id, 0, 0, Rendering::canvasWidth, Rendering::canvasHeight);

      // Setup textures
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), Rendering::getGBuffer(), 0);     // Deferred Albedo
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), Rendering::getNormalTexture());  // Normals
      bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), Rendering::getMatInfoTexture()); // Material Info
      bgfx::setTexture(3, Graphics::Shader::getTextureUniform(3), Rendering::getDepthTexture());   // Depth
      bgfx::setTexture(4, mRadianceCubeUniform, mRadianceCubemap);
      bgfx::setTexture(5, mIrradianceCubeUniform, mIrradianceCubemap);

      bgfx::setState(0
         | BGFX_STATE_RGB_WRITE
         | BGFX_STATE_ALPHA_WRITE
         );

      fullScreenQuad((F32)Rendering::canvasWidth, (F32)Rendering::canvasHeight);

      bgfx::submit(mDeferredAmbientView->id, mShader->mProgram);
   }

   void SkyLight::postRender()
   {

   }
}
