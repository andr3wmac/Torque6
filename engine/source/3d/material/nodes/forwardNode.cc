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
#include "forwardNode.h"
#include "textureNode.h"

namespace Scene 
{
   IMPLEMENT_CONOBJECT(ForwardNode);

   ForwardNode::ForwardNode()
   {
      type = "Forward";
      mColorSrc = StringTable->insert("");
      mLit = false;
   }

   void ForwardNode::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("ColorSrc", TypeString, Offset(mColorSrc, ForwardNode), "");
      addField("Lit", TypeBool, Offset(mLit, ForwardNode), "");
   }

   void ForwardNode::generateVertex(MaterialTemplate* matTemplate, ReturnType refType)
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
      matTemplate->addVertexOutput("v_wpos");

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

      matTemplate->addVertexBody("");
      matTemplate->addVertexBody("    // UV Coordinates");
      matTemplate->addVertexBody("    v_texcoord0 = a_texcoord0;");

      matTemplate->addVertexBody("");
      matTemplate->addVertexBody("    // Normal, Tangent, Bitangent");
	   matTemplate->addVertexBody("    v_normal = mul(modelTransform, vec4(a_normal.xyz, 0.0) ).xyz;");
	   matTemplate->addVertexBody("    v_tangent = mul(modelTransform, vec4(a_tangent.xyz, 0.0) ).xyz;");
	   matTemplate->addVertexBody("    v_bitangent = mul(modelTransform, vec4(a_bitangent.xyz, 0.0) ).xyz;");

      if ( mLit )
      {
         matTemplate->addVertexOutput("v_shadowcoord");
         matTemplate->addVertexHeader("uniform mat4 u_lightMtx;");

         matTemplate->addVertexBody("");
         matTemplate->addVertexBody("    // World-Space Position (used for lighting)");
         matTemplate->addVertexBody("    vec3 wpos = mul(modelTransform, vertPosition).xyz;");
         matTemplate->addVertexBody("    v_wpos = wpos;");

         matTemplate->addVertexBody("");
	      matTemplate->addVertexBody("    // Shadow Map Coord, used to receive shadows");
	      matTemplate->addVertexBody("    const float shadowMapOffset = 0.001;");
	      matTemplate->addVertexBody("    vec3 posOffset = a_position + a_normal.xyz * shadowMapOffset;");
	      matTemplate->addVertexBody("    v_shadowcoord = mul(u_lightMtx, vec4(posOffset, 1.0));");
      }

      matTemplate->addVertexBody("");
      matTemplate->addVertexBody("    // Output Final Vertex Position");
      matTemplate->addVertexBody("    gl_Position = mul(u_modelViewProj, vertPosition);");
   }

   void ForwardNode::generatePixel(MaterialTemplate* matTemplate, ReturnType refType)
   {
      Parent::generatePixel(matTemplate, refType);

      // Color Output
      matTemplate->addPixelBody("    // Color");
      BaseNode* colorSource = findNode(matTemplate, mColorSrc);
      if ( colorSource != NULL )
      {
         colorSource->generatePixel(matTemplate, ReturnVec3);

         char texOut[128];
         dSprintf(texOut, 128, "    vec4 outputColor = vec4(toLinear(%s), 1.0);", colorSource->getPixelReference(matTemplate, ReturnVec3));
         matTemplate->addPixelBody(texOut);
      } else {
         matTemplate->addPixelBody("    vec4 outputColor = vec4(1.0, 0.0, 0.0, 1.0);");
      }

      if ( mLit )
      {
         matTemplate->addPixelHeader("#include <forwardLighting.sc>");
         matTemplate->addPixelHeader("uniform vec4 u_camPos;");

         matTemplate->addPixelBody("    // Compute forward lighting.");
         matTemplate->addPixelBody("    vec3 viewDir = normalize(u_camPos.xyz - v_wpos);");
         matTemplate->addPixelBody("    vec3 lightColor = computeForwardLighting(v_wpos.xyz, viewDir, v_normal.xyz, v_shadowcoord);");
         matTemplate->addPixelBody("    gl_FragColor = encodeRGBE8(outputColor.rgb * lightColor.rgb * 1000.0);");
      } else {
         matTemplate->addPixelBody("    gl_FragColor = encodeRGBE8(outputColor.rgb * 1000.0);");
      }
   }
}