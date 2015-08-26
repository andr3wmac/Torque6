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

ConsoleMethod(GuiTextCtrl, setText, void, 3, 3, "( newText ) Use the setText method to set the content of label to newText.\n"
   "@param newText A string representing the new value for this label.\n"
   "@return No return value")
{
   object->setText(argv[2]);
}

ConsoleMethod(GuiTextCtrl, setTextID, void, 3, 3, "( int newTextID ) Set the textID to a new value.\n"
   "@param newTextID The desired new ID\n"
   "@return No return value.")
{
   object->setTextID(argv[2]);
}

extern "C"{
   DLL_PUBLIC GuiTextCtrl* GuiTextCtrlCreateInstance()
   {
      return new GuiTextCtrl();
   }

   DLL_PUBLIC const char* GuiTextCtrlGetText(GuiTextCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getText());
   }

   DLL_PUBLIC void GuiTextCtrlSetText(GuiTextCtrl* ctrl, const char* text)
   {
      ctrl->setText(text);
   }

   DLL_PUBLIC const char* GuiTextCtrlGetTextID(GuiTextCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getInitialTextID());
   }

   DLL_PUBLIC void GuiTextCtrlSetTextID(GuiTextCtrl* ctrl, const char* text)
   {
      ctrl->setInitialTextID(text);
   }

   DLL_PUBLIC S32 GuiTextCtrlGetMaxLength(GuiTextCtrl* ctrl)
   {
      return ctrl->getMaxStrLen();
   }

   DLL_PUBLIC void GuiTextCtrlSetMaxLength(GuiTextCtrl* ctrl, S32 length)
   {
      ctrl->setMaxStrLen(length);
   }

   DLL_PUBLIC bool GuiTextCtrlGetTruncate(GuiTextCtrl* ctrl)
   {
      return ctrl->getTruncateWhenUnfocused();
   }

   DLL_PUBLIC void GuiTextCtrlSetTruncate(GuiTextCtrl* ctrl, bool truncate)
   {
      ctrl->setTruncateWhenUnfocused(truncate);
   }
}