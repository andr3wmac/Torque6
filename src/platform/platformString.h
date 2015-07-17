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

#ifndef _PLATFORM_STRING_H_
#define _PLATFORM_STRING_H_

#ifndef _TORQUE_TYPES_H_
#include "platform/types.h"
#endif

#ifndef _PLATFORM_LIBRARY_H_
#include "platform/platformLibrary.h"
#endif

#include <stdarg.h>

//------------------------------------------------------------------------------

DLL_PUBLIC extern U32 dStrlen(const char *str);

DLL_PUBLIC extern char* dStrcat(char *dst, const char *src);
DLL_PUBLIC extern UTF8* dStrcat(UTF8 *dst, const UTF8 *src);

DLL_PUBLIC extern char* dStrncat(char* dst, const char* src, dsize_t len);
DLL_PUBLIC extern char* dStrcatl(char *dst, dsize_t dstSize, ...);

DLL_PUBLIC extern int dStrcmp(const char *str1, const char *str2);
DLL_PUBLIC extern int dStrcmp(const UTF16 *str1, const UTF16 *str2);
DLL_PUBLIC extern int dStrcmp(const UTF8 *str1, const UTF8 *str2);

DLL_PUBLIC extern int dStricmp(const char *str1, const char *str2);
DLL_PUBLIC extern int dStrncmp(const char *str1, const char *str2, dsize_t len);
DLL_PUBLIC extern int dStrnicmp(const char *str1, const char *str2, dsize_t len);

DLL_PUBLIC extern char* dStrcpy(char *dst, const char *src);
DLL_PUBLIC extern char* dStrcpyl(char *dst, dsize_t dstSize, ...);
DLL_PUBLIC extern char* dStrncpy(char *dst, const char *src, dsize_t len);
DLL_PUBLIC extern char* dStrncpy(UTF8 *dst, const UTF8 *src, dsize_t len);

DLL_PUBLIC extern char* dStrupr(char *str);
DLL_PUBLIC extern char* dStrlwr(char *str);
DLL_PUBLIC inline char dToupper(const char c) { if (c >= char('a') && c <= char('z')) return char(c + 'A' - 'a'); else return c; }
DLL_PUBLIC inline char dTolower(const char c) { if (c >= char('A') && c <= char('Z')) return char(c - 'A' + 'a'); else return c; }

DLL_PUBLIC extern char* dStrchr(char *str, int c);
DLL_PUBLIC extern const char* dStrchr(const char *str, int c);
DLL_PUBLIC extern char* dStrrchr(char *str, int c);
DLL_PUBLIC extern const char* dStrrchr(const char *str, int c);
DLL_PUBLIC extern U32 dStrspn(const char *str, const char *set);
DLL_PUBLIC extern U32 dStrcspn(const char *str, const char *set);
DLL_PUBLIC extern char* dStrstr(char *str1, char *str2);
DLL_PUBLIC extern char* dStrstr(const char *str1, const char *str2);

DLL_PUBLIC extern char* dStrtok(char *str, const char *sep);

DLL_PUBLIC extern int dStrrev(char* str);

DLL_PUBLIC extern int dAtoi(const char *str);
DLL_PUBLIC extern float dAtof(const char *str);
DLL_PUBLIC extern bool dAtob(const char *str);
DLL_PUBLIC extern int dItoa(int n, char s[]);

DLL_PUBLIC extern bool dIsalnum(const char c);
DLL_PUBLIC extern bool dIsalpha(const char c);
DLL_PUBLIC extern bool dIsdigit(const char c);
DLL_PUBLIC extern bool dIsspace(const char c);

DLL_PUBLIC extern int dSscanf(const char *buffer, const char *format, ...);
DLL_PUBLIC extern int dFflushStdout();
DLL_PUBLIC extern int dFflushStderr();

DLL_PUBLIC extern void dPrintf(const char *format, ...);
DLL_PUBLIC extern int dVprintf(const char *format, va_list arglist);
DLL_PUBLIC extern int dSprintf(char *buffer, dsize_t bufferSize, const char *format, ...);
DLL_PUBLIC extern int dVsprintf(char *buffer, dsize_t bufferSize, const char *format, va_list arglist);

#define QSORT_CALLBACK FN_CDECL
DLL_PUBLIC extern void dQsort(void *base, U32 nelem, U32 width, int (QSORT_CALLBACK *fcmp)(const void *, const void *));

// UNICODE is a windows platform API switching flag. Don't define it on other platforms.
#ifdef UNICODE
#define dT(s)    L##s
#else
#define dT(s)    s
#endif

#define dStrdup(x) dStrdup_r(x, __FILE__, __LINE__)

DLL_PUBLIC extern char* dStrdup_r(const char *src, const char*, dsize_t);

#endif  // _PLATFORM_STRING_H_
