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

#include "lightComponent.h"
#include "console/consoleTypes.h"
#include "graphics/core.h"
#include "lighting/lighting.h"
#include "rendering/renderCamera.h"
#include "rendering/rendering.h"
#include "scene/scene.h"

// bgfx/bx
#include <bgfx/bgfx.h>
#include <bx/fpumath.h>

// Script bindings.
#include "lightComponent_Binding.h"

namespace Scene
{
   IMPLEMENT_CONOBJECT(LightComponent);

   LightComponent::LightComponent()
   {
      mLightData           = NULL;
      mLightTint           = ColorF(1.0f, 1.0f, 1.0f);
      mLightAttenuation    = 0.01f;
      mLightIntensity      = 1.0f;

      mDeferredLightShader          = Graphics::getDefaultShader("components/lightComponent/pointlight_vs.tsh", "components/lightComponent/pointlight_fs.tsh")->mProgram;
      mDeferredLightPosUniform      = Graphics::Shader::getUniformVec4("singleLightPos");
      mDeferredLightColorUniform    = Graphics::Shader::getUniformVec4("singleLightColor");
      mDeferredLightParamsUniform   = Graphics::Shader::getUniformVec4("singleLightParams");
   }

   void LightComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addGroup("LightComponent");
         addField("Tint",        TypeColorF, Offset(mLightTint, LightComponent), "");
         addField("Attenuation", TypeF32,    Offset(mLightAttenuation, LightComponent), "");
         addField("Intensity",   TypeF32,    Offset(mLightIntensity, LightComponent), "");
      endGroup("LightComponent");
   }

   void LightComponent::onAddToScene()
   {  
      // Register Light Data ( for forward )
      mLightData = Lighting::createLightData();

      // Register render hook.
      Rendering::addRenderHook(this);

      refresh();
   }

   void LightComponent::onRemoveFromScene()
   {  
      // Erase Light Data.
      mLightData->flags |= Lighting::LightData::Deleted;
      mLightData = NULL;

      // Remove render hook.
      Rendering::removeRenderHook(this);
   }

   void LightComponent::refresh()
   {
      Parent::refresh();

      mBoundingBox.minExtents.set(-1.0f, -1.0f, -1.0f);
      mBoundingBox.maxExtents.set(1.0f, 1.0f, 1.0f);
      mBoundingBox.transform(mTransform);

      // Sanity Checks.
      if ( mOwnerObject == NULL ) return;
      if ( mLightData == NULL ) return;

      mLightData->position    = mWorldPosition;
      mLightData->color[0]    = mLightTint.red;
      mLightData->color[1]    = mLightTint.green;
      mLightData->color[2]    = mLightTint.blue;
      mLightData->attenuation = mLightAttenuation;
      mLightData->intensity   = mLightIntensity / (4.0f * M_PI_F);
   }

   void LightComponent::preRender(Rendering::RenderCamera* camera)
   {

   }

   void LightComponent::render(Rendering::RenderCamera* camera)
   {
      if (!camera->getRenderPath()->hasLightBuffer())
         return;

      // [PosX, PosY, PosZ, Empty]
      float lightPos[4] = { mLightData->position.x, mLightData->position.y, mLightData->position.z, 0.0f };
      bgfx::setUniform(mDeferredLightPosUniform, lightPos);

      // [ColorR, ColorG, ColorB, Empty]
      float lightColor[4] = { mLightData->color[0], mLightData->color[1], mLightData->color[2], 0.0f };
      bgfx::setUniform(mDeferredLightColorUniform, lightColor);

      // [Attenuation, Intensity, Empty, Empty]
      float lightParams[4] = { mLightData->attenuation, mLightData->intensity, 0.0f, 0.0f };
      bgfx::setUniform(mDeferredLightParamsUniform, lightParams);

      bgfx::setTransform(&mTransformMatrix[0]);

      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), camera->getRenderPath()->getDepthTextureRead());
      bgfx::setTexture(1, Graphics::Shader::getTextureUniform(1), camera->getRenderPath()->getNormalTexture());
      bgfx::setTexture(2, Graphics::Shader::getTextureUniform(2), camera->getRenderPath()->getMatInfoTexture());

      bgfx::setIndexBuffer(Graphics::cubeIB);
      bgfx::setVertexBuffer(Graphics::cubeVB);
      bgfx::setState(0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_BLEND_ADD | BGFX_STATE_CULL_CCW | BGFX_STATE_DEPTH_TEST_GEQUAL);

      bgfx::submit(camera->getRenderPath()->getLightBufferView()->id, mDeferredLightShader);
   }

   void LightComponent::postRender(Rendering::RenderCamera* camera)
   {

   }
}
