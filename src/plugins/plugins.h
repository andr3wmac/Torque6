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

#ifndef _PLUGINS_H_
#define _PLUGINS_H_

#ifndef _EVENT_H_
#include "platform/event.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

#ifndef _RENDERABLE_H_
#include "3d/rendering/renderable.h"
#endif

#ifndef _PLUGINS_SHARED_H
#include "plugins_shared.h"
#endif

namespace Plugins
{
   class Plugin : public virtual Tickable, public virtual Renderable
   {
      protected:
         bool              mLoaded;
         LIBRARY_HANDLE    mHInst;
         char              mPath[1024];

      public:
         Plugin();
         ~Plugin();
         
         bool load(const char* path);
         void unload();

         virtual void interpolateTick( F32 delta );
         virtual void processTick();
         virtual void advanceTime( F32 timeDelta );

         virtual void preRender();
         virtual void render();
         virtual void postRender();

         // Function Pointers
         PLUGIN_FUNC_PTR(create)
         PLUGIN_FUNC_PTR(destroy)

         PLUGIN_FUNC_PTR(interpolateTick, F32 delta)
         PLUGIN_FUNC_PTR(processTick)
         PLUGIN_FUNC_PTR(advanceTime, F32 timeDelta)

         PLUGIN_FUNC_PTR(preRender)
         PLUGIN_FUNC_PTR(render)
         PLUGIN_FUNC_PTR(postRender)
   };

   extern Vector<Plugin> _pluginList;

   // 
   void init();
   void destroy();

   bool load(const char* path);

   void addPluginAPI(PluginAPI* api);
   PluginAPI* getPluginAPI(const char* name);
   void requestPluginAPI(const char* name, void (*requestCallback)(PluginAPI* api));
}

#endif // _PLUGINS_H_
