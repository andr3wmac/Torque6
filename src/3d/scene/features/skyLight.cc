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
#include "3d/scene/scene.h"
#include "3d/rendering/common.h"
#include "3d/rendering/deferredRendering.h"

float radicalInverse_VdC(int bits) {
   bits = (bits << 16u) | (bits >> 16u);
   bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
   bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
   bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
   bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
   return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
Point2F Hammersley(int i, int N) {
   return Point2F(float(i) / float(N), radicalInverse_VdC(i));
}

namespace Scene
{
   IMPLEMENT_CONOBJECT(SkyLight);

   SkyLight::SkyLight()
   {
      mName = "Sky Light";

      // Input skybox cubemap
      mSkyCubePath      = StringTable->EmptyString;
      mSkyCubemap.idx   = bgfx::invalidHandle;
      mSkyCubeUniform   = Graphics::Shader::getUniform("u_skyCube", bgfx::UniformType::Int1);

      // Shared
      mDeferredAmbientView    = Graphics::getView("DeferredAmbient", 1600);
      mShader                 = Graphics::getDefaultShader("features/skyLight/skyLight_vs.sc", "features/skyLight/skyLight_fs.sc");
      mGenerateParamsUniform  = Graphics::Shader::getUniform("u_generateParams", bgfx::UniformType::Vec4);

      // Radiance Generation 512x512
      // 6 Mip Levels: 512, 256, 128, 64, 32, 16
      mRadianceSize           = 512;
      mGenerateRadianceShader = Graphics::getDefaultShader("features/skyLight/generateRad_vs.sc", "features/skyLight/generateRad_fs.sc");
      mRadianceCubemap.idx    = bgfx::invalidHandle;
      mRadianceCubeUniform    = Graphics::Shader::getUniform("u_radianceCube", bgfx::UniformType::Int1);

      // Irradiance Generation 128x128
      mGenerateIrradianceShader  = Graphics::getDefaultShader("features/skyLight/generateIrr_vs.sc", "features/skyLight/generateIrr_fs.sc");
      mIrradianceCubemap.idx     = bgfx::invalidHandle;
      mIrradianceCubeUniform     = Graphics::Shader::getUniform("u_irradianceCube", bgfx::UniformType::Int1);

      mGenerateRadiance    = false;
      mRadianceReady       = false;
      mGenerateIrradiance  = false;
      mIrradianceReady     = false;

      initBuffers();
   }

   SkyLight::~SkyLight()
   {
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

      mRadianceCubemap     = bgfx::createTextureCube(mRadianceSize, 6, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_BLIT_DST);
      mIrradianceCubemap   = bgfx::createTextureCube(128, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_BLIT_DST);
   }

   void SkyLight::destroyBuffers()
   {
      if (bgfx::isValid(mRadianceCubemap))
         bgfx::destroyTexture(mRadianceCubemap);

      if (bgfx::isValid(mIrradianceCubemap))
         bgfx::destroyTexture(mIrradianceCubemap);

      mRadianceCubemap.idx    = bgfx::invalidHandle;
      mIrradianceCubemap.idx  = bgfx::invalidHandle;
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
      // Initialize temporary buffers to use to generate radiance cube.
      Graphics::ViewTableEntry*  tempCopyRadianceView = Graphics::getTemporaryView("CopyRadiance", 199);
      Graphics::ViewTableEntry*  tempRadianceView[6][6];
      bgfx::FrameBufferHandle    tempRadianceBuffers[6][6];
      bgfx::TextureHandle        tempRadianceTextures[6][6];

      U32 radianceSize = mRadianceSize;
      for (U32 mip = 0; mip < 6; ++mip)
      {
         for (U32 side = 0; side < 6; ++side)
         {
            char viewName[64];
            dSprintf(viewName, 64, "GenerateRadianceCubeMip%dSide%d", mip, side);

            tempRadianceView[mip][side]      = Graphics::getTemporaryView(StringTable->insert(viewName), 100);
            tempRadianceTextures[mip][side]  = bgfx::createTexture2D(radianceSize, radianceSize, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT);
            bgfx::TextureHandle fbtextures[] = { tempRadianceTextures[mip][side] };
            tempRadianceBuffers[mip][side]   = bgfx::createFrameBuffer(BX_COUNTOF(fbtextures), fbtextures);
         }

         radianceSize = radianceSize / 2;
      }

      // Generate temp lookup table of Hammersley points
      // Note: This is done for compatability reasons. 
      //       Older openGL and DX9 can't do Hammersley in a shader (no bitwise operations)
      const bgfx::Memory* mem = bgfx::alloc(1024);
      for (U32 i = 0, n = 0; i < 512; ++i, n += 2)
      {
         Point2F pt = Hammersley(i, 512);
         mem->data[n] = pt.x * 255;
         mem->data[n + 1] = pt.y * 255;
      }
      bgfx::TextureHandle tempLUT = bgfx::createTexture2D(512, 1, 1, bgfx::TextureFormat::RG8, BGFX_TEXTURE_MAG_POINT | BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MIP_POINT, mem);

      // Process
      radianceSize = mRadianceSize;
      for (U32 mip = 0; mip < 6; ++mip)
      {
         for (U32 side = 0; side < 6; ++side)
         {
            // Side, Mip, Roughness
            F32 generateParams[4] = { (F32)side, (F32)mip, (F32)mip / 6.0f, 0.0f };
            bgfx::setUniform(mGenerateParamsUniform, generateParams);

            // This projection matrix is used because its a full screen quad.
            F32 proj[16];
            bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
            bgfx::setViewTransform(tempRadianceView[mip][side]->id, NULL, proj);
            bgfx::setViewRect(tempRadianceView[mip][side]->id, 0, 0, radianceSize, radianceSize);
            bgfx::setViewFrameBuffer(tempRadianceView[mip][side]->id, tempRadianceBuffers[mip][side]);

            // Setup textures
            bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), tempLUT);
            bgfx::setTexture(1, mSkyCubeUniform, mSkyCubemap);

            bgfx::setState(0
               | BGFX_STATE_RGB_WRITE
               | BGFX_STATE_ALPHA_WRITE
               );

            fullScreenQuad(radianceSize, radianceSize);
            bgfx::submit(tempRadianceView[mip][side]->id, mGenerateRadianceShader->mProgram);

            // Copy framebuffer into cubemap side at mip level
            bgfx::blit(tempCopyRadianceView->id, mRadianceCubemap, mip, 0, 0, side, tempRadianceTextures[mip][side], 0, 0, 0, 0, radianceSize, radianceSize, 1);
         }
         radianceSize = radianceSize / 2;
      }

      mGenerateRadiance    = false;
      mRadianceReady       = true;
      mGenerateIrradiance  = true;

      // Destroy temporary buffers.
      bgfx::destroyTexture(tempLUT);
      for (U32 mip = 0; mip < 6; ++mip)
      {
         for (U32 side = 0; side < 6; ++side)
         {
            if (bgfx::isValid(tempRadianceTextures[mip][side]))
               bgfx::destroyTexture(tempRadianceTextures[mip][side]);
            if (bgfx::isValid(tempRadianceBuffers[mip][side]))
               bgfx::destroyFrameBuffer(tempRadianceBuffers[mip][side]);
         }
      }
   }

   void SkyLight::generateIrradianceCubeTexture()
   {
      // Initialize temporary buffers to use to generate irradiance cube.
      Graphics::ViewTableEntry*  tempIrradianceCopyView = Graphics::getTemporaryView("CopyIrradiance", 249);
      Graphics::ViewTableEntry*  tempIrradianceView[6];
      bgfx::FrameBufferHandle    tempIrradianceBuffers[6];
      bgfx::TextureHandle        tempIrradianceTextures[6];
      for (U32 side = 0; side < 6; ++side)
      {
         char viewName[64];
         dSprintf(viewName, 64, "GenerateIrradianceCubeSide%d", side);

         tempIrradianceView[side]     = Graphics::getTemporaryView(StringTable->insert(viewName), 200);
         tempIrradianceTextures[side]     = bgfx::createTexture2D(128, 128, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT);
         bgfx::TextureHandle fbtextures[] = { tempIrradianceTextures[side] };
         tempIrradianceBuffers[side]      = bgfx::createFrameBuffer(BX_COUNTOF(fbtextures), fbtextures);
      }

      // Generate temp lookup table of Hammersley points
      // Note: This is done for compatability reasons. 
      //       Older openGL and DX9 can't do Hammersley in a shader (no bitwise operations)
      const bgfx::Memory* mem = bgfx::alloc(1024);
      for (U32 i = 0, n = 0; i < 512; ++i, n += 2)
      {
         Point2F pt = Hammersley(i, 512);
         mem->data[n] = pt.x * 255;
         mem->data[n + 1] = pt.y * 255;
      }
      bgfx::TextureHandle tempLUT = bgfx::createTexture2D(512, 1, 1, bgfx::TextureFormat::RG8, BGFX_TEXTURE_MAG_POINT | BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MIP_POINT, mem);

      // Process
      for (U32 side = 0; side < 6; ++side)
      {
         F32 generateParams[4] = { (F32)side, 0.0f, 0.0f, 0.0f };
         bgfx::setUniform(mGenerateParamsUniform, generateParams);

         // This projection matrix is used because its a full screen quad.
         F32 proj[16];
         bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
         bgfx::setViewTransform(tempIrradianceView[side]->id, NULL, proj);
         bgfx::setViewRect(tempIrradianceView[side]->id, 0, 0, 128, 128);
         bgfx::setViewFrameBuffer(tempIrradianceView[side]->id, tempIrradianceBuffers[side]);

         // Setup textures
         bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), tempLUT);
         bgfx::setTexture(1, mSkyCubeUniform, mSkyCubemap);

         bgfx::setState(0
            | BGFX_STATE_RGB_WRITE
            | BGFX_STATE_ALPHA_WRITE
            );

         fullScreenQuad(128, 128);
         bgfx::submit(tempIrradianceView[side]->id, mGenerateIrradianceShader->mProgram);

         // Copy framebuffer into cubemap side.
         bgfx::blit(tempIrradianceCopyView->id, mIrradianceCubemap, 0, 0, 0, side, tempIrradianceTextures[side], 0, 0, 0, 0, 128, 128, 1);
      }

      mGenerateIrradiance  = false;
      mIrradianceReady     = true;

      // Destroy temporary buffers.
      bgfx::destroyTexture(tempLUT);
      for (U32 side = 0; side < 6; ++side)
      {
         if (bgfx::isValid(tempIrradianceTextures[side]))
            bgfx::destroyTexture(tempIrradianceTextures[side]);
         if (bgfx::isValid(tempIrradianceBuffers[side]))
            bgfx::destroyFrameBuffer(tempIrradianceBuffers[side]);
      }
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
