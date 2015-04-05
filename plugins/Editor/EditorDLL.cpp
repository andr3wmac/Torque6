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

#include "EditorDLL.h"
#include <sim/simObject.h>
#include <plugins/plugins_shared.h>
#include <3d/rendering/common.h>
#include <graphics/utilities.h>
#include <bx/fpumath.h>

#include "SceneEditor/SceneEditor.h"

using namespace Plugins;

bool editorOpen = false;
bool loadedGUI = false;

S32 mainEditorArea = -1;

S32 myLabel = 0;
S32 frameCount = 0;
U32 tickCount = 0;
char buf[256];

void create()
{
   // Register Console Function
   Link.Con.addCommand("Editor", "open", openEditor, "", 1, 1);
   Link.Con.addCommand("Editor", "close", closeEditor, "", 1, 1);
}

void loadGUI()
{
   // Main Dialog
   mainEditorArea = Link.SysGUI.beginScrollArea("Torque 6 Editor", 5, 5, 200, 150);
   Link.SysGUI.separator();
   Link.SysGUI.button("Scene Editor", "", NULL);
   Link.SysGUI.button("Entity Editor", "", NULL);
   Link.SysGUI.button("Asset Browser", "", NULL);
   Link.SysGUI.separator();
   myLabel = Link.SysGUI.label("FPS: ");
   Link.SysGUI.endScrollArea();

   loadedGUI = true;
}

// Console Function
void openEditor(SimObject *obj, S32 argc, const char *argv[])
{
   if ( !loadedGUI ) loadGUI();

   editorOpen = true;
   Link.SysGUI.setElementHidden(mainEditorArea, false);

   sceneEditor.enable();
}

void closeEditor(SimObject *obj, S32 argc, const char *argv[])
{
   editorOpen = false;
   Link.SysGUI.setElementHidden(mainEditorArea, true);

   sceneEditor.disable();
}

// Call Every Tick.
void processTick()
{
   if ( !editorOpen ) return;

   tickCount++;
   if ( tickCount > 31 )
   {
      dSprintf(buf, 256, "FPS: %d", frameCount);
      Link.SysGUI.setLabelValue(myLabel, buf);
      frameCount = 0;
      tickCount = 0;
   }
}

void render()
{
   if ( !editorOpen ) return;

   frameCount++;

   sceneEditor.render();
}