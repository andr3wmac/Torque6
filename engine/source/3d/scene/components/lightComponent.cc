//-----------------------------------------------------------------------------
// Copyright (c) 2014 Andrew Mac
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
#include "3d/scene/rendering/forwardRendering.h"
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

      Con::printf("Light Component Created!");
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
      
      //ColorI lightColor(mLightColor);
      //U32 vertColor = BGFXCOLOR_RGBA(255, lightColor.blue, lightColor.green, lightColor.red);

      // Setup Forward Render Data (for now)
      Scene::ForwardRenderData data;
      Scene::forwardRenderList.push_back(data);
      mRenderData = &Scene::forwardRenderList.back();

      mRenderData->uniforms.clear();
      Scene::UniformData lightUniformData;
      lightUniformData.count = 1;
      lightUniformData.data = &mLightColor.red;
      lightUniformData.uniform = Graphics::Shader::getUniform("lightColor");
      mRenderData->uniforms.push_back(lightUniformData);

      // Setup Light Data
      Scene::LightData light_data;
      Scene::lightList.push_back(light_data);
      mLightData = &Scene::lightList.back();

      mShaderAsset.setAssetId(StringTable->insert("AnimatedMeshExample:lightShader"));

      refresh();
   }

   void LightComponent::refresh()
   {
      Parent::refresh();

      // Sanity Checks.
      if ( mOwnerEntity == NULL ) return;
      //if ( mRenderData == NULL ) return;
      if ( mLightData == NULL ) return;
      //if ( mShaderAsset.isNull() ) return;

      // Material Data
      mRenderData->shader = mShaderAsset->getProgram();

      // Base Component transform matrix is always slot 0 in the transform table.
      mRenderData->transformTable = mTransformMatrix;
      mRenderData->transformCount = 1;

      mRenderData->textures.clear();
      mRenderData->uniforms.clear();

      // Update render data.
      mRenderData->indexBuffer = Graphics::cubeIB;
      mRenderData->vertexBuffer = Graphics::cubeVB;

      mLightData->position = mWorldPosition;
      Con::printf("Light Data Position: %f %f %f", mLightData->position.x, mLightData->position.y, mLightData->position.z);
      Con::printf("Light Position: %f %f %f", mPosition.x, mPosition.y, mPosition.z);
      Con::printf("Light World Position: %f %f %f", mWorldPosition.x, mWorldPosition.y, mWorldPosition.z);
      mLightData->radius = mLightRadius;
      mLightData->color[0] = mLightColor.red;
      mLightData->color[1] = mLightColor.green;
      mLightData->color[2] = mLightColor.blue;
      mLightData->attenuation = mLightAtten;
   }
}