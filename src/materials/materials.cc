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
#include "materials.h"
#include "graphics/core.h"
#include "scene/components/baseComponent.h"

#include "materialAsset.h"
#include "materialTemplate.h"

namespace Materials
{
   MaterialAsset* getMaterialAsset(const char* id)
   {
      AssetPtr<MaterialAsset> result;
      StringTableEntry assetId = StringTable->insert(id);
      result.setAssetId(assetId);
      return result;
   }

   void createMaterialAsset(const char* name, const char* templateFile, const char* savePath)
   {
      MaterialAsset* newAsset = new MaterialAsset();
      newAsset->setAssetName(name);
      newAsset->setTemplateFile(StringTable->insert(templateFile));

      Platform::createPath(savePath);

      // Save the module file.
      Taml taml;
      taml.write(newAsset, savePath);

      SAFE_DELETE(newAsset);
   }

   void compileAllMaterials(bool recompile)
   {
      // Find all material assets.
      AssetQuery assQuery;
      AssetDatabase.findAssetType(&assQuery, "MaterialAsset", false);
      for (S32 n = 0; n < assQuery.size(); n++)
      {
         StringTableEntry assetID = assQuery[n];
         MaterialAsset* material = getMaterialAsset(assetID);
         material->compileMaterial(recompile);
      }
   }

   void createMaterialTemplate(const char* savePath)
   {
      Materials::MaterialTemplate* newTemplate = new Materials::MaterialTemplate();

      Platform::createPath(savePath);

      // Save the module file.
      Taml taml;
      taml.write(newTemplate, savePath);

      SAFE_DELETE(newTemplate);
   }

   Vector<const char*> sVariantList;
   Vector<Materials::BaseNode*> sVariantNodes;

   void addVariantNode(const char* variant, BaseNode* node)
   {
      if (getVariantIndex(variant) < 0)
         sVariantList.push_back(variant);

      sVariantNodes.push_back(node);
      node->variant = variant;
   }

   S32 getVariantIndex(const char* variant)
   {
      for (S32 n = 0; n < sVariantList.size(); ++n)
      {
         if (dStrcmp(sVariantList[n], variant) == 0)
            return n;
      }

      return -1;
   }

   Vector<const char*> getVariants()
   {
      return sVariantList;
   }

   void copyMaterialNode(BaseNode* source, BaseNode* destination)
   {
      AbstractClassRep::FieldList fieldList = source->getFieldList();
      for (Vector<AbstractClassRep::Field>::iterator itr = fieldList.begin(); itr != fieldList.end(); itr++)
      {
         const AbstractClassRep::Field* f = itr;

         for (U32 j = 0; S32(j) < f->elementCount; j++)
         {
            const char *val = (*f->getDataFn)(source, Con::getData(f->type, (void *)(((const char *)source) + f->offset), j, f->table, f->flag));
            destination->setField(f->pFieldname, val);
         }
      }
   }

   BaseNode* getVariantNode(const char* variant, BaseNode* source)
   {
      if (source == NULL)
         return source;

      for (S32 n = 0; n < sVariantNodes.size(); ++n)
      {
         if ((dStrcmp(sVariantNodes[n]->variant, variant) == 0) && (dStrcmp(sVariantNodes[n]->type, source->type) == 0))
         {
            copyMaterialNode(source, sVariantNodes[n]);
            return sVariantNodes[n];
         }
      }

      return source;
   }
}