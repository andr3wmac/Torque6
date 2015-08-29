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

ConsoleMethod(GuiButtonBaseCtrl, performClick, void, 2, 2, "() - simulates a button click from script.")
{
   object->onAction();
}

ConsoleMethod(GuiButtonBaseCtrl, setText, void, 3, 3, "(string text) - Sets the text of the button to the string.")
{
   object->setText(argv[2]);
}

ConsoleMethod(GuiButtonBaseCtrl, setTextID, void, 3, 3, "(string id) - Sets the text of the button to the localized string.")
{
   object->setTextID(argv[2]);
}
ConsoleMethod(GuiButtonBaseCtrl, getText, const char *, 2, 2, "() - returns the text of the button.\n"
   "@return The text member of the button as a char string")
{
   return object->getText();
}
ConsoleMethod(GuiButtonBaseCtrl, setStateOn, void, 3, 3, "(bool isStateOn) - sets the state on member and updates siblings of the same group.")
{
   object->setStateOn(dAtob(argv[2]));
}

ConsoleMethod(GuiButtonBaseCtrl, getStateOn, bool, 2, 2, "(bool getStateOn) - gets whether the state of the button is currently 'on'")
{
   return object->getStateOn();
}

extern "C"{
   DLL_PUBLIC GuiButtonBaseCtrl* GuiButtonBaseCtrlCreateInstance()
   {
      return new GuiButtonBaseCtrl();
   }

   DLL_PUBLIC const char* GuiButtonBaseCtrlGetText(GuiButtonBaseCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getText());
   }

   DLL_PUBLIC void GuiButtonBaseCtrlSetText(GuiButtonBaseCtrl* ctrl, const char* text)
   {
      ctrl->setText(text);
   }

   DLL_PUBLIC const char* GuiButtonBaseCtrlGetTextID(GuiButtonBaseCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getTextID());
   }

   DLL_PUBLIC void GuiButtonBaseCtrlSetTextID(GuiButtonBaseCtrl* ctrl, const char* textId)
   {
      ctrl->setTextID(textId);
   }

   DLL_PUBLIC S32 GuiButtonBaseCtrlGetGroupNum(GuiButtonBaseCtrl* ctrl)
   {
      return ctrl->getGroupNum();
   }

   DLL_PUBLIC void GuiButtonBaseCtrlSetGroupNum(GuiButtonBaseCtrl* ctrl, S32 groupNum)
   {
      ctrl->setGroupNum(groupNum);
   }

   DLL_PUBLIC S32 GuiButtonBaseCtrlGetButtonType(GuiButtonBaseCtrl* ctrl)
   {
      return ctrl->getButtonType();
   }

   DLL_PUBLIC void GuiButtonBaseCtrlSetButtonType(GuiButtonBaseCtrl* ctrl, S32 buttonType)
   {
      ctrl->setButtonType(buttonType);
   }

   DLL_PUBLIC bool GuiButtonBaseCtrlGetUseMouseEvents(GuiButtonBaseCtrl* ctrl)
   {
      return ctrl->getUseMouseEvents();
   }

   DLL_PUBLIC void GuiButtonBaseCtrlSetUseMouseEvents(GuiButtonBaseCtrl* ctrl, bool useMouseEvents)
   {
      ctrl->setUseMouseEvents(useMouseEvents);
   }

   DLL_PUBLIC void GuiButtonBaseCtrlPerformClick(GuiButtonBaseCtrl* ctrl)
   {
      ctrl->onAction();
   }

   DLL_PUBLIC void GuiButtonBaseCtrlSetStateOn(GuiButtonBaseCtrl* ctrl, bool isStateOn)
   {
      ctrl->setStateOn(isStateOn);
   }

   DLL_PUBLIC bool GuiButtonBaseCtrlGetStateOn(GuiButtonBaseCtrl* ctrl)
   {
      return ctrl->getStateOn();
   }
}