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
#include "debug/torqueDebug.h"
#include <bgfx/bgfx.h>
#include <../tools/shaderc/shaderc.h>
#include "platform/platformFileMonitor.h"

// Script bindings.
#include "shaders_Binding.h"

namespace Graphics
{
   // Shader statics.
   bgfx::UniformHandle Shader::textureUniforms[16];
   HashMap<const char*, bgfx::UniformHandle> Shader::uniformMap;

   // Global shader variables.
   static Shader  gShaderList[256];
   static U32     gShaderCount = 0;
   static char    gShaderPath[1024];
   static char    gShaderIncludePath[1024];
   static char    gShaderVaryingPath[1024];

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

      for (U32 n = 0; n < gShaderCount; ++n)
      {
         gShaderList[n].unload();
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

   void setDefaultShaderPath(const char* path)
   {
      dSprintf(gShaderPath, 1024, "%s/", path);
      dSprintf(gShaderIncludePath, 1024, "%s/includes/", path);
      dSprintf(gShaderVaryingPath, 1024, "%s/includes/varying.def.tsh", path);
   }

   Shader* getShader(const char* vertexShaderPath, const char* fragmentShaderPath, bool forceRecompile, bool monitorFile)
   {
      // See if the shader is already loaded.
      for ( U32 n = 0; n < gShaderCount; ++n )
      {
         Shader* s = &gShaderList[n];
         if ( !s->loaded )
            continue;

         if ( dStrcmp(s->mVertexShaderPath, vertexShaderPath) == 0 && dStrcmp(s->mPixelShaderPath, fragmentShaderPath) == 0 )
            return s;
      }

      // Try to fill an unloaded spot.
      for ( U32 n = 0; n < gShaderCount; ++n )
      {
         Shader* s = &gShaderList[n];
         if ( !s->loaded )
         {
            s->load(vertexShaderPath, fragmentShaderPath, forceRecompile, monitorFile);
            return s;
         }
      }

      if (gShaderList[gShaderCount].load(vertexShaderPath, fragmentShaderPath, forceRecompile, monitorFile) )
      {
         gShaderCount++;
         return &gShaderList[gShaderCount - 1];
      }

      return NULL;
   }

   Shader* getShader(const char* computeShaderPath, bool forceRecompile, bool monitorFile)
   {
      // See if the compute shader is already loaded.
      for (U32 n = 0; n < gShaderCount; ++n)
      {
         Shader* s = &gShaderList[n];
         if (!s->loaded)
            continue;

         if (dStrcmp(s->mComputeShaderPath, computeShaderPath) == 0)
            return s;
      }

      // Try to fill an unloaded spot.
      for (U32 n = 0; n < gShaderCount; ++n)
      {
         Shader* s = &gShaderList[n];
         if (!s->loaded)
         {
            s->load(computeShaderPath, forceRecompile, monitorFile);
            return s;
         }
      }

      if (gShaderList[gShaderCount].load(computeShaderPath, forceRecompile, monitorFile))
      {
         gShaderCount++;
         return &gShaderList[gShaderCount - 1];
      }

      return NULL;
   }

   Shader* getDefaultShader(const char* vertexShaderPath, const char* fragmentShaderPath, bool forceRecompile, bool monitorFile)
   {
      // Create full shader paths using set default shader path
      char full_vs_path[512];
      char full_fs_path[512];
      dSprintf(full_vs_path, 512, "%s%s", gShaderPath, vertexShaderPath);
      dSprintf(full_fs_path, 512, "%s%s", gShaderPath, fragmentShaderPath);

      return getShader(full_vs_path, full_fs_path, forceRecompile, monitorFile);
   }

   Shader* getDefaultShader(const char* computeShaderPath, bool forceRecompile, bool monitorFile)
   {
      // Create full compute shader paths using set default shader path
      char full_cs_path[512];
      dSprintf(full_cs_path, 512, "%s%s", gShaderPath, computeShaderPath);

      return getShader(full_cs_path, forceRecompile, monitorFile);
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
      //
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

   S32 Shader::compileShader(uint64_t _flags,
         const char* _filePath,
         const char* _outFilePath,
         const char* _type,
         const char* _platform,
         const char* _profile,
         const char* _bin2c,
         const char* _includeDir,
         const char* _varyingdef,
         char* _outputText,
         uint16_t& _outputSize)
   {
      const char* argv[16];
      int argc = 0;

      // -f <file path>                Input file path.
      argv[argc] = "-f";
      argv[argc + 1] = _filePath;
      argc += 2;

      // -o <file path>                Output file path.
      argv[argc] = "-o";
      argv[argc + 1] = _outFilePath;
      argc += 2;

      // --platform <platform>     Target platform.
      argv[argc] = "--platform";
      argv[argc + 1] = _platform;
      argc += 2;

      // --type <type>             Shader type (vertex, fragment)
      argv[argc] = "--type";
      argv[argc + 1] = _type;
      argc += 2;

      // -i <include path>             Include path (for multiple paths use semicolon).
      if (_includeDir)
      {
         argv[argc] = "-i";
         argv[argc + 1] = _includeDir;
         argc += 2;
      }

      // --bin2c <file path>       Generate C header file.
      if (_bin2c)
      {
         argv[argc] = "--bin2c";
         argv[argc + 1] = _bin2c;
         argc += 2;
      }

      // --varyingdef <file path>  Path to varying.def.sc file.
      if (_varyingdef)
      {
         argv[argc] = "--varyingdef";
         argv[argc + 1] = _varyingdef;
         argc += 2;
      }

      // -p, --profile <profile>       Shader model (f.e. ps_3_0).
      if (_profile)
      {
         argv[argc] = "-p";
         argv[argc + 1] = _profile;
         argc += 2;
      }

      S32 result = bgfx::compileShader(argc, argv);
      bgfx::getShaderError(_outputText, _outputSize);
      
      return result;
   }

   bool Shader::load(const char* vertexShaderPath, const char* fragmentShaderPath, bool forceRecompile, bool monitorFile)
   {
      unload();

      #if TORQUE_DEBUG_RECOMPILE_ALL_SHADERS
         forceRecompile = true;
      #endif

      mVertexShaderPath = StringTable->insert(vertexShaderPath);
      mPixelShaderPath = StringTable->insert(fragmentShaderPath);

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
            
         case bgfx::RendererType::Metal:
            rendererExt = "mtl";
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
      dSprintf(vertex_compiled_path, 256, "%s.%s.bin", vertexShaderPath, rendererExt);
      StringTableEntry cachedVertexPath = Platform::getCachedFilePath(vertex_compiled_path);
      if (forceRecompile || !Platform::isFile(cachedVertexPath))
      {
         Platform::createPath(cachedVertexPath);
         switch (renderer)
         {
            case bgfx::RendererType::Direct3D12:
               compileShader(0, vertexShaderPath, cachedVertexPath, "v", "windows", "vs_4_0", NULL, Graphics::gShaderIncludePath, Graphics::gShaderVaryingPath, shader_output, shader_output_size);
               break;

            case bgfx::RendererType::Direct3D11:
               compileShader(0, vertexShaderPath, cachedVertexPath, "v", "windows", "vs_5_0", NULL, Graphics::gShaderIncludePath, Graphics::gShaderVaryingPath, shader_output, shader_output_size);
               break;

            case bgfx::RendererType::Direct3D9:
               compileShader(0, vertexShaderPath, cachedVertexPath, "v", "windows", "vs_3_0", NULL, Graphics::gShaderIncludePath, Graphics::gShaderVaryingPath, shader_output, shader_output_size);
               break;
                 
            case bgfx::RendererType::Metal:
               compileShader(0, vertexShaderPath, cachedVertexPath, "v", "osx", "metal", NULL, Graphics::gShaderIncludePath, Graphics::gShaderVaryingPath, shader_output, shader_output_size);
               break;

            default:
               compileShader(0, vertexShaderPath, cachedVertexPath, "v", "osx", "120", NULL, Graphics::gShaderIncludePath, Graphics::gShaderVaryingPath, shader_output, shader_output_size);
               break;
         }

         printShaderError(shader_output_size, shader_output, vertexShaderPath);
      }

      mVertexShaderFile = new FileObject();
      if ( mVertexShaderFile->readMemory(cachedVertexPath) )
      {
         const bgfx::Memory* mem = bgfx::makeRef(mVertexShaderFile->getBuffer(), mVertexShaderFile->getBufferSize());
         mVertexShader = bgfx::createShader(mem);
      }

      // Pixel Shader
      char pixel_compiled_path[256];
      dSprintf(pixel_compiled_path, 256, "%s.%s.bin", fragmentShaderPath, rendererExt);
      StringTableEntry cachedPixelPath = Platform::getCachedFilePath(pixel_compiled_path);
      if (forceRecompile || !Platform::isFile(cachedPixelPath))
      {
         Platform::createPath(cachedPixelPath);
         switch (renderer)
         {
            case bgfx::RendererType::Direct3D12:
               compileShader(0, fragmentShaderPath, cachedPixelPath, "f", "windows", "ps_4_0", NULL, Graphics::gShaderIncludePath, Graphics::gShaderVaryingPath, shader_output, shader_output_size);
               break;

            case bgfx::RendererType::Direct3D11:
               compileShader(0, fragmentShaderPath, cachedPixelPath, "f", "windows", "ps_5_0", NULL, Graphics::gShaderIncludePath, Graphics::gShaderVaryingPath, shader_output, shader_output_size);
               break;

            case bgfx::RendererType::Direct3D9:
               compileShader(0, fragmentShaderPath, cachedPixelPath, "f", "windows", "ps_3_0", NULL, Graphics::gShaderIncludePath, Graphics::gShaderVaryingPath, shader_output, shader_output_size);
               break;
                 
            case bgfx::RendererType::Metal:
               compileShader(0, fragmentShaderPath, cachedPixelPath, "f", "osx", "metal", NULL, Graphics::gShaderIncludePath, Graphics::gShaderVaryingPath, shader_output, shader_output_size);
               break;

            default:
               compileShader(0, fragmentShaderPath, cachedPixelPath, "f", "osx", "120", NULL, Graphics::gShaderIncludePath, Graphics::gShaderVaryingPath, shader_output, shader_output_size);
               break;
         }

         printShaderError(shader_output_size, shader_output, fragmentShaderPath);
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
         if (monitorFile)
         {
            PlatformFileChangeDelegate pixelDelegate(this, &Shader::pixelShaderChanged);
            addFileMonitor(mPixelShaderPath, pixelDelegate);
            PlatformFileChangeDelegate vertexDelegate(this, &Shader::vertexShaderChanged);
            addFileMonitor(mVertexShaderPath, vertexDelegate);
         }

         loaded = true;
         return bgfx::isValid(mProgram);
      }

      return false;
   }

   bool Shader::load(const char* computeShaderPath, bool forceRecompile, bool monitorFile)
   {
      unload();

      #if TORQUE_DEBUG_RECOMPILE_ALL_SHADERS
         forceRecompile = true;
      #endif

      mComputeShaderPath = StringTable->insert(computeShaderPath);

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

         case bgfx::RendererType::Metal:
            rendererExt = "mtl";
            break;
              
         default:
            rendererExt = "gl";
            break;
      }

      char shader_output[U16_MAX];
      U16 shader_output_size = 0;

      // Compute Shader
      char compute_compiled_path[256];
      dSprintf(compute_compiled_path, 256, "%s.%s.bin", computeShaderPath, rendererExt);
      StringTableEntry cachedComputePath = Platform::getCachedFilePath(compute_compiled_path);
      if (forceRecompile || !Platform::isFile(cachedComputePath))
      {
         switch (renderer)
         {
            case bgfx::RendererType::Direct3D12:
               compileShader(0, computeShaderPath, cachedComputePath, "c", "windows", "cs_5_0", NULL, Graphics::gShaderIncludePath, Graphics::gShaderVaryingPath, shader_output, shader_output_size);
               break;

            case bgfx::RendererType::Direct3D11:
               compileShader(0, computeShaderPath, cachedComputePath, "c", "windows", "cs_5_0", NULL, Graphics::gShaderIncludePath, Graphics::gShaderVaryingPath, shader_output, shader_output_size);
               break;

            case bgfx::RendererType::Direct3D9:
               //compileShader(0, computeShaderPath, cachedComputePath, "c", "windows", "ps_3_0", NULL, Graphics::shaderIncludePath, Graphics::shaderVaryingPath, shader_output, shader_output_size);
               break;
                 
            case bgfx::RendererType::Metal:
               compileShader(0, computeShaderPath, cachedComputePath, "c", "osx", "metal", NULL, Graphics::gShaderIncludePath, Graphics::gShaderVaryingPath, shader_output, shader_output_size);
               break;

            default:
               compileShader(0, computeShaderPath, cachedComputePath, "c", "osx", "120", NULL, Graphics::gShaderIncludePath, Graphics::gShaderVaryingPath, shader_output, shader_output_size);
               break;
         }

         printShaderError(shader_output_size, shader_output, computeShaderPath);
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
         if (monitorFile)
         {
            PlatformFileChangeDelegate computeDelegate(this, &Shader::computeShaderChanged);
            addFileMonitor(mComputeShaderPath, computeDelegate);
         }

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

   void Shader::computeShaderChanged(const char* computeShaderPath)
   {
      if (!loaded) return;
      Con::printf("Compute Shader File Changed: %s", computeShaderPath);
      load(mComputeShaderPath);
   }

   void Shader::pixelShaderChanged(const char* pixelShaderPath)
   {
      if (!loaded) return;
      Con::printf("Pixel Shader File Changed: %s", pixelShaderPath);
      load(mVertexShaderPath, mPixelShaderPath, true);
   }

   void Shader::vertexShaderChanged(const char* vertexShaderPath)
   {
      if (!loaded) return;
      Con::printf("Vertex Shader File Changed: %s", vertexShaderPath);
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

      mShader.load(mVertexShaderPath, mPixelShaderPath, false, true);
   }

}
