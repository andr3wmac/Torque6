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

#include "debugMode.h"
#include "console/consoleInternal.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "scene/scene.h"
#include "rendering/renderCamera.h"

#include <bgfx/bgfx.h>
#include <debugdraw/debugdraw.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

#include "debugMode_Binding.h"

namespace Debug
{
   typedef HashMap<StringTableEntry, DebugMode*> DebugModeMap;
   static DebugModeMap sDebugModeMap;
   static bool sDebugEnabled = false;

   bool isDebugEnabled()
   {
      return sDebugEnabled;
   }

   void renderDebug(Rendering::RenderCamera* camera, U8 viewID)
   {
      ddBegin(viewID);
      ddSetState(false, false, false);

      for (DebugModeMap::iterator itr = sDebugModeMap.begin(); itr != sDebugModeMap.end(); ++itr)
      {
         DebugMode* mode = itr->value;

         if (mode->isEnabled())
            mode->render(camera);
      }

      ddEnd();
   }

   void registerDebugMode(const char* modeName, DebugMode* modeClass)
   {
      StringTableEntry name = StringTable->insert(modeName);
      sDebugModeMap[name] = modeClass;
   }

   DebugMode* getDebugMode(const char* modeName)
   {
      StringTableEntry name = StringTable->insert(modeName);

      DebugModeMap::iterator itr = sDebugModeMap.find(name);
      if (itr == sDebugModeMap.end())
         return NULL;

      return sDebugModeMap[name];
   }

   void setDebugMode(const char* modeName, bool enabled)
   {
      StringTableEntry name = StringTable->insert(modeName);

      DebugModeMap::iterator itr = sDebugModeMap.find(name);
      if (itr == sDebugModeMap.end())
         return;

      DebugMode* mode = sDebugModeMap[name];
      mode->setEnabled(enabled);

      // Check all debug modes to determine if debug is enabled/disabled.
      sDebugEnabled = false;
      for (DebugModeMap::iterator itr = sDebugModeMap.begin(); itr != sDebugModeMap.end(); ++itr)
      {
         DebugMode* mode = itr->value;

         if (mode->isEnabled())
         {
            sDebugEnabled = true;
            break;
         }
      }
   }
}
