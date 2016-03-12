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

   void ForwardOpaqueNode::generatePixel(const MaterialGenerationSettings &settings, ReturnType refType)
   {
      Parent::generatePixel(settings, refType);

      MaterialTemplate* matTemplate = settings.matTemplate;

      // Headers
      matTemplate->addPixelHeader("");
      matTemplate->addPixelHeader("#include <lighting.tsh>");
      matTemplate->addPixelHeader("uniform vec4 u_camPos;");
      matTemplate->addPixelHeader("uniform vec4 dirLightDirection;");
      matTemplate->addPixelHeader("uniform vec4 dirLightColor;");

      // Cascaded ShadowMap
      //matTemplate->addPixelHeader("");
      //matTemplate->addPixelHeader("// Cascaded ShadowMap");
      //matTemplate->addPixelHeader("#include <shadows.tsh>");
      //matTemplate->addPixelHeader("SAMPLER2DSHADOW(Texture0, 0); // ShadowMap");
      //matTemplate->addPixelHeader("uniform mat4 u_shadowMtx[4];");
      //matTemplate->addPixelHeader("uniform vec4 u_shadowParams;");

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

         matTemplate->addPixelBody("    vec3 normal = %s * 2.0 - 1.0;", normalNode->getPixelReference(settings, ReturnVec3));
         matTemplate->addPixelBody("    vec3 n_tang = normalize(v_tangent.xyz);");
         matTemplate->addPixelBody("    vec3 n_bitang = normalize(v_bitangent.xyz);");
         matTemplate->addPixelBody("    vec3 n_norm = normalize(v_normal.xyz);");
         matTemplate->addPixelBody("    mat3 tbn = getTBN(n_tang, n_bitang, n_norm);");
         matTemplate->addPixelBody("    normal = normalize(mul(tbn, normal) );");

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
      matTemplate->addPixelBody("    vec3 reflectivity = vec3_splat(%s);", metallicVal);
      matTemplate->addPixelBody("    reflectivity = clamp(reflectivity, 0.0, 0.999);");
      matTemplate->addPixelBody("    vec3 surfaceReflect = mix(vec3_splat(0.04), baseColor, reflectivity);");
      matTemplate->addPixelBody("    vec3 surfaceColor = baseColor * (vec3_splat(1.0) - reflectivity);");

      // Directional Light
      matTemplate->addPixelBody("");
      matTemplate->addPixelBody("    // Directional Light + Shadows");
      matTemplate->addPixelBody("    float shadow = 1.0f; // getShadow(surface, Texture0, u_shadowMtx, u_shadowParams);");
      matTemplate->addPixelBody("    vec3 dirLight = getDirectionalLight(surface, viewDir, dirLightDirection.xyz, dirLightColor.rgb, shadow);");

      // Ambient Light
      matTemplate->addPixelBody("");
      matTemplate->addPixelBody("    // Ambient Light");
      matTemplate->addPixelBody("    vec3 ambLight = vec3(0.0, 0.0, 0.0);");

      // Final Output
      matTemplate->addPixelBody("");
      matTemplate->addPixelBody("    // Final Output");
      if (emissiveSet)
         matTemplate->addPixelBody("    gl_FragColor = encodeRGBE8(baseColor);");
      else
         matTemplate->addPixelBody("    gl_FragColor = encodeRGBE8((dirLight * surfaceColor) + ambLight);");
   }
}