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

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _GRAPHICS_UTILITIES_H_
#include "graphics/core.h"
#endif

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/shaders.h"
#endif

#ifndef _SHADERS_H_
#include "graphics/shaders.h"
#endif

#ifndef _RENDERINGCOMMON_H_
#include "3d/rendering/common.h"
#endif

namespace Scene 
{
   class MaterialTemplate;

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

         const char* type;
         char mReturnBuf[64];
         bool isRootNode;
         Point2I mPosition;

         BaseNode() 
         { 
            mPosition.set(400, 300);
            type = "Base";
            isRootNode = false; 
            mReturnBuf[0] = '\0'; 
         }

         virtual bool onAdd() { return Parent::onAdd(); }
         virtual void onRemove() { Parent::onRemove(); }

         virtual void generateVertex(MaterialTemplate* matTemplate, ReturnType refType = ReturnName) { }
         virtual const char* getVertexReference(MaterialTemplate* matTemplate, ReturnType refType);
         
         virtual void generatePixel(MaterialTemplate* matTemplate, ReturnType refType = ReturnName) { }
         virtual const char* getPixelReference(MaterialTemplate* matTemplate, ReturnType refType);

         virtual Graphics::ViewTableEntry* getRenderView() { return Graphics::getView("RenderLayer0"); }
         virtual BaseNode* findNode(MaterialTemplate* matTemplate, const char* name);

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

         bool isSkinned;
         Rendering::UniformSet uniforms;

         void addObject(SimObject* obj);

         void clearVertex();
         void addVertexHeader(const char* text);
         void addVertexInput(const char* text);
         void addVertexOutput(const char* text);
         void addVertexBody(const char* text);

         void clearPixel();
         void addPixelHeader(const char* text);
         void addPixelBody(const char* text);

         Graphics::ViewTableEntry* getRenderView();
         Scene::BaseNode* getRootNode();
         const char* getVertexShaderOutput();
         const char* getPixelShaderOutput();

         DECLARE_CONOBJECT(MaterialTemplate);
   };
}

#endif // _MATERIAL_TEMPLATE_H_