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
#include "deferredNode.h"
#include "textureNode.h"

namespace Scene 
{
   IMPLEMENT_CONOBJECT(DeferredNode);

   DeferredNode::DeferredNode()
   {
      type = "Deferred";
      mColorSrc = StringTable->insert("");
      mNormalSrc = StringTable->insert("");
      mMetallicSrc = StringTable->insert("");
      mRoughnessSrc = StringTable->insert("");
      mEmissiveSrc = StringTable->insert("");
      mWorldPosOffsetSrc = StringTable->insert("");
   }

   void DeferredNode::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("ColorSrc", TypeString, Offset(mColorSrc, DeferredNode), "");
      addField("NormalSrc", TypeString, Offset(mNormalSrc, DeferredNode), "");
      addField("MetallicSrc", TypeString, Offset(mMetallicSrc, DeferredNode), "");
      addField("RoughnessSrc", TypeString, Offset(mRoughnessSrc, DeferredNode), "");
      addField("WorldPosOffsetSrc", TypeString, Offset(mWorldPosOffsetSrc, DeferredNode), "");
   }

   void DeferredNode::generateVertex(MaterialTemplate* matTemplate, ReturnType refType)
   {
      Parent::generateVertex(matTemplate, refType);

      matTemplate->addVertexInput("a_position");
      matTemplate->addVertexInput("a_normal");
      matTemplate->addVertexInput("a_tangent");
      matTemplate->addVertexInput("a_bitangent");
      matTemplate->addVertexInput("a_texcoord0");

      matTemplate->addVertexOutput("v_normal");
      matTemplate->addVertexOutput("v_tangent");
      matTemplate->addVertexOutput("v_bitangent");
      matTemplate->addVertexOutput("v_texcoord0");

      matTemplate->addVertexBody("    // Vertex Position");
      matTemplate->addVertexBody("    vec4 vertPosition = vec4(a_position, 1.0);");
      matTemplate->addVertexBody("    mat4 modelTransform = u_model[0];");

      if ( matTemplate->isSkinned )
      {
         matTemplate->addVertexInput("a_indices");
         matTemplate->addVertexInput("a_weight");

         matTemplate->addVertexBody("");
         matTemplate->addVertexBody("    // Skinning");
         matTemplate->addVertexBody("    modelTransform =    mul(u_model[int(a_indices[0])], a_weight[0]);");
         matTemplate->addVertexBody("    modelTransform +=   mul(u_model[int(a_indices[1])], a_weight[1]);");
         matTemplate->addVertexBody("    modelTransform +=   mul(u_model[int(a_indices[2])], a_weight[2]);");
         matTemplate->addVertexBody("    modelTransform +=   mul(u_model[int(a_indices[3])], a_weight[3]); ");   
         matTemplate->addVertexBody("    vertPosition =      mul(modelTransform, vertPosition);");
      }

      // World Position Offset Source
      const char* wpoValue = "vec3(0.0, 0.0, 0.0)";
      BaseNode* wpoNode = findNode(matTemplate, mWorldPosOffsetSrc);
      if ( wpoNode )
      {
         wpoNode->generateVertex(matTemplate, ReturnVec3);
         wpoValue = wpoNode->getVertexReference(matTemplate, ReturnVec3);
      }

      char wpoOut[128];
      dSprintf(wpoOut, 128, "    vertPosition += vec4(%s, 0.0);", wpoValue);
      matTemplate->addVertexBody("");
      matTemplate->addVertexBody("    // World Position Offset");
      matTemplate->addVertexBody(wpoOut);

      matTemplate->addVertexBody("");
      matTemplate->addVertexBody("    // UV Coordinates");
      matTemplate->addVertexBody("    v_texcoord0 = a_texcoord0;");

      matTemplate->addVertexBody("");
      matTemplate->addVertexBody("    // Normal, Tangent, Bitangent");
	   matTemplate->addVertexBody("    v_normal = mul(modelTransform, vec4(a_normal.xyz, 0.0) ).xyz;");
	   matTemplate->addVertexBody("    v_tangent = mul(modelTransform, vec4(a_tangent.xyz, 0.0) ).xyz;");
	   matTemplate->addVertexBody("    v_bitangent = mul(modelTransform, vec4(a_bitangent.xyz, 0.0) ).xyz;");

      matTemplate->addVertexBody("");
      matTemplate->addVertexBody("    // Output Final Vertex Position");
      matTemplate->addVertexBody("    gl_Position = mul(u_modelViewProj, vertPosition);");
   }

   void DeferredNode::generatePixel(MaterialTemplate* matTemplate, ReturnType refType)
   {
      Parent::generatePixel(matTemplate);

      // Color Source
      const char* colorVal = "vec3(1.0, 1.0, 1.0)";
      BaseNode* colorNode = findNode(matTemplate, mColorSrc);
      if ( colorNode != NULL )
      {
         colorNode->generatePixel(matTemplate, ReturnVec3);
         colorVal = colorNode->getPixelReference(matTemplate, ReturnVec3);
      }

      // Emissive Source
      bool emissiveSet = false;
      char emissiveTex[256] = "vec4(0.0, 0.0, 0.0, 0.0)";
      const char* emissiveVal = "vec3(0.0, 0.0, 0.0)";
      BaseNode* emissiveNode = findNode(matTemplate, mEmissiveSrc);
      if ( emissiveNode != NULL )
      {
         emissiveSet = true;
         emissiveNode->generatePixel(matTemplate, ReturnVec4);
         dStrcpy(emissiveTex, emissiveNode->getPixelReference(matTemplate, ReturnName));
         emissiveVal = emissiveNode->getPixelReference(matTemplate, ReturnVec3);
      }

      // Base Color Output = Color + Emissive
      matTemplate->addPixelBody("");
      matTemplate->addPixelBody("    // Base Color");
      char colorOut[256];
      dSprintf(colorOut, 256, "    gl_FragData[0] = encodeRGBE8(%s + %s + vec3(0.00001, 0.00001, 0.00001));", colorVal, emissiveVal);
      matTemplate->addPixelBody(colorOut);
      
      // Normal Source
      const char* normalVal = "v_normal";
      matTemplate->addPixelBody("");
      matTemplate->addPixelBody("    // Normals");
      BaseNode* normalNode = findNode(matTemplate, mNormalSrc);
      if ( normalNode != NULL )
      {
         normalNode->generatePixel(matTemplate, ReturnVec3);

         char normalMapSampleOut[256];
         dSprintf(normalMapSampleOut, 256, "    vec3 normal = %s * 2.0 - 1.0;", normalNode->getPixelReference(matTemplate, ReturnVec3));
         matTemplate->addPixelBody(normalMapSampleOut);

	      matTemplate->addPixelBody("    vec3 n_tang = normalize(v_tangent.xyz);");
	      matTemplate->addPixelBody("    vec3 n_bitang = normalize(v_bitangent.xyz);");
         matTemplate->addPixelBody("    vec3 n_norm = normalize(v_normal.xyz);");
	      matTemplate->addPixelBody("    mat3 tbn = getTBN(n_tang, n_bitang, n_norm);");
         matTemplate->addPixelBody("    normal = normalize(mul(tbn, normal) );");

         normalVal = "normal";
      }

      // Normal Output
      char normalOut[128];
      dSprintf(normalOut, 128, "    gl_FragData[1] = vec4(encodeNormalUint(%s), 1.0);", normalVal);
      matTemplate->addPixelBody(normalOut);

      // Metallic Source
      const char* metallicVal = "0.0";
      BaseNode* metallicNode = findNode(matTemplate, mMetallicSrc);
      if ( metallicNode )
      {
         metallicNode->generatePixel(matTemplate, ReturnFloat);
         metallicVal = metallicNode->getPixelReference(matTemplate, ReturnFloat);
      }

      // Roughness Source
      const char* roughnessVal = "0.0";
      BaseNode* roughnessNode = findNode(matTemplate, mRoughnessSrc);
      if ( roughnessNode )
      {
         roughnessNode->generatePixel(matTemplate, ReturnFloat);
         roughnessVal = roughnessNode->getPixelReference(matTemplate, ReturnFloat);
      }

      // Material Info Output
      // R = Metallic
      // G = Roughness
      // B = Specular
      // A = Emissive (will be flags later)
      char matInfoOut[128];
      if ( emissiveSet )
         dSprintf(matInfoOut, 128, "    gl_FragData[2] = vec4(%s, %s, 0.5, %s.a);", metallicVal, roughnessVal, emissiveTex);
      else
         dSprintf(matInfoOut, 128, "    gl_FragData[2] = vec4(%s, %s, 0.5, 0.0);", metallicVal, roughnessVal);

      matTemplate->addPixelBody("");
      matTemplate->addPixelBody("    // Material Info");
      matTemplate->addPixelBody(matInfoOut);
   }
}