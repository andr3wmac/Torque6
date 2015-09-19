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

#include "graphics/shaders.h"
#include <bgfx.h>
#include <../tools/shaderc/shaderc.h>
#include "platform/platformFileMonitor.h"

// Script bindings.
#include "shaders_Binding.h"

namespace Graphics
{
   bgfx::UniformHandle Shader::textureUniforms[16];
   HashMap<const char*, bgfx::UniformHandle> Shader::uniformMap;
   Shader shaderList[256];
   U32 shaderCount = 0;
   char shaderPath[1024];
   char shaderIncludePath[1024];
   char shaderVaryingPath[1024];

   void initUniforms()
   {
      for(int n = 0; n < 16; ++n)
         Shader::textureUniforms[n].idx = bgfx::invalidHandle;
   }

   void destroyUniforms()
   {
      for(int n = 0; n < 16; ++n)
      {
         if ( Shader::textureUniforms[n].idx != bgfx::invalidHandle )
            bgfx::destroyUniform(Shader::textureUniforms[n]);
      }

      typedef HashMap<const char *, bgfx::UniformHandle, HashTable< const char *, bgfx::UniformHandle> >::iterator it_type;
      for(it_type iterator = Shader::uniformMap.begin(); iterator != Shader::uniformMap.end(); ++iterator) 
      {
         if ( iterator->value.idx != bgfx::invalidHandle )
            bgfx::destroyUniform(iterator->value);
      }

      for (U32 n = 0; n < shaderCount; ++n)
      {
        shaderList[n].unload();
      }
   }

   bgfx::UniformHandle Shader::getTextureUniform(U32 slot)
   {
      if ( slot >= 16 )
      {
         bgfx::UniformHandle dummy;
         dummy.idx = bgfx::invalidHandle;
         return dummy;
      }

      if ( textureUniforms[slot].idx == bgfx::invalidHandle )
      {
         char uniformName[32];
         dSprintf(uniformName, 32, "Texture%d", slot);
         textureUniforms[slot] = bgfx::createUniform(uniformName, bgfx::UniformType::Int1);
      }
      return textureUniforms[slot];
   }

   bgfx::UniformHandle Shader::getUniform(const char* name, bgfx::UniformType::Enum type, U32 count)
   {
      if ( uniformMap.find(name) == uniformMap.end() )
      {
         bgfx::UniformHandle newHandle = bgfx::createUniform(name, type, count);
         uniformMap.insert(name, newHandle);
      }

      return uniformMap[name];
   }

   bgfx::UniformHandle Shader::getShadowMapUniform(U32 cascade)
   {
      char shadowMapName[32];
      dSprintf(shadowMapName, 32, "s_shadowMap%d", cascade);
      return getUniform(shadowMapName, bgfx::UniformType::Int1, 1);
   }

   bgfx::UniformHandle Shader::getUniformVec4(const char* name, U32 count)
   {
      return getUniform(name, bgfx::UniformType::Vec4, count);
   }

   bgfx::UniformHandle Shader::getUniformMat4(const char* name, U32 count)
   {
      return getUniform(name, bgfx::UniformType::Mat4, count);
   }

   void destroyShader(Shader* shader)
   {
      if ( shader == NULL )
         return;

      shader->unload();
   }

   Shader* getShader(const char* vertex_shader_path, const char* fragment_shader_path, bool defaultPath)
   {
      // Create full shader paths
      char full_vs_path[512];
      char full_fs_path[512];

      if ( defaultPath )
      {
         dSprintf(full_vs_path, 512, "%s%s", shaderPath, vertex_shader_path);
         dSprintf(full_fs_path, 512, "%s%s", shaderPath, fragment_shader_path);
      } else {
         dSprintf(full_vs_path, 512, "%s", vertex_shader_path);
         dSprintf(full_fs_path, 512, "%s", fragment_shader_path);
      }

      for ( U32 n = 0; n < shaderCount; ++n )
      {
         Shader* s = &shaderList[n];
         if ( !s->loaded )
            continue;

         if ( dStrcmp(s->mVertexShaderPath, full_vs_path) == 0 && dStrcmp(s->mPixelShaderPath, full_fs_path) == 0 )
            return s;
      }

      // Try to fill an unloaded spot.
      for ( U32 n = 0; n < shaderCount; ++n )
      {
         Shader* s = &shaderList[n];
         if ( !s->loaded )
         {
            s->load(full_vs_path, full_fs_path);
            return s;
         }
      }

      if ( shaderList[shaderCount].load(full_vs_path, full_fs_path) )
      {
         shaderCount++;
         return &shaderList[shaderCount - 1];
      }

      return NULL;
   }

   Shader* getShader(const char* compute_shader_path, bool defaultPath)
   {
      // Create full shader paths
      char full_cs_path[512];

      if (defaultPath)
         dSprintf(full_cs_path, 512, "%s%s", shaderPath, compute_shader_path);
      else
         dSprintf(full_cs_path, 512, "%s", compute_shader_path);

      for (U32 n = 0; n < shaderCount; ++n)
      {
         Shader* s = &shaderList[n];
         if (!s->loaded)
            continue;

         if (dStrcmp(s->mComputeShaderPath, full_cs_path) == 0)
            return s;
      }

      // Try to fill an unloaded spot.
      for (U32 n = 0; n < shaderCount; ++n)
      {
         Shader* s = &shaderList[n];
         if (!s->loaded)
         {
            s->load(full_cs_path);
            return s;
         }
      }

      if (shaderList[shaderCount].load(full_cs_path))
      {
         shaderCount++;
         return &shaderList[shaderCount - 1];
      }

      return NULL;
   }

   ShaderAsset* getShaderAsset(const char* id)
   {
      AssetPtr<ShaderAsset> result;
      StringTableEntry assetId = StringTable->insert(id);
      result.setAssetId(assetId);
      return result;
   }

   Shader::Shader()
   {
      loaded               = false;

      mVertexShaderFile    = NULL;
      mPixelShaderFile     = NULL;
      mComputeShaderFile   = NULL;

      mVertexShaderPath    = StringTable->EmptyString;
      mPixelShaderPath     = StringTable->EmptyString;
      mComputeShaderPath   = StringTable->EmptyString;

      mPixelShader.idx     = bgfx::invalidHandle;
      mVertexShader.idx    = bgfx::invalidHandle;
      mComputeShader.idx   = bgfx::invalidHandle;

      mProgram.idx         = bgfx::invalidHandle;
   }

   Shader::~Shader()
   {
      unload();
   }

   void Shader::printShaderError(U16 output_size, char* output_log, const char* source_path)
   {
      if (output_size > 0)
      {
         Con::printf("Error compiling shader %s:", source_path);
         char* pch = dStrtok(output_log, "\n");
         while (pch != NULL)
         {
            Con::printf("%s", pch);
            pch = strtok(NULL, "\n");
         }
      }
   }

   bool Shader::load(const char* vertex_shader_path, const char* pixel_shader_path, bool force_recompile)
   {
      unload();

      mVertexShaderPath = StringTable->insert(vertex_shader_path);
      mPixelShaderPath = StringTable->insert(pixel_shader_path);

      // Determine renderer extension
      bgfx::RendererType::Enum renderer = bgfx::getRendererType();
      const char* rendererExt;
      switch (renderer)
      {
         case bgfx::RendererType::Direct3D12:
            rendererExt = "d3d12";
            break;

         case bgfx::RendererType::Direct3D11:
            rendererExt = "d3d11";
            break;

         case bgfx::RendererType::Direct3D9:
            rendererExt = "d3d9";
            break;

         default:
            rendererExt = "gl";
            break;
      }
      
      // Output from any shader compilations. Errors, etc.
      char shader_output[UINT16_MAX];
      U16 shader_output_size = 0;

      // Vertex Shader
      char vertex_compiled_path[256];
      dSprintf(vertex_compiled_path, 256, "%s.%s.bin", vertex_shader_path, rendererExt);
      StringTableEntry cachedVertexPath = Platform::getCachedFilePath(vertex_compiled_path);
      if (force_recompile || !Platform::isFile(cachedVertexPath))
      {
         Platform::createPath(cachedVertexPath);
         switch (renderer)
         {
            case bgfx::RendererType::Direct3D12:
               bgfx::compileShader(0, vertex_shader_path, cachedVertexPath, "v", "windows", "vs_4_0", NULL, Graphics::shaderIncludePath, Graphics::shaderVaryingPath, shader_output, shader_output_size);
               break;

            case bgfx::RendererType::Direct3D11:
               bgfx::compileShader(0, vertex_shader_path, cachedVertexPath, "v", "windows", "vs_5_0", NULL, Graphics::shaderIncludePath, Graphics::shaderVaryingPath, shader_output, shader_output_size);
               break;

            case bgfx::RendererType::Direct3D9:
               bgfx::compileShader(0, vertex_shader_path, cachedVertexPath, "v", "windows", "vs_3_0", NULL, Graphics::shaderIncludePath, Graphics::shaderVaryingPath, shader_output, shader_output_size);
               break;

            default:
               bgfx::compileShader(0, vertex_shader_path, cachedVertexPath, "v", "linux", NULL, NULL, Graphics::shaderIncludePath, Graphics::shaderVaryingPath, shader_output, shader_output_size);
               break;
         }

         printShaderError(shader_output_size, shader_output, vertex_shader_path);
      }

      mVertexShaderFile = new FileObject();
      if ( mVertexShaderFile->readMemory(cachedVertexPath) )
      {
         const bgfx::Memory* mem = bgfx::makeRef(mVertexShaderFile->getBuffer(), mVertexShaderFile->getBufferSize());
         mVertexShader = bgfx::createShader(mem);
      }

      // Pixel Shader
      char pixel_compiled_path[256];
      dSprintf(pixel_compiled_path, 256, "%s.%s.bin", pixel_shader_path, rendererExt);
      StringTableEntry cachedPixelPath = Platform::getCachedFilePath(pixel_compiled_path);
      if (force_recompile || !Platform::isFile(cachedPixelPath))
      {
         Platform::createPath(cachedPixelPath);
         switch (renderer)
         {
            case bgfx::RendererType::Direct3D12:
               bgfx::compileShader(0, pixel_shader_path, cachedPixelPath, "f", "windows", "ps_4_0", NULL, Graphics::shaderIncludePath, Graphics::shaderVaryingPath, shader_output, shader_output_size);
               break;

            case bgfx::RendererType::Direct3D11:
               bgfx::compileShader(0, pixel_shader_path, cachedPixelPath, "f", "windows", "ps_5_0", NULL, Graphics::shaderIncludePath, Graphics::shaderVaryingPath, shader_output, shader_output_size);
               break;

            case bgfx::RendererType::Direct3D9:
               bgfx::compileShader(0, pixel_shader_path, cachedPixelPath, "f", "windows", "ps_3_0", NULL, Graphics::shaderIncludePath, Graphics::shaderVaryingPath, shader_output, shader_output_size);
               break;

            default:
               bgfx::compileShader(0, pixel_shader_path, cachedPixelPath, "f", "linux", NULL, NULL, Graphics::shaderIncludePath, Graphics::shaderVaryingPath, shader_output, shader_output_size);
               break;
         }

         printShaderError(shader_output_size, shader_output, pixel_shader_path);
      }

      mPixelShaderFile = new FileObject();
      if ( mPixelShaderFile->readMemory(cachedPixelPath) )
      {
         const bgfx::Memory* mem = bgfx::makeRef(mPixelShaderFile->getBuffer(), mPixelShaderFile->getBufferSize());
         mPixelShader = bgfx::createShader(mem);
      }

      // Load Program
      if ( mPixelShader.idx != bgfx::invalidHandle && mVertexShader.idx != bgfx::invalidHandle )
      {
         mProgram = bgfx::createProgram(mVertexShader, mPixelShader, true);
         
         // Add file monitor to reload shader if it changes.
         PlatformFileChangeDelegate pixelDelegate(this, &Shader::pixelShaderChanged);
         addFileMonitor(mPixelShaderPath, pixelDelegate);
         PlatformFileChangeDelegate vertexDelegate(this, &Shader::vertexShaderChanged);
         addFileMonitor(mVertexShaderPath, vertexDelegate);

         loaded = true;
         return bgfx::isValid(mProgram);
      }

      return false;
   }

   bool Shader::load(const char* compute_shader_path, bool force_recompile)
   {
      unload();

      mComputeShaderPath = StringTable->insert(compute_shader_path);

      // Determine renderer extension
      bgfx::RendererType::Enum renderer = bgfx::getRendererType();
      const char* rendererExt;
      switch (renderer)
      {
         case bgfx::RendererType::Direct3D12:
            rendererExt = "d3d12";
            break;

         case bgfx::RendererType::Direct3D11:
            rendererExt = "d3d11";
            break;

         case bgfx::RendererType::Direct3D9:
            rendererExt = "d3d9";
            break;

         default:
            rendererExt = "gl";
            break;
      }

      char shader_output[U16_MAX];
      U16 shader_output_size = 0;

      // Compute Shader
      char compute_compiled_path[256];
      dSprintf(compute_compiled_path, 256, "%s.%s.bin", compute_shader_path, rendererExt);
      StringTableEntry cachedComputePath = Platform::getCachedFilePath(compute_compiled_path);
      if (force_recompile || !Platform::isFile(cachedComputePath))
      {
         switch (renderer)
         {
            case bgfx::RendererType::Direct3D12:
               bgfx::compileShader(0, compute_shader_path, cachedComputePath, "c", "windows", "cs_5_0", NULL, Graphics::shaderIncludePath, Graphics::shaderVaryingPath, shader_output, shader_output_size);
               break;

            case bgfx::RendererType::Direct3D11:
               bgfx::compileShader(0, compute_shader_path, cachedComputePath, "c", "windows", "cs_5_0", NULL, Graphics::shaderIncludePath, Graphics::shaderVaryingPath, shader_output, shader_output_size);
               break;

            case bgfx::RendererType::Direct3D9:
               //bgfx::compileShader(0, compute_shader_path, compute_compiled_path, "c", "windows", "ps_3_0", NULL, Graphics::shaderIncludePath, Graphics::shaderVaryingPath, shader_output, shader_output_size);
               break;

            default:
               bgfx::compileShader(0, compute_shader_path, cachedComputePath, "c", "linux", NULL, NULL, Graphics::shaderIncludePath, Graphics::shaderVaryingPath, shader_output, shader_output_size);
               break;
         }

         printShaderError(shader_output_size, shader_output, compute_shader_path);
      }

      mComputeShaderFile = new FileObject();
      if (mComputeShaderFile->readMemory(cachedComputePath))
      {
         const bgfx::Memory* mem = bgfx::makeRef(mComputeShaderFile->getBuffer(), mComputeShaderFile->getBufferSize());
         mComputeShader = bgfx::createShader(mem);
      }

      // Load Program
      if (mComputeShader.idx != bgfx::invalidHandle)
      {
         mProgram = bgfx::createProgram(mComputeShader, true);

         // Add file monitor to reload shader if it changes.
         PlatformFileChangeDelegate computeDelegate(this, &Shader::computeShaderChanged);
         addFileMonitor(mComputeShaderPath, computeDelegate);

         loaded = true;
         return bgfx::isValid(mProgram);
      }

      return false;
   }

   void Shader::unload()
   {
      mVertexShaderPath    = StringTable->EmptyString;
      mPixelShaderPath     = StringTable->EmptyString;
      mComputeShaderPath   = StringTable->EmptyString;

      if ( mVertexShaderFile != NULL )
      {
         delete mVertexShaderFile;
         mVertexShaderFile = NULL;
      }

      if ( mPixelShaderFile != NULL )
      {
         delete mPixelShaderFile;
         mPixelShaderFile = NULL;
      }

      if (mComputeShaderFile != NULL)
      {
         delete mComputeShaderFile;
         mComputeShaderFile = NULL;
      }

      if ( bgfx::isValid(mVertexShader) )
         bgfx::destroyShader(mVertexShader);

      if ( bgfx::isValid(mPixelShader) )
         bgfx::destroyShader(mPixelShader);

      if (bgfx::isValid(mComputeShader))
         bgfx::destroyShader(mComputeShader);

      if ( bgfx::isValid(mProgram) )
         bgfx::destroyProgram(mProgram);

      mPixelShader.idx     = bgfx::invalidHandle;
      mVertexShader.idx    = bgfx::invalidHandle;
      mComputeShader.idx   = bgfx::invalidHandle;
      mProgram.idx         = bgfx::invalidHandle;
      loaded               = false;
   }

   void Shader::computeShaderChanged(const char* compute_shader_path)
   {
      if (!loaded) return;
      Con::printf("Compute Shader File Changed: %s", compute_shader_path);
      load(mComputeShaderPath);
   }

   void Shader::pixelShaderChanged(const char* pixel_shader_path)
   {
      if (!loaded) return;
      Con::printf("Pixel Shader File Changed: %s", pixel_shader_path);
      load(mVertexShaderPath, mPixelShaderPath, true);
   }

   void Shader::vertexShaderChanged(const char* vertex_shader_path)
   {
      if (!loaded) return;
      Con::printf("Vertex Shader File Changed: %s", vertex_shader_path);
      load(mVertexShaderPath, mPixelShaderPath, true);
   }

   //------------------------------------------------------------------------------

   ConsoleType( ShaderAssetPtr, TypeShaderAssetPtr, sizeof(AssetPtr<ShaderAsset>), ASSET_ID_FIELD_PREFIX )

   //-----------------------------------------------------------------------------

   ConsoleGetType( TypeShaderAssetPtr )
   {
       // Fetch asset Id.
       return (*((AssetPtr<ShaderAsset>*)dptr)).getAssetId();
   }

   //-----------------------------------------------------------------------------

   ConsoleSetType( TypeShaderAssetPtr )
   {
       // Was a single argument specified?
       if( argc == 1 )
       {
           // Yes, so fetch field value.
           const char* pFieldValue = argv[0];

           // Fetch asset pointer.
           AssetPtr<ShaderAsset>* pAssetPtr = dynamic_cast<AssetPtr<ShaderAsset>*>((AssetPtrBase*)(dptr));

           // Is the asset pointer the correct type?
           if ( pAssetPtr == NULL )
           {
               // No, so fail.
               Con::warnf( "(TypeShaderAssetPtr) - Failed to set asset Id '%d'.", pFieldValue );
               return;
           }

           // Set asset.
           pAssetPtr->setAssetId( pFieldValue );

           return;
      }

       // Warn.
       Con::warnf( "(TypeShaderAssetPtr) - Cannot set multiple args to a single asset." );
   }

   //------------------------------------------------------------------------------

   IMPLEMENT_CONOBJECT(ShaderAsset);

   //------------------------------------------------------------------------------

   ShaderAsset::ShaderAsset()
   {
      mVertexShaderPath = StringTable->EmptyString;
      mPixelShaderPath = StringTable->EmptyString;
   }

   //------------------------------------------------------------------------------

   ShaderAsset::~ShaderAsset()
   {
      //
   }

   //------------------------------------------------------------------------------

   void ShaderAsset::initPersistFields()
   {
       // Call parent.
       Parent::initPersistFields();

       addProtectedField("vertexShaderFile", TypeAssetLooseFilePath, Offset(mVertexShaderPath, ShaderAsset), &setVertexShaderPath, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
       addProtectedField("pixelShaderFile", TypeAssetLooseFilePath, Offset(mPixelShaderPath, ShaderAsset), &setPixelShaderPath, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
   }

   //------------------------------------------------------------------------------

   bool ShaderAsset::onAdd()
   {
       // Call Parent.
       if(!Parent::onAdd())
           return false;

       // Return Okay.
       return true;
   }

   //------------------------------------------------------------------------------

   void ShaderAsset::onRemove()
   {
       // Call Parent.
       Parent::onRemove();
   }

   //------------------------------------------------------------------------------

   void ShaderAsset::onAssetRefresh( void ) 
   {
       // Ignore if not yet added to the sim.
       if ( !isProperlyAdded() )
           return;

       // Call parent.
       Parent::onAssetRefresh();
   }

   //------------------------------------------------------------------------------

   void ShaderAsset::copyTo(SimObject* object)
   {
       // Call to parent.
       Parent::copyTo(object);

       // Cast to asset.
       ShaderAsset* pAsset = static_cast<ShaderAsset*>(object);

       // Sanity!
       AssertFatal(pAsset != NULL, "ShaderAsset::copyTo() - Object is not the correct type.");
   }

   void ShaderAsset::initializeAsset( void )
   {
       // Call parent.
       Parent::initializeAsset();

       mVertexShaderPath = expandAssetFilePath( mVertexShaderPath );
       mPixelShaderPath = expandAssetFilePath( mPixelShaderPath );

       compileAndLoad();
   }

   bool ShaderAsset::isAssetValid() const
   {
      return false;
   }

   void ShaderAsset::setPixelShaderPath( const char* pShaderPath )
   {
      // Sanity!
      AssertFatal( pShaderPath != NULL, "Cannot use a NULL image file." );

      // Fetch image file.
      pShaderPath = StringTable->insert( pShaderPath );

      // Ignore no change.
      if ( pShaderPath == mPixelShaderPath )
         return;

      // Update.
      mPixelShaderPath = getOwned() ? expandAssetFilePath( pShaderPath ) : StringTable->insert( pShaderPath );

      // Refresh the asset.
      refreshAsset();
   }

   void ShaderAsset::setVertexShaderPath( const char* pShaderPath )
   {
      // Sanity!
      AssertFatal( pShaderPath != NULL, "Cannot use a NULL image file." );

      // Fetch image file.
      pShaderPath = StringTable->insert( pShaderPath );

      // Ignore no change.
      if ( pShaderPath == mVertexShaderPath )
         return;

      // Update.
      mVertexShaderPath = getOwned() ? expandAssetFilePath( pShaderPath ) : StringTable->insert( pShaderPath );

      // Refresh the asset.
      refreshAsset();
   }

   void ShaderAsset::compileAndLoad()
   {
      if ( mVertexShaderPath == StringTable->EmptyString || mPixelShaderPath == StringTable->EmptyString )
         return;

      mShader.load(mVertexShaderPath, mPixelShaderPath);
   }

}
