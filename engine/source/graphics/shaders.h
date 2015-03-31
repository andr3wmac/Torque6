//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
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

#ifndef _SHADERS_H_
#define _SHADERS_H_

#ifndef _FILEOBJECT_H_
#include "io/fileObject.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx.h>
#endif

#ifndef _HASHTABLE_H
#include "collection/hashTable.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

namespace Graphics
{

   class Shader
   {
      public:
         Shader();
         ~Shader();

         bool load(const char* vertex_shader_path, const char* fragment_shader_path);
         void unload();

         StringTableEntry mVertexShaderPath;
         StringTableEntry mPixelShaderPath;

         FileObject* mVertexShaderFile;
         FileObject* mPixelShaderFile;

         bgfx::ShaderHandle mVertexShader;
         bgfx::ShaderHandle mPixelShader;

         bgfx::ProgramHandle mProgram;

         static bgfx::UniformHandle textureUniforms[16];
         static bgfx::UniformHandle getShadowmapUniform();
         static bgfx::UniformHandle getTextureUniform(U32 slot);
         static HashMap<const char*, bgfx::UniformHandle> uniformMap;
         static bgfx::UniformHandle getUniform(const char* name, bgfx::UniformType::Enum type, U32 count = 1);
         static bgfx::UniformHandle getUniformVec2(const char* name, U32 count = 1);
         static bgfx::UniformHandle getUniformVec3(const char* name, U32 count = 1);
         static bgfx::UniformHandle getUniformVec4(const char* name, U32 count = 1);
         static bgfx::UniformHandle getUniform4x4Matrix(const char* name, U32 count = 1);
   };

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

      Graphics::Shader mShader;

   public:
      ShaderAsset();
      virtual ~ShaderAsset();

      static void initPersistFields();
      virtual bool onAdd();
      virtual void onRemove();
      virtual void copyTo(SimObject* object);

      void setPixelShaderPath( const char* pShaderPath );
      void setVertexShaderPath( const char* pShaderPath );

      // Asset validation.
      virtual bool isAssetValid( void ) const;

      void compileAndLoad();
      bgfx::ProgramHandle getProgram() { return mShader.mProgram; }

      /// Declare Console Object.
      DECLARE_CONOBJECT(ShaderAsset);

   protected:
       virtual void initializeAsset( void );
       virtual void onAssetRefresh( void );

       static bool setPixelShaderPath( void* obj, const char* data )    { static_cast<ShaderAsset*>(obj)->setPixelShaderPath(data); return false; }
       static bool setVertexShaderPath( void* obj, const char* data )   { static_cast<ShaderAsset*>(obj)->setVertexShaderPath(data); return false; }
   };

   extern char shaderPath[1024];
   extern char shaderIncludePath[1024];
   extern char shaderVaryingPath[1024];
   extern Shader shaderList[256];
   extern U32 shaderCount;
   Shader* getShader(const char* vertex_shader_path, const char* fragment_shader_path);
   ShaderAsset* getShaderAsset(const char* id);

   void initUniforms();
   void destroyUniforms();

}
#endif //_SHADERS_H_
