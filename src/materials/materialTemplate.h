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

#ifndef _MATERIAL_TEMPLATE_H_
#define _MATERIAL_TEMPLATE_H_

#ifndef _RENDERING_H_
#include "rendering/rendering.h"
#endif

namespace Materials
{
   class MaterialTemplate;
   class MaterialVariant;

   struct MaterialGenerationSettings
   {
      MaterialTemplate* matTemplate;
      const char*       matVariant;
      bool              isSkinned;

      MaterialGenerationSettings()
         : matTemplate(NULL),
           matVariant(NULL),
           isSkinned(false) { }
   };

   class DLL_PUBLIC BaseNode : public SimObject
   {
      private:
         typedef SimObject Parent;

      public:
         enum ReturnType
         {
            ReturnName,
            ReturnFloat,
            ReturnVec2,
            ReturnVec3,
            ReturnVec4
         };

         const char* variant;
         const char* type;

         char mReturnBuf[64];
         bool isRootNode;
         Point2I mPosition;

         BaseNode() 
         { 
            variant = "";
            type = "Base";

            mReturnBuf[0] = '\0';
            isRootNode = false; 

            mPosition.set(400, 300);
         }

         virtual bool onAdd() { return Parent::onAdd(); }
         virtual void onRemove() { Parent::onRemove(); }

         virtual void generateVertex(const MaterialGenerationSettings &settings, ReturnType refType = ReturnName) { }
         virtual const char* getVertexReference(const MaterialGenerationSettings &settings, ReturnType refType);
         
         virtual void generatePixel(const MaterialGenerationSettings &settings, ReturnType refType = ReturnName) { }
         virtual const char* getPixelReference(const MaterialGenerationSettings &settings, ReturnType refType);

         virtual BaseNode* findNode(const MaterialGenerationSettings &settings, const char* name);

         static void initPersistFields();

         DECLARE_CONOBJECT(BaseNode);
   };

   class MaterialTemplate : public SimGroup
   {
      private:
         typedef SimGroup Parent;

         char vertexShaderFinal[10000];

         char vertexShaderInputs[500];
         U32 vertexShaderInputsPos;
         char vertexShaderOutputs[500];
         U32 vertexShaderOutputsPos;

         char vertexShaderHeader[5000];
         U32 vertexShaderHeaderPos;
         char vertexShaderBody[5000];
         U32 vertexShaderBodyPos;

         char pixelShaderFinal[10000];

         char pixelShaderHeader[5000];
         U32 pixelShaderHeaderPos;
         char pixelShaderBody[5000];
         U32 pixelShaderBodyPos;

      public:
         MaterialTemplate();
         ~MaterialTemplate();

         Rendering::UniformSet uniforms;

         void addObject(SimObject* obj);

         void clearVertex();
         void addVertexHeader(const char *format, ...);
         void addVertexInput(const char *format, ...);
         void addVertexOutput(const char *format, ...);
         void addVertexBody(const char *format, ...);

         void clearPixel();
         void addPixelHeader(const char *format, ...);
         void addPixelBody(const char *format, ...);

         Materials::BaseNode* getRootNode(const MaterialGenerationSettings &settings);
         const char* getVertexShaderOutput(const MaterialGenerationSettings &settings);
         const char* getPixelShaderOutput(const MaterialGenerationSettings &settings);

         DECLARE_CONOBJECT(MaterialTemplate);
   };
}

#endif // _MATERIAL_TEMPLATE_H_