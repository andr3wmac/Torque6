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

#include "directionalLight.h"
#include "console/consoleInternal.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "scene/scene.h"
#include "rendering/rendering.h"
#include "rendering/renderCamera.h"
#include "scene/components/cameraComponent.h"
#include "lighting/lighting.h"

#include <bx/fpumath.h>

namespace Scene
{
   IMPLEMENT_CONOBJECT(DirectionalLight);

   DirectionalLight::DirectionalLight()
   {
      mEnabled = false;

      // Shaders
      mLightShader       = Graphics::getDefaultShader("components/directionalLight/dirlight_vs.tsh", "components/directionalLight/dirlight_fs.tsh");
      mDebugLightShader  = Graphics::getDefaultShader("components/directionalLight/dirlight_debug_vs.tsh", "components/directionalLight/dirlight_debug_fs.tsh");

      // Directional Light Settings
      mColor.set(1.0f, 1.0f, 1.0f, 1.0f);
      mDirection.set(0.0f, -1.0f, 1.0f);

      // Shadow Params
      mShadowCascadeSize   = 2048;
      mBias                = 0.001f;
      mNormalOffset        = 1.0f;
      mSplitDistribution   = 0.95f;
      mFarPlane            = 200.0f;

      mShadowMap = new Lighting::CascadedShadowMap();
   }

   DirectionalLight::~DirectionalLight()
   {
      
   }

   void DirectionalLight::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addGroup("DirectionalLight");

         addField("Color", TypeColorF, Offset(mColor, DirectionalLight), "");
         addField("Direction", TypePoint3F, Offset(mDirection, DirectionalLight), "");

      endGroup("DirectionalLight");

      addGroup("Shadows");

         addField("SplitDistribution", TypeF32, Offset(mSplitDistribution, DirectionalLight), "");
         addField("FarPlane", TypeF32, Offset(mFarPlane, DirectionalLight), "");
         addField("Bias", TypeF32, Offset(mBias, DirectionalLight), "");
         addField("NormalOffset", TypeF32, Offset(mNormalOffset, DirectionalLight), "");

      endGroup("Shadows");
   }

   void DirectionalLight::resize()
   {
      refresh();
   }

   void DirectionalLight::onAddToScene()
   {
      CameraComponent* camera = mOwnerObject->findComponentByType<CameraComponent>();
      if (!camera)
         return;

      mCamera = camera->getRenderCamera();
      if (!mCamera)
         return;

      Rendering::addRenderHook(this);
      mShadowMap->init(mShadowCascadeSize);
      mEnabled = true;
   }

   void DirectionalLight::onRemoveFromScene()
   {
      mEnabled = false;

      if (mCamera)
         Rendering::removeRenderHook(this);
   }

   void DirectionalLight::refresh()
   {
      if (!mEnabled)
         return;

      // Used for forward rendering
      Lighting::setDirectionalLight(mDirection, mColor, mShadowMap->getShadowMap());
   }

   void DirectionalLight::preRender(Rendering::RenderCamera* camera)
   {
      
   }

   void DirectionalLight::render(Rendering::RenderCamera* camera)
   {
      if (!mEnabled)
         return;

      // Only render ShadowMaps for the camera we're attached to.
      if (camera == mCamera)
      {
         mShadowMap->configure(mDirection, mSplitDistribution, mFarPlane, mBias, mNormalOffset);
         mShadowMap->render(camera);
      }

      if (!camera->getRenderPath()->hasLightBuffer())
         return;

      // Directional Light
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);

      // Normals, Material Info, Depth
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), camera->getRenderPath()->getNormalTexture());
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), camera->getRenderPath()->getMatInfoTexture());
      bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), camera->getRenderPath()->getDepthTexture());

      // ShadowMap Cascades
      bgfx::setTexture(3, Lighting::directionalLight.shadowMapUniform, Lighting::directionalLight.shadowMap);

      // Draw Directional Light
      bgfx::setTransform(proj);
      bgfx::setState(0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE);
      fullScreenQuad((F32)camera->width, (F32)camera->height);

      if (Lighting::ShadowMapCascadeDebug::CascadeDebugEnabled)
         bgfx::submit(camera->getRenderPath()->getLightBufferView()->id, mDebugLightShader->mProgram);
      else
         bgfx::submit(camera->getRenderPath()->getLightBufferView()->id, mLightShader->mProgram);
   }

   void DirectionalLight::postRender(Rendering::RenderCamera* camera)
   {

   }
}
