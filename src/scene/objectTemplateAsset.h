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

#ifndef _OBJECT_TEMPLATE_ASSET_H_
#define _OBJECT_TEMPLATE_ASSET_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _VERTEXLAYOUTS_H_
#include "graphics/core.h"
#endif

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

#ifndef _SHADERS_H_
#include "graphics/shaders.h"
#endif

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

#ifndef _OBJECT_TEMPLATE_H_
#include <scene/objectTemplate.h>
#endif

//-----------------------------------------------------------------------------

DefineConsoleType( TypeObjectTemplateAssetPtr )

//-----------------------------------------------------------------------------

class ObjectTemplateAsset : public AssetBase
{

private:
   typedef AssetBase Parent;

   StringTableEntry mTemplateFile;

public:
   ObjectTemplateAsset();
   ~ObjectTemplateAsset();

   Scene::ObjectTemplate* getInstance();

   StringTableEntry getTemplateFile() { return mTemplateFile; }
   void setTemplateFile(const char* file) { mTemplateFile = file; }

   static void initPersistFields();

   /// Declare Console Object.
   DECLARE_CONOBJECT(ObjectTemplateAsset);
};

#endif // _OBJECT_TEMPLATE_ASSET_H_
