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
#include <bgfx/bgfx.h>
#endif

namespace Graphics
{
   // --------------------------
   // Common Priorities:
   // --------------------------
   // 0     - Torque GUI Bottom
   // 500   - Shadow Mapping
   // 750   - Global Illumination
   // 1000  - Deferred Geometry
   // 1500  - Deferred Light
   // 2000  - Forward Rendering
   // 3000  - Transparency
   // 3500  - SSAO
   //       - SSDO
   // 3600  - SSR
   // 4000  - Post Rendering Begin
   //       - HDR
   // 5000  - Post Rendering Finish
   //       - FXAA
   //       - DLAA
   //       - SMAA
   // 6000  - Torque GUI Top
   // 7000  - SysGUI

   struct ViewTableEntry
   {
      bool  temporary;
      bool  deleted;
      char  name[256];
      U8    id;
      S16   priority;

      ViewTableEntry()
      {
         temporary   = false;
         deleted     = true;
         name[0]     = '\0';
         id          = 0;
         priority    = 0;
      }
   };
   
   void              resetViews();
   ViewTableEntry*   getView(const char* name);
   ViewTableEntry*   getView(const char* name, S16 priority);
   ViewTableEntry*   getTemporaryView(const char* name);
   ViewTableEntry*   getTemporaryView(const char* name, S16 priority);
}
#endif //_GRAPHICS_VIEWTABLE_H_
