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

#ifndef _SHADER_ASSET_H_
#define _SHADER_ASSET_H_

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

//-----------------------------------------------------------------------------

DefineConsoleType( TypePluginAssetPtr )

//-----------------------------------------------------------------------------

class PluginAsset : public AssetBase
{

private:
   typedef AssetBase  Parent;

   StringTableEntry mPluginPath;

public:
   PluginAsset();
   virtual ~PluginAsset();

   static void initPersistFields();
   virtual bool onAdd();
   virtual void onRemove();
   virtual void copyTo(SimObject* object);

   void setPluginPath( const char* pPluginPath );
   StringTableEntry getPluginPath() { return mPluginPath; };

   // Asset validation.
   virtual bool isAssetValid( void ) const;

   /// Declare Console Object.
   DECLARE_CONOBJECT(PluginAsset);

protected:
    virtual void initializeAsset( void );
    virtual void onAssetRefresh( void );

    static bool setPluginPath( void* obj, const char* data )    { static_cast<PluginAsset*>(obj)->setPluginPath(data); return false; }
};

#endif // _SHADER_ASSET_H_