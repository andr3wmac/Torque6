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

#include "graphics/shaders.h"
#include <bgfx.h>

namespace Graphics
{
   bgfx::UniformHandle Shader::textureUniforms[16];
   HashMap<const char*, bgfx::UniformHandle> Shader::uniformMap;
   Shader shaderList[256];
   U32 shaderCount = 0;

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

      typedef HashMap<const char *, bgfx::UniformHandle, HashTable<const char *, bgfx::UniformHandle>>::iterator it_type;
      for(it_type iterator = Shader::uniformMap.begin(); iterator != Shader::uniformMap.end(); iterator++) 
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
         textureUniforms[slot] = bgfx::createUniform(uniformName, bgfx::UniformType::Uniform1i);
      }
      return textureUniforms[slot];
   }

   bgfx::UniformHandle Shader::getUniform(const char* name)
   {
      if ( uniformMap.find(name) == uniformMap.end() ) 
      {
         bgfx::UniformHandle newHandle = bgfx::createUniform(name, bgfx::UniformType::Uniform4fv);
         uniformMap.insert(name, newHandle);
      }

      return uniformMap[name];
   }

   bgfx::UniformHandle Shader::getUniformMatrix(const char* name)
   {
      if ( uniformMap.find(name) == uniformMap.end() ) 
      {
         bgfx::UniformHandle newHandle = bgfx::createUniform(name, bgfx::UniformType::Uniform4x4fv);
         uniformMap.insert(name, newHandle);
      }

      return uniformMap[name];
   }

   bgfx::UniformHandle Shader::getUniformArray(const char* name, U32 count)
   {
      if ( uniformMap.find(name) == uniformMap.end() ) 
      {
         bgfx::UniformHandle newHandle = bgfx::createUniform(name, bgfx::UniformType::Uniform4fv, count);
         uniformMap.insert(name, newHandle);
      }

      return uniformMap[name];
   }

   Shader* getShader(const char* vertex_shader_path, const char* fragment_shader_path)
   {
      for ( U32 n = 0; n < shaderCount; ++n )
      {
         Shader* s = &shaderList[n];
         if ( dStrcmp(s->mVertexShaderPath, vertex_shader_path) == 0 && dStrcmp(s->mPixelShaderPath, fragment_shader_path) == 0 )
            return s;
      }

      if ( shaderList[shaderCount].load(vertex_shader_path, fragment_shader_path) )
      {
         shaderCount++;
         return &shaderList[shaderCount - 1];
      }

      return NULL;
   }

   Shader::Shader()
   {
      mVertexShaderFile = NULL;
      mPixelShaderFile = NULL;

      mVertexShaderPath = StringTable->EmptyString;
      mPixelShaderPath = StringTable->EmptyString;

      mPixelShader.idx = bgfx::invalidHandle;
      mVertexShader.idx = bgfx::invalidHandle;
      mProgram.idx = bgfx::invalidHandle;
   }

   Shader::~Shader()
   {
      unload();
   }

   bool Shader::load(const char* vertex_shader_path, const char* pixel_shader_path)
   {
      unload();

      mVertexShaderPath = StringTable->insert(vertex_shader_path);
      mPixelShaderPath = StringTable->insert(pixel_shader_path);

      bool is_dx9 = (bgfx::getRendererType() == bgfx::RendererType::Direct3D9);

      // Vertex Shader
      char vertex_compiled_path[256];
      dSprintf(vertex_compiled_path, 256, "%s.bin", vertex_shader_path); 
      if ( is_dx9 )
         bgfx::compileShader(0, vertex_shader_path, vertex_compiled_path, "v", "windows", "vs_3_0", NULL, "shaders/includes/", "shaders/includes/varying.def.sc");
      else
         bgfx::compileShader(0, vertex_shader_path, vertex_compiled_path, "v", "linux", NULL, NULL, "shaders/includes/", "shaders/includes/varying.def.sc");

      mVertexShaderFile = new FileObject();
      if ( mVertexShaderFile->readMemory(vertex_compiled_path) )
      {
         const bgfx::Memory* mem = bgfx::makeRef(mVertexShaderFile->getBuffer(), mVertexShaderFile->getBufferSize());
         mVertexShader = bgfx::createShader(mem);
      }

      // Pixel Shader
      char pixel_compiled_path[256];
      dSprintf(pixel_compiled_path, 256, "%s.bin", pixel_shader_path); 
      if ( is_dx9 )
         bgfx::compileShader(0, pixel_shader_path, pixel_compiled_path, "f", "windows", "ps_3_0", NULL, "shaders/includes/", "shaders/includes/varying.def.sc");
      else
         bgfx::compileShader(0, pixel_shader_path, pixel_compiled_path, "f", "linux", NULL, NULL, "shaders/includes/", "shaders/includes/varying.def.sc");

     mPixelShaderFile = new FileObject();
      if ( mPixelShaderFile->readMemory(pixel_compiled_path) )
      {
         const bgfx::Memory* mem = bgfx::makeRef(mPixelShaderFile->getBuffer(), mPixelShaderFile->getBufferSize());
         mPixelShader = bgfx::createShader(mem);
      }

      // Load Program
      if ( mPixelShader.idx != bgfx::invalidHandle && mVertexShader.idx != bgfx::invalidHandle )
      {
         mProgram = bgfx::createProgram(mVertexShader, mPixelShader, true);
         return bgfx::isValid(mProgram);
      }

      return false;
   }

   void Shader::unload()
   {
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

      if ( bgfx::isValid(mVertexShader) )
         bgfx::destroyShader(mVertexShader);

      if ( bgfx::isValid(mPixelShader) )
         bgfx::destroyShader(mPixelShader);

      if ( bgfx::isValid(mProgram) )
         bgfx::destroyProgram(mProgram);

      mPixelShader.idx = bgfx::invalidHandle;
      mVertexShader.idx = bgfx::invalidHandle;
      mProgram.idx = bgfx::invalidHandle;
   }
}