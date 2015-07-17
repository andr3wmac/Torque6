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

#ifndef _PLATFORM_LIBRARY_H_
#define _PLATFORM_LIBRARY_H_

#include <cstring>

//------------------------------------------------------------------------------

#if defined _WIN32 || defined __CYGWIN__

  #include <windows.h>
  #define LIBRARY_HANDLE HMODULE
  #define LIBRARY_FUNC FARPROC WINAPI

  #ifndef TORQUE_PLUGIN
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllexport))
      #define DLL_PUBLIC_EXPORT __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC __declspec(dllexport)
      #define DLL_PUBLIC_EXPORT __declspec(dllexport)
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllimport))
      #define DLL_PUBLIC_EXPORT __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC __declspec(dllimport)
      #define DLL_PUBLIC_EXPORT __declspec(dllexport)
    #endif
  #endif
  #define DLL_LOCAL

#else

  #include <dlfcn.h>
  #define LIBRARY_HANDLE void*
  #define LIBRARY_FUNC void*

  #if __GNUC__ >= 4
    #define DLL_PUBLIC __attribute__ ((visibility ("default")))
    #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
    #define DLL_PUBLIC_EXPORT __attribute__ ((visibility ("default")))
  #else
    #define DLL_PUBLIC
    #define DLL_LOCAL
    #define DLL_PUBLIC_EXPORT
  #endif
#endif

// Library Functions
DLL_PUBLIC LIBRARY_HANDLE  openLibrary(const char* name, const char* path = "");
DLL_PUBLIC LIBRARY_FUNC    getLibraryFunc(LIBRARY_HANDLE lib, const char* func);
DLL_PUBLIC void            closeLibrary(LIBRARY_HANDLE lib);

#endif // _PLATFORM_LIBRARY_H_
