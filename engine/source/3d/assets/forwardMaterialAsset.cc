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
#include "forwardMaterialAsset.h"
#include "graphics/utilities.h"
#include "../scene/core.h"
#include "3d/rendering/shadows.h"

// Script bindings.
#include "forwardMaterialAsset_ScriptBinding.h"

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

//------------------------------------------------------------------------------

ConsoleType( ForwardMaterialAssetPtr, TypeForwardMaterialAssetPtr, sizeof(AssetPtr<ForwardMaterialAsset>), ASSET_ID_FIELD_PREFIX )

//-----------------------------------------------------------------------------

ConsoleGetType( TypeForwardMaterialAssetPtr )
{
    // Fetch asset Id.
    return (*((AssetPtr<ForwardMaterialAsset>*)dptr)).getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleSetType( TypeForwardMaterialAssetPtr )
{
    // Was a single argument specified?
    if( argc == 1 )
    {
        // Yes, so fetch field value.
        const char* pFieldValue = argv[0];

        // Fetch asset pointer.
        AssetPtr<ForwardMaterialAsset>* pAssetPtr = dynamic_cast<AssetPtr<ForwardMaterialAsset>*>((AssetPtrBase*)(dptr));

        // Is the asset pointer the correct type?
        if ( pAssetPtr == NULL )
        {
            // No, so fail.
            Con::warnf( "(TypeForwardMaterialAssetPtr) - Failed to set asset Id '%d'.", pFieldValue );
            return;
        }

        // Set asset.
        pAssetPtr->setAssetId( pFieldValue );

        return;
   }

    // Warn.
    Con::warnf( "(TypeForwardMaterialAssetPtr) - Cannot set multiple args to a single asset." );
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(ForwardMaterialAsset);

//------------------------------------------------------------------------------

ForwardMaterialAsset::ForwardMaterialAsset()
{
   //
}

//------------------------------------------------------------------------------

ForwardMaterialAsset::~ForwardMaterialAsset()
{
   //
}

void ForwardMaterialAsset::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();
}

void ForwardMaterialAsset::applyMaterial(Rendering::RenderData* renderData, Scene::BaseComponent* component)
{
   // Forward material applys the textures for us
   Parent::applyMaterial(renderData, component);

   // Shadowmap
   Rendering::applyShadowMap(renderData);

   // Render to Forward View
   renderData->view = Graphics::RenderLayer2;

   // Forward Lighting Uniforms
   if ( component != NULL )
   {
      F32 lightPosRadius[4][4];
      F32 lightColorAttn[4][4];

      // Find Nearest Lights
      // TODO: Replace with Bounding Volume Hiearchy
      Vector<Rendering::LightData*> nearestLights = Rendering::getNearestLights(component->getWorldPosition());
      for( S32 t = 0; t < nearestLights.size(); ++t )
      {
         dMemcpy(lightPosRadius[t], nearestLights[t]->position, sizeof(F32) * 3);
         lightPosRadius[t][3] = nearestLights[t]->radius;
         dMemcpy(lightColorAttn[t], nearestLights[t]->color, sizeof(F32) * 3);
         lightColorAttn[t][3] = nearestLights[t]->attenuation;
      }
      
      // [PosX, PosY, PosZ, Radius]
      Rendering::UniformData* uLightPosRadius = renderData->uniforms.addUniform();
      uLightPosRadius->uniform = Graphics::Shader::getUniformVec4("lightPosRadius", 4);
      uLightPosRadius->count = nearestLights.size();
      uLightPosRadius->data = new F32[4][4];
      dMemcpy(uLightPosRadius->data, lightPosRadius, sizeof(lightPosRadius));

      // [ColorR, ColorG, ColorB, Attenuation(0-1)]
      Rendering::UniformData* uLightColorAttn = renderData->uniforms.addUniform();
      uLightColorAttn->uniform = Graphics::Shader::getUniformVec4("lightColorAttn", 4);
      uLightColorAttn->count = nearestLights.size();
      uLightColorAttn->data = new F32[4][4];
      dMemcpy(uLightColorAttn->data, lightColorAttn, sizeof(lightColorAttn));

      // Directional Light - Direction
      Rendering::UniformData* uDirLightDirection = renderData->uniforms.addUniform();
      uDirLightDirection->uniform = Graphics::Shader::getUniformVec3("dirLightDirection");
      uDirLightDirection->data = new Point3F(Scene::directionalLightDir);
      uDirLightDirection->count = 1;

      // Directional Light - Light Color
      Rendering::UniformData* uDirLightColor = renderData->uniforms.addUniform();
      uDirLightColor->uniform = Graphics::Shader::getUniformVec3("dirLightColor");
      uDirLightColor->data = new Point3F(Scene::directionalLightColor.red, 
                                         Scene::directionalLightColor.green, 
                                         Scene::directionalLightColor.blue);
      uDirLightColor->count = 1;

      // Directional Light - Ambient
      Rendering::UniformData* uDirLightAmbient = renderData->uniforms.addUniform();
      uDirLightAmbient->uniform = Graphics::Shader::getUniformVec3("dirLightAmbient");
      uDirLightAmbient->data = new Point3F(Scene::directionalLightAmbient.red, 
                                         Scene::directionalLightAmbient.green, 
                                         Scene::directionalLightAmbient.blue);
      uDirLightAmbient->count = 1;
   }
}
