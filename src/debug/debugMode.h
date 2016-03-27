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


#ifndef _DEBUG_MODE_H_
#define _DEBUG_MODE_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _RENDERING_H_
#include "rendering/rendering.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx/bgfx.h>
#endif

namespace Debug
{
   class DLL_PUBLIC DebugMode
   {
      protected:
         bool mEnabled;

      public:
         DebugMode() : mEnabled(false) { }

         virtual bool isEnabled() { return mEnabled; }
         virtual void setEnabled(bool enabled) 
         { 
            if (mEnabled == enabled)
               return;

            mEnabled = enabled; 
            if (mEnabled)
               onEnable();
            else
               onDisable();
         }

         virtual void onEnable() { }
         virtual void onDisable() { }
         virtual void render(Rendering::RenderCamera*) { }
   };

   bool isDebugEnabled();
   void renderDebug(Rendering::RenderCamera* camera, U8 viewID);

   void registerDebugMode(const char* modeName, DebugMode* mode);
   DebugMode* getDebugMode(const char* modeName);
   void setDebugMode(const char* modeName, bool enabled);
}

class DLL_PUBLIC _DebugModeRegister_
{
public:
   _DebugModeRegister_(const char* modeName, Debug::DebugMode* modeClass)
   {
      Debug::registerDebugMode(modeName, modeClass);
   }
};

#define IMPLEMENT_DEBUG_MODE(modeName, modeClass)                                            \
    modeClass modeClass::modeClass##Inst;                                                    \
    _DebugModeRegister_ modeClass::modeClass##Register(modeName, &modeClass::modeClass##Inst)

#define DECLARE_DEBUG_MODE(modeName, modeClass)    \
   static modeClass modeClass##Inst;               \
   static _DebugModeRegister_ modeClass##Register

#endif // _DEBUG_MODE_H_