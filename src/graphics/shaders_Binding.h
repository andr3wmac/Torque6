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

#include "c-interface/c-interface.h"

ConsoleNamespaceFunction( Graphics, setDefaultShaderPath, ConsoleVoid, 2, 2, (""))
{
   dSprintf(Graphics::shaderPath, 1024, "%s/", argv[1]);
   dSprintf(Graphics::shaderIncludePath, 1024, "%s/includes/", argv[1]);
   dSprintf(Graphics::shaderVaryingPath, 1024, "%s/includes/varying.def.sc", argv[1]);
}

extern "C"{
   DLL_PUBLIC Graphics::ShaderAsset* ShaderAssetCreateInstance()
   {
      return new Graphics::ShaderAsset();
   }

   DLL_PUBLIC const char* ShaderAssetGetVertexShaderFile(Graphics::ShaderAsset* shaderAsset)
   {
      return CInterface::GetMarshallableString(shaderAsset->getVertexShaderPath());
   }

   DLL_PUBLIC void ShaderAssetSetVertexShaderFile(Graphics::ShaderAsset* shaderAsset, const char* val)
   {
      shaderAsset->setVertexShaderPath(val);
   }

   DLL_PUBLIC const char* ShaderAssetGetPixelShaderFile(Graphics::ShaderAsset* shaderAsset)
   {
      return CInterface::GetMarshallableString(shaderAsset->getPixelShaderPath());
   }

   DLL_PUBLIC void ShaderAssetSetPixelShaderFile(Graphics::ShaderAsset* shaderAsset, const char* val)
   {
      shaderAsset->setPixelShaderPath(val);
   }

   DLL_PUBLIC void Graphics_SetDefaultShaderPath(const char* path)
   {
      dSprintf(Graphics::shaderPath, 1024, "%s/", path);
      dSprintf(Graphics::shaderIncludePath, 1024, "%s/includes/", path);
      dSprintf(Graphics::shaderVaryingPath, 1024, "%s/includes/varying.def.sc", path);
   }
}