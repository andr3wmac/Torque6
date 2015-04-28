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

#include "platformLibrary.h"
#include <cstring>

//------------------------------------------------------------------------------

LIBRARY_HANDLE openLibrary(const char* name, const char* path)
{
   char final_path[512];
   int pos = 0;

   std::strcpy(&final_path[pos], path);
   pos += std::strlen(path);

#if defined _WIN32 || defined __CYGWIN__
   std::strcpy(&final_path[pos], name);
   pos += std::strlen(name);

   std::strcpy(&final_path[pos], ".dll");
   pos += std::strlen(".dll");

   return LoadLibraryA(final_path);
#else
   std::strcpy(&final_path[pos], "lib");
   pos += std::strlen("lib");

   std::strcpy(&final_path[pos], name);
   pos += std::strlen(name);

   std::strcpy(&final_path[pos], ".so");
   pos += std::strlen(".so");

   return dlopen(final_path, RTLD_LAZY);
#endif
}

LIBRARY_FUNC getLibraryFunc(LIBRARY_HANDLE lib, const char* func)
{
#if defined _WIN32 || defined __CYGWIN__
   return GetProcAddress(lib, func);
#else
   return dlsym(lib, func);
#endif
}

void closeLibrary(LIBRARY_HANDLE lib)
{
#if defined _WIN32 || defined __CYGWIN__
   FreeLibrary(lib);
#else
   dlclose(lib);
#endif
}