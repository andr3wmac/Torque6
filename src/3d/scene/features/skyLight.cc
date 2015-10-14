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
      mRadianceCubePath = StringTable->EmptyString;
      mIrradianceCubePath = StringTable->EmptyString;

      mDeferredAmbientView = Graphics::getView("DeferredAmbient", 1600);
      mShader = Graphics::getDefaultShader("features/skyLight/skyLight_vs.sc", "features/skyLight/skyLight_fs.sc");

      // Load Ambient Cubemap
      ambientCubemap.idx = bgfx::invalidHandle;
      u_ambientCube = Graphics::Shader::getUniform("u_ambientCube", bgfx::UniformType::Int1);
      ambientIrrCubemap.idx = bgfx::invalidHandle;
      u_ambientIrrCube = Graphics::Shader::getUniform("u_ambientIrrCube", bgfx::UniformType::Int1);

      mRadianceReady = false;
      mIrradianceReady = false;
   }

   SkyLight::~SkyLight()
   {
      destroyBuffers();
   }

   void SkyLight::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addProtectedField("RadianceCube", TypeAssetLooseFilePath, Offset(mRadianceCubePath, SkyLight), &SkyLight::setRadianceCube, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
      addProtectedField("IrradianceCube", TypeAssetLooseFilePath, Offset(mIrradianceCubePath, SkyLight), &SkyLight::setIrradianceCube, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
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

   void SkyLight::loadRadianceCubeTexture(StringTableEntry path)
   {
      mRadianceCubePath = path;
      TextureObject* ambientCubemapTex = TextureManager::loadTexture(mRadianceCubePath, TextureHandle::BitmapKeepTexture, false);
      if (ambientCubemapTex != NULL)
      {
         ambientCubemap = ambientCubemapTex->getBGFXTexture();
         mRadianceReady = true;
      }
   }

   void SkyLight::loadIrradianceCubeTexture(StringTableEntry path)
   {
      mIrradianceCubePath = path;
      TextureObject* ambientIrrCubemapTex = TextureManager::loadTexture(mIrradianceCubePath, TextureHandle::BitmapKeepTexture, false);
      if (ambientIrrCubemapTex != NULL)
      {
         ambientIrrCubemap = ambientIrrCubemapTex->getBGFXTexture();
         mIrradianceReady = true;
      }
   }

   void SkyLight::refresh()
   {

   }

   void SkyLight::preRender()
   {

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
      bgfx::setTexture(4, u_ambientCube, ambientCubemap);
      bgfx::setTexture(5, u_ambientIrrCube, ambientIrrCubemap);

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
