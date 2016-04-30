//-----------------------------------------------------------------------------
// Copyright (c) 2016 Andrew Mac
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

#ifndef SCRIPT_COMPONENT_H
#define SCRIPT_COMPONENT_H

#ifndef _BASE_COMPONENT_H_
#include "baseComponent.h"
#endif

namespace Scene 
{
   class DLL_PUBLIC ScriptComponent : public BaseComponent
   {
      private:
         typedef BaseComponent Parent;

         StringTableEntry mScriptFile;

      public:
         ScriptComponent();
         ~ScriptComponent();

         void setScriptFile(StringTableEntry scriptFile);

         void onAddToScene();
         void onRemoveFromScene();
         void onSceneStart();
         void onScenePlay();
         void onScenePause();
         void onSceneStop();
         void refresh();

         static void initPersistFields();
         static bool setScriptFile(void* obj, const char* data) { static_cast<ScriptComponent*>(obj)->setScriptFile(StringTable->insert(data)); return false; }

         DECLARE_CONOBJECT(ScriptComponent);
   };
}

#endif // SCRIPT_COMPONENT_H