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
#include "console/consoleInternal.h"
#include "materialTemplate.h"
#include "graphics/core.h"
#include "3d/entity/components/baseComponent.h"

// Script bindings.
#include "materialTemplate_ScriptBinding.h"

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
   IMPLEMENT_CONOBJECT(BaseNode);

   void BaseNode::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("Position", TypePoint2I, Offset(mPosition, BaseNode), "");
   }

   BaseNode* BaseNode::findNode(MaterialTemplate* matTemplate, const char* name)
   {
      if ( dStrlen(name) < 1 )
         return NULL;

      BaseNode* result = dynamic_cast<BaseNode*>(matTemplate->findObjectByInternalName(name));
      return result;
   }

   const char* BaseNode::getVertexReference(MaterialTemplate* matTemplate, ReturnType refType)
   {
      switch(refType)
      {
         case ReturnFloat:
            return "0.0";
         case ReturnVec2:
            return "vec2(0.0, 0.0)";
         case ReturnVec3:
            return "vec3(0.0, 0.0, 0.0)";
         case ReturnVec4:
            return "vec4(0.0, 0.0, 0.0, 0.0)";
      }
      return "";
   }

   const char* BaseNode::getPixelReference(MaterialTemplate* matTemplate, ReturnType refType)
   {
      switch(refType)
      {
         case ReturnFloat:
            return "0.0";
         case ReturnVec2:
            return "vec2(0.0, 0.0)";
         case ReturnVec3:
            return "vec3(0.0, 0.0, 0.0)";
         case ReturnVec4:
            return "vec4(0.0, 0.0, 0.0, 0.0)";
      }
      return "";
   }

   IMPLEMENT_CONOBJECT(MaterialTemplate);

   MaterialTemplate::MaterialTemplate()
   {
      isSkinned = false;
      clearVertex();
      clearPixel();
   }

   MaterialTemplate::~MaterialTemplate()
   {
      //
   }

   void MaterialTemplate::addObject(SimObject* obj)
   {
      Parent::addObject(obj);
   }

   Scene::BaseNode* MaterialTemplate::getRootNode()
   {
      for( S32 n = 0; n < this->size(); ++n )
      {
         Scene::BaseNode* node = dynamic_cast<Scene::BaseNode*>(this->at(n));
         if ( !node->isRootNode ) continue;
         return node;
      }

      return NULL;
   }

   Graphics::ViewTableEntry* MaterialTemplate::getRenderView()
   {
      Scene::BaseNode* rootNode = getRootNode();
      return rootNode->getRenderView();
   }

   const char* MaterialTemplate::getVertexShaderOutput()
   {
      Scene::BaseNode* rootNode = getRootNode();
      rootNode->generateVertex(this);

      U32 pos = 0;

      dStrcpy(vertexShaderFinal, "$input ");
      pos += dStrlen("$input ");
      vertexShaderInputs[vertexShaderInputsPos - 2] = ' ';
      vertexShaderInputs[vertexShaderInputsPos - 1] = '\n';
      dStrcpy(&vertexShaderFinal[pos], vertexShaderInputs);
      pos += vertexShaderInputsPos;

      dStrcpy(&vertexShaderFinal[pos], "$output ");
      pos += dStrlen("$output ");
      vertexShaderOutputs[vertexShaderOutputsPos - 2] = ' ';
      vertexShaderOutputs[vertexShaderOutputsPos - 1] = '\n';
      dStrcpy(&vertexShaderFinal[pos], vertexShaderOutputs);
      pos += vertexShaderOutputsPos;

      dStrcpy(&vertexShaderFinal[pos], vertexShaderHeader);
      pos += vertexShaderHeaderPos;

      dStrcpy(&vertexShaderFinal[pos], "void main()\n{");
      pos += dStrlen("void main()\n{");

      dStrcpy(&vertexShaderFinal[pos], vertexShaderBody);
      pos += vertexShaderBodyPos;

      dStrcpy(&vertexShaderFinal[pos], "}");
      pos += dStrlen("}");

      return vertexShaderFinal;
   }

   const char* MaterialTemplate::getPixelShaderOutput()
   {
      Scene::BaseNode* rootNode = getRootNode();
      rootNode->generatePixel(this);

      U32 pos = 0;

      dStrcpy(pixelShaderFinal, "$input ");
      pos += dStrlen("$input ");
      vertexShaderOutputs[vertexShaderOutputsPos - 2] = ' ';
      vertexShaderOutputs[vertexShaderOutputsPos - 1] = '\n';
      dStrcpy(&pixelShaderFinal[pos], vertexShaderOutputs);
      pos += vertexShaderOutputsPos;

      dStrcpy(&pixelShaderFinal[pos], pixelShaderHeader);
      pos += pixelShaderHeaderPos;

      dStrcpy(&pixelShaderFinal[pos], "void main()\n{");
      pos += dStrlen("void main()\n{");

      dStrcpy(&pixelShaderFinal[pos], pixelShaderBody);
      pos += pixelShaderBodyPos;

      dStrcpy(&pixelShaderFinal[pos], "}");
      pos += dStrlen("}");

      return pixelShaderFinal;
   }

   void MaterialTemplate::clearVertex()
   {
      vertexShaderInputs[0] = '\0';
      vertexShaderInputsPos = 0;
      vertexShaderOutputs[0] = '\0';
      vertexShaderOutputsPos = 0;

      vertexShaderHeader[0] = '\0';
      vertexShaderHeaderPos = 0;
      vertexShaderBody[0] = '\0';
      vertexShaderBodyPos = 0;
   }

   void MaterialTemplate::addVertexHeader(const char* text)
   {
      dStrcpy(&vertexShaderHeader[vertexShaderHeaderPos], text);
      vertexShaderHeaderPos += dStrlen(text);
      dStrcpy(&vertexShaderHeader[vertexShaderHeaderPos], "\n");
      vertexShaderHeaderPos += dStrlen("\n");
   }

   void MaterialTemplate::addVertexInput(const char* text)
   {
      dStrcpy(&vertexShaderInputs[vertexShaderInputsPos], text);
      vertexShaderInputsPos += dStrlen(text);
      dStrcpy(&vertexShaderInputs[vertexShaderInputsPos], ", ");
      vertexShaderInputsPos += dStrlen(", ");
   }

   void MaterialTemplate::addVertexOutput(const char* text)
   {
      dStrcpy(&vertexShaderOutputs[vertexShaderOutputsPos], text);
      vertexShaderOutputsPos += dStrlen(text);
      dStrcpy(&vertexShaderOutputs[vertexShaderOutputsPos], ", ");
      vertexShaderOutputsPos += dStrlen(", ");
   }

   void MaterialTemplate::addVertexBody(const char* text)
   {
      dStrcpy(&vertexShaderBody[vertexShaderBodyPos], text);
      vertexShaderBodyPos += dStrlen(text);
      dStrcpy(&vertexShaderBody[vertexShaderBodyPos], "\n");
      vertexShaderBodyPos += dStrlen("\n");
   }

   void MaterialTemplate::clearPixel()
   {      
      pixelShaderHeader[0] = '\0';
      pixelShaderHeaderPos = 0;
      pixelShaderBody[0] = '\0';
      pixelShaderBodyPos = 0;
   }

   void MaterialTemplate::addPixelHeader(const char* text)
   {
      dStrcpy(&pixelShaderHeader[pixelShaderHeaderPos], text);
      pixelShaderHeaderPos += dStrlen(text);
      dStrcpy(&pixelShaderHeader[pixelShaderHeaderPos], "\n");
      pixelShaderHeaderPos += dStrlen("\n");
   }

   void MaterialTemplate::addPixelBody(const char* text)
   {
      dStrcpy(&pixelShaderBody[pixelShaderBodyPos], text);
      pixelShaderBodyPos += dStrlen(text);
      dStrcpy(&pixelShaderBody[pixelShaderBodyPos], "\n");
      pixelShaderBodyPos += dStrlen("\n");
   }
}