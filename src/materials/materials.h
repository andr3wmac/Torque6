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

#ifndef _MATERIALS_H_
#define _MATERIALS_H_

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

//-----------------------------------------------------------------------------
// The Material System
//-----------------------------------------------------------------------------

// Materials are node based and generate shaders from the node connections.
// There are root nodes which define the actual structure and content of the 
// output shader and then they follow the nodes based on named connections
// until they resolve as deep as they have to go. At that point the shader
// is written to disk, compiled, and loaded.

// MaterialAsset is the Torque asset class for working with materials. It
// defines a path to the template file which creates the MaterialTemplate.
// MaterialAsset also loads and defines the textures for the material.

// MaterialTemplates are a collection of named nodes. Multiple materials can 
// use same template and repurpose it with different texture inputs.

// A "Material Variant Node" is a node that overrides another node during 
// material generation. A list of variants is generated based on the variant
// names passed in when variant nodes are defined. They must be defined with
// special macros in the definition body and in the implementation file.
//   Example:

// In .h file:
// class DeferredOpaqueNode : public OpaqueNode
// {
//    private:
//       typedef RootNode Parent;
//    ..
//
//    public:
//       DECLARE_MATERIAL_VARIANT_NODE("deferred", DeferredOpaqueNode);
//    ..
// };

// In .cc file:
// IMPLEMENT_MATERIAL_VARIANT_NODE("deferred", DeferredOpaqueNode);

// This creates a material variation known as "deferred" and when the material
// system compiles all the materials it will do an additional pass of shaders
// prefixed with deferred and any occurrence of the OpaqueNode will be
// substituted with a DeferredOpaqueNode with field arguments copied over.

class MaterialAsset;

namespace Materials
{
   class BaseNode;
   class MaterialTemplate;

   // Material Templates
   void createMaterialTemplate(const char* savePath);

   // Material Assets
   void createMaterialAsset(const char* name, const char* templateFile, const char* savePath);
   MaterialAsset* getMaterialAsset(const char* id);
   
   // Material Variant Nodes
   void addVariantNode(const char* variant, BaseNode* node);
   S32 getVariantIndex(const char* variant);
   Vector<const char*> getVariants();
   BaseNode* getVariantNode(const char* variant, BaseNode* source);

   // Compilation
   void compileAllMaterials(bool recompile = false);
}

class DLL_PUBLIC _MaterialVariantNodeRegister_
{
   public:
      _MaterialVariantNodeRegister_(const char* variant, Materials::BaseNode* node)
      {
         Materials::addVariantNode(variant, node);
      }
};

#define IMPLEMENT_MATERIAL_VARIANT_NODE(variantName, variantNode)                                           \
    variantNode variantNode::variantNode##Inst;                                                             \
    _MaterialVariantNodeRegister_ variantNode::variantNode##Register(variantName, &variantNode::variantNode##Inst)

#define DECLARE_MATERIAL_VARIANT_NODE(variantName, variantNode)   \
   static variantNode variantNode##Inst;                          \
   static _MaterialVariantNodeRegister_ variantNode##Register

#endif // _MATERIALS_H_