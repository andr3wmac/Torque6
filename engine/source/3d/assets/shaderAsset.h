//-----------------------------------------------------------------------------
// Copyright (c) 2014 Andrew Mac
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
#include "graphics/vertexLayouts.h"
#endif

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

#ifndef _SHADERS_H_
#include "graphics/shaders.h"
#endif

//-----------------------------------------------------------------------------

DefineConsoleType( TypeShaderAssetPtr )

//-----------------------------------------------------------------------------

class ShaderAsset : public AssetBase
{

private:
   typedef AssetBase  Parent;

   StringTableEntry mVertexShaderPath;
   StringTableEntry mPixelShaderPath;
   FileObject mVertexShaderFile;
   FileObject mPixelShaderFile;

   bgfx::ShaderHandle mVertexShader;
   bgfx::ShaderHandle mPixelShader;
   bgfx::ProgramHandle mProgram;

public:
   ShaderAsset();
   virtual ~ShaderAsset();

   static void initPersistFields();
   virtual bool onAdd();
   virtual void onRemove();
   virtual void copyTo(SimObject* object);

   // Asset validation.
   virtual bool isAssetValid( void ) const;

   void compileAndLoad();
   bgfx::ProgramHandle getProgram() { return mProgram; }

   /// Declare Console Object.
   DECLARE_CONOBJECT(ShaderAsset);

protected:
    virtual void initializeAsset( void );
    virtual void onAssetRefresh( void );
};

#endif // _SHADER_ASSET_H_