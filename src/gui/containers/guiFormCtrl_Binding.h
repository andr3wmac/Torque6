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

#include "c-interface/c-interface.h"

ConsoleMethod(GuiFormCtrl, getMenuID, S32, 2, 2, "Returns the ID of the Form Menu")
{
   return object->getMenuBarID();
}

ConsoleMethod(GuiFormCtrl, setCaption, void, 3, 3, "setCaption(caption) - Sets the title of the Form Menu")
{
   object->setCaption(argv[2]);
}

extern "C"{
   DLL_PUBLIC GuiFormCtrl* GuiFormCtrlCreateInstance()
   {
      return new GuiFormCtrl();
   }

   DLL_PUBLIC const char* GuiFormCtrlGetCaption(GuiFormCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getCaption());
   }

   DLL_PUBLIC void GuiFormCtrlSetCaption(GuiFormCtrl* ctrl, const char* caption)
   {
      ctrl->setCaption(caption);
   }

   DLL_PUBLIC const char* GuiFormCtrlGetContentLibrary(GuiFormCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getContentLibrary());
   }

   DLL_PUBLIC void GuiFormCtrlSetContentLibrary(GuiFormCtrl* ctrl, const char* contentLibrary)
   {
      ctrl->setContentLibrary(contentLibrary);
   }

   DLL_PUBLIC const char* GuiFormCtrlGetContent(GuiFormCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getContent());
   }

   DLL_PUBLIC void GuiFormCtrlSetContent(GuiFormCtrl* ctrl, const char* content)
   {
      ctrl->setContent(content);
   }

   DLL_PUBLIC bool GuiFormCtrlGetMovable(GuiFormCtrl* ctrl)
   {
      return ctrl->getCanMove();
   }

   DLL_PUBLIC void GuiFormCtrlSetMovable(GuiFormCtrl* ctrl, bool movable)
   {
      ctrl->setCanMove(movable);
   }

   DLL_PUBLIC bool GuiFormCtrlGetHasMenu(GuiFormCtrl* ctrl)
   {
      return ctrl->getCanMove();
   }

   DLL_PUBLIC void GuiFormCtrlSetHasMenu(GuiFormCtrl* ctrl, bool hasMenu)
   {
      ctrl->setCanMove(hasMenu);
   }

   DLL_PUBLIC S32 GuiFormCtrlGetMenuID(GuiFormCtrl* ctrl)
   {
      return ctrl->getMenuBarID();
   }
}