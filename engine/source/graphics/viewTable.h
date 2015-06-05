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

#ifndef _GRAPHICS_VIEWTABLE_H_
#define _GRAPHICS_VIEWTABLE_H_

#ifndef _FILEOBJECT_H_
#include "io/fileObject.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx.h>
#endif

namespace Graphics
{
   // --------------------------
   //  View Table (viewTable.cc)
   // --------------------------
   struct ViewTableEntry
   {
      bool  deleted;
      char  name[256];
      U8    id;

      ViewTableEntry()
      {
         deleted = true;
         name[0] = '\0';
         id = 0;
      }
   };

   extern S32              _viewTableLastID;
   extern U32              _viewTableCount;
   extern ViewTableEntry   _viewTable[256];

   ViewTableEntry*         getView(const char* name, const char* target = "", bool beforeTarget = false);
   ViewTableEntry*         insertView(const char* name, U8 viewID);
   void                    debugViewTable();
}
#endif //_GRAPHICS_VIEWTABLE_H_
