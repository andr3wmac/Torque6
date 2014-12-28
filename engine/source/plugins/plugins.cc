//-----------------------------------------------------------------------------
// Copyright (c) 2014 Andrew Mac
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

#include "console/consoleTypes.h"
#include "plugins.h"
#include "graphics/utilities.h"
#include "platform/event.h"
#include "sysgui/sysgui.h"

#include "plugins_shared.h"
#include "plugins_ScriptBinding.h"

// TODO: Add platform checks
#include <windows.h> 

namespace Plugins
{
   Vector<Plugin> pluginList;
   PluginLink pluginLink;

   void init()
   {
      // Initialize all the function pointers.
      pluginLink.Con.printf = Con::printf;
      pluginLink.Con.errorf = Con::errorf;
      pluginLink.Con.warnf = Con::warnf;
      pluginLink.Con.addCommand = Con::addCommand;
      pluginLink.Con.classLinkNamespaces = Con::classLinkNamespaces;
      pluginLink.Con.setData = Con::setData;
      pluginLink.Con.registerClassRep = AbstractClassRep::registerClassRep;

      pluginLink.SysGUI.beginScrollArea = SysGUI::beginScrollArea;
      pluginLink.SysGUI.button = SysGUI::button;
      pluginLink.SysGUI.checkBox = SysGUI::checkBox;
      pluginLink.SysGUI.endScrollArea = SysGUI::endScrollArea;
      pluginLink.SysGUI.label = SysGUI::label;
      pluginLink.SysGUI.list = SysGUI::list;
      pluginLink.SysGUI.separator = SysGUI::separator;
      pluginLink.SysGUI.slider = SysGUI::slider;
      pluginLink.SysGUI.textInput = SysGUI::textInput;
   }

   void destroy()
   {

   }

   bool load(const char* path)
   {
      char expandedPath[1024];
      Con::expandPath( expandedPath, sizeof(expandedPath), path );

      HINSTANCE hInst = LoadLibraryA(expandedPath);
      if ( hInst != 0 )
      {
         Plugin p;
         
         p.hInst = hInst;
         dStrcpy(p.path, expandedPath);

         p.init = (Plugin::initFunc)GetProcAddress(p.hInst, "initPlugin");
         p.init(pluginLink);

         pluginList.push_back(p);
         return true;
      } else {
         Con::errorf("[PLUGIN] Could not load plugin: %s", path);
      }

      return false;
   }
}      