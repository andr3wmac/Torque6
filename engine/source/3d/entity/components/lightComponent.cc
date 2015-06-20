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
#include "graphics/core.h"
#include "3d/rendering/common.h"
#include "3d/scene/core.h"

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
      mRenderData    = NULL;
      mLightData     = NULL;
      mLightRadius   = 10.0f;
      mLightColor    = ColorF(1.0f, 1.0f, 1.0f);
      mLightAtten    = 0.8f;
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
      // Register Light Data ( for forward )
      Rendering::LightData light_data;
      Rendering::lightList.push_back(light_data);
      mLightData = &Rendering::lightList.back();

      // Render Data ( for deferred )
      mRenderData       = Rendering::createRenderData();
      mRenderData->view = Graphics::getView("DeferredLight");
      
      mRenderData->indexBuffer   = Graphics::cubeIB;
      mRenderData->vertexBuffer  = Graphics::cubeVB;
      mRenderData->shader        = Graphics::getShader("deferred/pointlight_vs.sc", "deferred/pointlight_fs.sc")->mProgram;
      mRenderData->state         = 0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_CULL_CCW | BGFX_STATE_BLEND_ADD;

      mScale.set(mLightRadius - 1.0f, mLightRadius - 1.0f, mLightRadius - 1.0f);

      refresh();
   }

   void LightComponent::onRemoveFromScene()
   {  
      if ( mRenderData == NULL )
         return;

      mRenderData->deleted = true;
   }

   void LightComponent::refresh()
   {
      Parent::refresh();

      mBoundingBox.minExtents.set(mPosition.x + mLightRadius * -1.0f, mPosition.y + mLightRadius * -1.0f, mPosition.z + mLightRadius * -1.0f);
      mBoundingBox.maxExtents.set(mPosition.x + mLightRadius,         mPosition.y + mLightRadius,         mPosition.z + mLightRadius);

      // Sanity Checks.
      if ( mOwnerEntity == NULL ) return;
      if ( mLightData == NULL ) return;

      mLightData->position    = mWorldPosition;
      mLightData->radius      = mLightRadius;
      mLightData->color[0]    = mLightColor.red;
      mLightData->color[1]    = mLightColor.green;
      mLightData->color[2]    = mLightColor.blue;
      mLightData->attenuation = mLightAtten;

      // Debug Render.
      if ( mRenderData )
      {
         // Base Component transform matrix is always slot 0 in the transform table.
         mRenderData->transformTable = &mTransformMatrix[0];
         mRenderData->transformCount = 1;

         // Depth Texture
         textures.clear();
         mRenderData->textures = &textures;

         // Depth
         Rendering::TextureData depthTex;
         depthTex.uniform = Graphics::Shader::getTextureUniform(0);
         depthTex.isDepthTexture = true;
         textures.push_back(depthTex);

         // Normals
         Rendering::TextureData normalsTex;
         normalsTex.uniform = Graphics::Shader::getTextureUniform(1);
         normalsTex.isNormalTexture = true;
         textures.push_back(normalsTex);

         // Setup Uniforms with Light Data
         mRenderData->uniforms.uniforms = &uniforms;
         uniforms.clear();

         // [PosX, PosY, PosZ, Radius]
         uniforms.push_back(Rendering::UniformData(Graphics::Shader::getUniformVec4("singleLightPosRadius")));
         Rendering::UniformData* uLightPosRadius = &uniforms.back();
         uLightPosRadius->setValue(Point4F(mLightData->position.x, mLightData->position.y, mLightData->position.z, mLightData->radius));

         // [ColorR, ColorG, ColorB, Attenuation(0-1)]
         uniforms.push_back(Rendering::UniformData(Graphics::Shader::getUniformVec4("singleLightColorAttn")));
         Rendering::UniformData* uLightColorAttn = &uniforms.back();
         uLightColorAttn->setValue(Point4F(mLightData->color[0], mLightData->color[1], mLightData->color[2], mLightData->attenuation));

         // Camera Pos
         uniforms.push_back(Rendering::UniformData(Graphics::Shader::getUniformVec4("ptLightCamPos")));
         Rendering::UniformData* uLightCamPos = &uniforms.back();
         uLightCamPos->setValue(Scene::getActiveCamera()->getPosition());
      }
   }
}
