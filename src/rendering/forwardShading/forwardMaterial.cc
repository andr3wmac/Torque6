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


#include "forwardMaterial.h"
#include "console/consoleInternal.h"
#include "graphics/shaders.h"
#include "graphics/dgl.h"
#include "scene/scene.h"
#include "rendering/renderCamera.h"
#include "materials/materialAsset.h"

#include <bgfx/bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

namespace Materials
{
   IMPLEMENT_MATERIAL_VARIANT_NODE("forward", ForwardOpaqueNode);

   void ForwardOpaqueNode::generatePixel(const MaterialGenerationSettings &settings, ReturnType refType, U32 flags)
   {
      Parent::generatePixel(settings, refType);

      MaterialTemplate* matTemplate = settings.matTemplate;

      // Headers
      matTemplate->addPixelHeader("");
      matTemplate->addPixelHeader("#include <lighting.tsh>");
      matTemplate->addPixelHeader("uniform vec4 u_camPos;");
      matTemplate->addPixelHeader("uniform vec4 u_sceneDirLightDirection;");
      matTemplate->addPixelHeader("uniform vec4 u_sceneDirLightColor;");

      // Color Source
      const char* colorVal = "vec4(1.0, 1.0, 1.0, 1.0)";
      BaseNode* colorNode = findNode(settings, mColorSrc);
      if (colorNode != NULL)
      {
         colorNode->generatePixel(settings, ReturnVec4);
         colorVal = colorNode->getPixelReference(settings, ReturnVec4);
      }

      // Emissive Source
      bool emissiveSet = false;
      char emissiveTex[256] = "vec4(0.0, 0.0, 0.0, 0.0)";
      const char* emissiveVal = "vec4(0.0, 0.0, 0.0, 0.0)";
      BaseNode* emissiveNode = findNode(settings, mEmissiveSrc);
      if (emissiveNode != NULL)
      {
         emissiveSet = true;
         emissiveNode->generatePixel(settings, ReturnVec4);
         dStrcpy(emissiveTex, emissiveNode->getPixelReference(settings, ReturnName));
         emissiveVal = emissiveNode->getPixelReference(settings, ReturnVec4);
      }

      // Base Color
      matTemplate->addPixelBody("");
      matTemplate->addPixelBody("    // Base Color");

      // Base Color Alpha Threshold
      if (mAlphaThreshold > 0.0f)
      {
         matTemplate->addPixelBody("    if (%s.a < %f) discard;", colorVal, mAlphaThreshold);
      }

      // Base Color Output = Color + Emissive
      matTemplate->addPixelBody("    vec3 baseColor = %s.rgb + %s.rgb + vec3(0.00001, 0.00001, 0.00001);", colorVal, emissiveVal);

      // Normal Source
      const char* normalVal = "normalize(v_normal)";
      matTemplate->addPixelBody("");
      matTemplate->addPixelBody("    // Normals");
      BaseNode* normalNode = findNode(settings, mNormalSrc);
      if (normalNode != NULL)
      {
         normalNode->generatePixel(settings, ReturnVec3);

         matTemplate->addPixelBody("    vec3 normalMap   = normalize(2.0 * %s - 1.0);", normalNode->getPixelReference(settings, ReturnVec3, NodeFlags::NormalMap));
         matTemplate->addPixelBody("    mat3 tbn         = getTBN(v_tangent.xyz, v_bitangent.xyz, v_normal.xyz);");
         matTemplate->addPixelBody("    vec3 normal      = normalize(mul(tbn, normalMap));");

         normalVal = "normal";
      }

      // Normal Output
      matTemplate->addPixelBody("    vec3 normals = %s.xyz;", normalVal);

      // Metallic Source
      const char* metallicVal = "0.0";
      BaseNode* metallicNode = findNode(settings, mMetallicSrc);
      if (metallicNode)
      {
         metallicNode->generatePixel(settings, ReturnFloat);
         metallicVal = metallicNode->getPixelReference(settings, ReturnFloat);
      }
      // Roughness Source
      const char* roughnessVal = "0.0";
      BaseNode* roughnessNode = findNode(settings, mRoughnessSrc);
      if (roughnessNode)
      {
         roughnessNode->generatePixel(settings, ReturnFloat);
         roughnessVal = roughnessNode->getPixelReference(settings, ReturnFloat);
      }

      // Define Surface
      matTemplate->addPixelBody("");
      matTemplate->addPixelBody("    // Surface Info");
      matTemplate->addPixelBody("    Surface surface;");
      matTemplate->addPixelBody("    surface.worldSpacePosition = v_position.xyz;");
      matTemplate->addPixelBody("    surface.albedo             = baseColor;");
      matTemplate->addPixelBody("    surface.normal             = normals;");
      matTemplate->addPixelBody("    surface.metallic           = %s;", metallicVal);
      matTemplate->addPixelBody("    surface.roughness          = %s;", roughnessVal);

      // View Direction
      matTemplate->addPixelBody("");
      matTemplate->addPixelBody("    // View Direction");
      matTemplate->addPixelBody("    vec3 viewDir = normalize(u_camPos.xyz - v_position.xyz);", normalVal);

      // Reflectivity
      matTemplate->addPixelBody("");
      matTemplate->addPixelBody("    // Reflectivity");
      matTemplate->addPixelBody("    vec3 reflectivity   = vec3_splat(surface.metallic);");
      matTemplate->addPixelBody("    reflectivity        = clamp(reflectivity, 0.0, 0.999);");
      matTemplate->addPixelBody("    vec3 surfaceReflect = mix(vec3_splat(0.04), surface.albedo, reflectivity);");
      matTemplate->addPixelBody("    vec3 surfaceColor   = surface.albedo * (vec3_splat(1.0) - reflectivity);");
      matTemplate->addPixelBody("    surfaceColor        = clamp(surfaceColor, 0.0, 1.0);");

      // Directional Light + Shadows
      U8 extraTextureSlot = matTemplate->getUnusedTextureSlot();
      matTemplate->addPixelHeader("");
      matTemplate->addPixelHeader("// Cascaded ShadowMap");
      matTemplate->addPixelHeader("#include <shadows.tsh>");
      matTemplate->addPixelHeader("SAMPLER2DSHADOW(ShadowMap, %d); // ShadowMap", extraTextureSlot);
      matTemplate->addPixelHeader("uniform mat4 u_shadowMtx[4];");
      matTemplate->addPixelHeader("uniform vec4 u_shadowParams;");

      matTemplate->addPixelBody("");
      matTemplate->addPixelBody("    // Directional Light + Shadows");
      matTemplate->addPixelBody("    float shadow              = getShadow(surface, ShadowMap, u_shadowMtx, u_shadowParams);");
      matTemplate->addPixelBody("    LightingResult dirLight   = getDirectionalLight(surface, viewDir, u_sceneDirLightDirection.xyz, u_sceneDirLightColor.rgb, shadow);");

      // Environment Light
      matTemplate->addPixelHeader("");
      matTemplate->addPixelHeader("// Environment Lighting");
      matTemplate->addPixelHeader("SAMPLER2D(BRDFTexture, %d);          // BRDF Texture", extraTextureSlot + 1);
      matTemplate->addPixelHeader("SAMPLERCUBE(RadianceCubemap, %d);    // Radiance", extraTextureSlot + 2);
      matTemplate->addPixelHeader("SAMPLERCUBE(IrradianceCubemap, %d);  // Irradiance", extraTextureSlot + 3);

      matTemplate->addPixelBody("");
      matTemplate->addPixelBody("    // Sky Light");
      matTemplate->addPixelBody("    LightingResult ibl;");
      matTemplate->addPixelBody("    ibl.diffuse  = iblDiffuse(surface, IrradianceCubemap);");
      matTemplate->addPixelBody("    ibl.specular = iblSpecular(surface, viewDir, BRDFTexture, RadianceCubemap);");

      // Final Output
      matTemplate->addPixelBody("");
      matTemplate->addPixelBody("    // Final Output");
      if (emissiveSet)
         matTemplate->addPixelBody("    gl_FragColor = encodeRGBE8(baseColor);");
      else
         matTemplate->addPixelBody("    gl_FragColor = encodeRGBE8(((dirLight.diffuse + ibl.diffuse) * surfaceColor) + ((dirLight.specular + ibl.specular) * surfaceReflect));");
   }
}