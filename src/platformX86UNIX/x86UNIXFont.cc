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

#include "graphics/gFont.h"
#include "graphics/gBitmap.h"
#include "math/mRect.h"
#include "console/console.h"
#include "string/unicode.h"
#include "platformX86UNIX/platformX86UNIX.h"
#include "platformX86UNIX/x86UNIXFont.h"

x86UNIXFont::x86UNIXFont()
{}

x86UNIXFont::~x86UNIXFont()
{}


bool x86UNIXFont::create(const char *name, U32 size, U32 charset)
{
  return false;
}

bool x86UNIXFont::isValidChar(const UTF16 str) const
{
  return true;
}

bool x86UNIXFont::isValidChar(const UTF8 *str) const
{
  return isValidChar(oneUTF32toUTF16(oneUTF8toUTF32(str,NULL)));
}

PlatformFont::CharInfo &x86UNIXFont::getCharInfo(const UTF16 ch) const
{
  static PlatformFont::CharInfo c;
  dMemset(&c, 0, sizeof(c));
  return c;
}


PlatformFont::CharInfo &x86UNIXFont::getCharInfo(const UTF8 *str) const
{
  return getCharInfo(oneUTF32toUTF16(oneUTF8toUTF32(str,NULL)));
}
