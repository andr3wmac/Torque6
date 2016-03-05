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
#include "opaqueNode.h"
#include "textureNode.h"
#include "opaqueNode_Binding.h"

namespace Materials
{
   IMPLEMENT_CONOBJECT(OpaqueNode);

   OpaqueNode::OpaqueNode()
   {
      type                 = "Opaque";

      mColorSrc            = StringTable->insert("");
      mNormalSrc           = StringTable->insert("");
      mMetallicSrc         = StringTable->insert("");
      mRoughnessSrc        = StringTable->insert("");
      mEmissiveSrc         = StringTable->insert("");
      mWorldPosOffsetSrc   = StringTable->insert("");
      mAlphaThreshold      = 0.0f;
   }

   void OpaqueNode::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("ColorSrc", TypeString, Offset(mColorSrc, OpaqueNode), "");
      addField("EmissiveSrc", TypeString, Offset(mEmissiveSrc, OpaqueNode), "");
      addField("NormalSrc", TypeString, Offset(mNormalSrc, OpaqueNode), "");
      addField("MetallicSrc", TypeString, Offset(mMetallicSrc, OpaqueNode), "");
      addField("RoughnessSrc", TypeString, Offset(mRoughnessSrc, OpaqueNode), "");
      addField("WorldPosOffsetSrc", TypeString, Offset(mWorldPosOffsetSrc, OpaqueNode), "");

      addField("AlphaThreshold", TypeF32, Offset(mAlphaThreshold, OpaqueNode), "");
   }

   void OpaqueNode::generateVertex(const MaterialGenerationSettings &settings, ReturnType refType)
   {
      Parent::generateVertex(settings, refType);

      MaterialTemplate* matTemplate = settings.matTemplate;

      matTemplate->addVertexInput("a_position");
      matTemplate->addVertexInput("a_normal");
      matTemplate->addVertexInput("a_tangent");
      matTemplate->addVertexInput("a_bitangent");
      matTemplate->addVertexInput("a_texcoord0");

      matTemplate->addVertexOutput("v_position");
      matTemplate->addVertexOutput("v_normal");
      matTemplate->addVertexOutput("v_tangent");
      matTemplate->addVertexOutput("v_bitangent");
      matTemplate->addVertexOutput("v_texcoord0");

      matTemplate->addVertexBody("    // Vertex Position");
      matTemplate->addVertexBody("    vec4 vertPosition = vec4(a_position, 1.0);");
      matTemplate->addVertexBody("    mat4 modelTransform = u_model[0];");

      if ( settings.isSkinned )
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
      BaseNode* wpoNode = findNode(settings, mWorldPosOffsetSrc);
      if ( wpoNode )
      {
         wpoNode->generateVertex(settings, ReturnVec3);
         wpoValue = wpoNode->getVertexReference(settings, ReturnVec3);
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
      matTemplate->addVertexBody("    v_position = gl_Position;");
   }

   void OpaqueNode::generatePixel(const MaterialGenerationSettings &settings, ReturnType refType)
   {
      Parent::generatePixel(settings, refType);

      MaterialTemplate* matTemplate = settings.matTemplate;

      // Color Source
      const char* colorVal = "vec4(1.0, 1.0, 1.0, 1.0)";
      BaseNode* colorNode = findNode(settings, mColorSrc);
      if ( colorNode != NULL )
      {
         colorNode->generatePixel(settings, ReturnVec4);
         colorVal = colorNode->getPixelReference(settings, ReturnVec4);
      }

      // Emissive Source
      bool emissiveSet = false;
      char emissiveTex[256] = "vec4(0.0, 0.0, 0.0, 0.0)";
      const char* emissiveVal = "vec4(0.0, 0.0, 0.0, 0.0)";
      BaseNode* emissiveNode = findNode(settings, mEmissiveSrc);
      if ( emissiveNode != NULL )
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
         char alphaThresholdOut[256];
         dSprintf(alphaThresholdOut, 256, "    if (%s.a < %f) discard;", colorVal, mAlphaThreshold);
         matTemplate->addPixelBody(alphaThresholdOut);
      }

      // Base Color Output = Color + Emissive
      char colorOut[256];
      dSprintf(colorOut, 256, "    gl_FragColor = encodeRGBE8(%s.rgb + %s.rgb + vec3(0.00001, 0.00001, 0.00001));", colorVal, emissiveVal);
      matTemplate->addPixelBody(colorOut);
   }
}