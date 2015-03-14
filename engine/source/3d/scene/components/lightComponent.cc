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

#include "console/consoleTypes.h"
#include "lightComponent.h"
#include "graphics/utilities.h"
#include "3d/rendering/forwardRendering.h"
#include "3d/assets/shaderAsset.h"

// Script bindings.
#include "lightComponent_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

// bgfx/bx
#include <bgfx.h>
#include <bx/fpumath.h>

// Assimp - Asset Import Library
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

namespace Scene
{
   IMPLEMENT_CONOBJECT(LightComponent);

   LightComponent::LightComponent()
   {
      mRenderData = NULL;
      mLightData = NULL;
      mLightRadius = 10.0f;
      mLightColor = ColorF(1.0f, 1.0f, 1.0f);
      mLightAtten = 0.8f;
   }

   void LightComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("Radius",      TypeF32,    Offset(mLightRadius, LightComponent), "");
      addField("Color",       TypeColorF, Offset(mLightColor,  LightComponent), "");
      addField("Attenuation", TypeF32,    Offset(mLightAtten,  LightComponent), "");
   }

   void LightComponent::onAddToScene()
   {  
      // Register Light Data
      Rendering::LightData light_data;
      Rendering::lightList.push_back(light_data);
      mLightData = &Rendering::lightList.back();

      // Debug Render
      mRenderData = Rendering::createRenderData();
      mRenderData->view = Graphics::ViewTable::DeferredLight;
      
      mRenderData->indexBuffer = Graphics::cubeIB;
      mRenderData->vertexBuffer = Graphics::cubeVB;
      mRenderData->shader = Graphics::getShader("pointlight_vs.sc", "pointlight_fs.sc")->mProgram;
      mRenderData->state = 0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_BLEND_ADD;

      mScale.set(mLightRadius - 1, mLightRadius - 1, mLightRadius - 1);

      refresh();
   }

   void LightComponent::refresh()
   {
      Parent::refresh();

      mBoundingBox.mMin.set(mPosition.x + mLightRadius * -1, mPosition.y + mLightRadius * -1, mPosition.z + mLightRadius * -1);
      mBoundingBox.mMax.set(mPosition.x + mLightRadius, mPosition.y + mLightRadius, mPosition.z + mLightRadius);

      // Sanity Checks.
      if ( mOwnerEntity == NULL ) return;
      if ( mLightData == NULL ) return;

      mLightData->position = mWorldPosition;
      mLightData->radius = mLightRadius;
      mLightData->color[0] = mLightColor.red;
      mLightData->color[1] = mLightColor.green;
      mLightData->color[2] = mLightColor.blue;
      mLightData->attenuation = mLightAtten;

      // Debug Render.
      if ( mRenderData )
      {
         // Base Component transform matrix is always slot 0 in the transform table.
         mRenderData->transformTable = &mTransformMatrix[0];
         mRenderData->transformCount = 1;

         // Depth Texture
         textures.clear();
         Rendering::TextureData texture;
         texture.uniform = Graphics::Shader::getTextureUniform(0);
         texture.isDepthTexture = true;
         //texture.handle = Rendering::getDepthTexture();
         textures.push_back(texture);
         mRenderData->textures = &textures;

         // Setup Uniforms with Light Data
         mRenderData->uniforms = &uniforms;
         uniforms.clear();

         // [PosX, PosY, PosZ, Radius]
         uniforms.push_back(Rendering::UniformData(Graphics::Shader::getUniformVec4("singleLightPosRadius"), NULL, 1));
         Rendering::UniformData* uLightPosRadius = &uniforms.back();
         F32 lightPosRadius[4] = {mLightData->position.x, mLightData->position.y, mLightData->position.z, mLightData->radius};
         uLightPosRadius->data = new F32[4];
         dMemcpy(uLightPosRadius->data, lightPosRadius, sizeof(lightPosRadius));

         // [ColorR, ColorG, ColorB, Attenuation(0-1)]
         uniforms.push_back(Rendering::UniformData(Graphics::Shader::getUniformVec4("singleLightColorAttn"), NULL, 1));
         Rendering::UniformData* uLightColorAttn = &uniforms.back();
         F32 lightColorAttn[4] = {mLightData->color[0], mLightData->color[1], mLightData->color[2], mLightData->attenuation};
         uLightColorAttn->data = new F32[4];
         dMemcpy(uLightColorAttn->data, lightColorAttn, sizeof(lightColorAttn));
      }
   }
}