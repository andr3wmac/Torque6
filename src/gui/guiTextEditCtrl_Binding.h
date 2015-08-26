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

ConsoleMethod(GuiTextEditCtrl, getText, const char*, 2, 2, "() Get the contents of the textedit control\n"
   "@return Returns the current textedit buffer.")
{
   if (!object->hasText())
      return StringTable->EmptyString;

   char *retBuffer = Con::getReturnBuffer(GuiTextEditCtrl::MAX_STRING_LENGTH);
   object->getText(retBuffer);

   return retBuffer;
}


ConsoleMethod(GuiTextEditCtrl, getCursorPos, S32, 2, 2, "() Use the getCursorPos method to get the current position of the text cursor in the control.\n"
   "@return Returns the current position of the text cursor in the control, where 0 is at the beginning of the line, 1 is after the first letter, and so on.\n"
   "@sa setCursorPos")
{
   return(object->getCursorPos());
}

ConsoleMethod(GuiTextEditCtrl, setCursorPos, void, 3, 3, "( newPos ) Use the setCursorPos method to set the current position of text cursor to newPos.\n"
   "If the requested position is beyond the end of text, the cursor will be placed after the last letter. If the value is less than zero, the cursor will be placed at the front of the entry.\n"
   "@param newPos The new position to place the cursor at, where 0 is at the beginning of the line, 1 is after the first letter, and so on.\n"
   "@return No return value.\n"
   "@sa getCursorPos")
{
   object->reallySetCursorPos(dAtoi(argv[2]));
}

ConsoleMethod(GuiTextEditCtrl, selectAllText, void, 2, 2, "textEditCtrl.selectAllText()")
{
   object->selectAllText();
}

ConsoleMethod(GuiTextEditCtrl, forceValidateText, void, 2, 2, "textEditCtrl.forceValidateText()")
{
   object->forceValidateText();
}

extern "C"{
   DLL_PUBLIC GuiTextEditCtrl* GuiTextEditCtrlCreateInstance()
   {
      return new GuiTextEditCtrl();
   }

   DLL_PUBLIC const char* GuiTextEditCtrlGetValidate(GuiTextEditCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getValidateCommand());
   }

   DLL_PUBLIC void GuiTextEditCtrlSetValidate(GuiTextEditCtrl* ctrl, const char* validateCommand)
   {
      ctrl->setValidateCommand(validateCommand);
   }

   DLL_PUBLIC const char* GuiTextEditCtrlGetEscapeCommand(GuiTextEditCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getEscapeCommand());
   }

   DLL_PUBLIC void GuiTextEditCtrlSetEscapeCommand(GuiTextEditCtrl* ctrl, const char* EscapeCommand)
   {
      ctrl->setEscapeCommand(EscapeCommand);
   }

   DLL_PUBLIC S32 GuiTextEditCtrlGetHistorySize(GuiTextEditCtrl* ctrl)
   {
      return ctrl->getHistorySize();
   }

   DLL_PUBLIC void GuiTextEditCtrlSetHistorySize(GuiTextEditCtrl* ctrl, S32 size)
   {
      ctrl->setHistorySize(size);
   }

   DLL_PUBLIC bool GuiTextEditCtrlGetTabComplete(GuiTextEditCtrl* ctrl)
   {
      return ctrl->getTabComplete();
   }

   DLL_PUBLIC void GuiTextEditCtrlSetTabComplete(GuiTextEditCtrl* ctrl, bool tabComplete)
   {
      ctrl->setTabComplete(tabComplete);
   }

   DLL_PUBLIC AudioAsset* GuiTextEditCtrlGetDeniedSound(GuiTextEditCtrl* ctrl)
   {
      return ctrl->getDeniedSound();
   }

   DLL_PUBLIC void GuiTextEditCtrlSetDeniedSound(GuiTextEditCtrl* ctrl, AudioAsset* sound)
   {
      ctrl->setDeniedSound(sound);
   }

   DLL_PUBLIC bool GuiTextEditCtrlGetSinkAllKeyEvents(GuiTextEditCtrl* ctrl)
   {
      return ctrl->getSinkAllKeyEvents();
   }

   DLL_PUBLIC void GuiTextEditCtrlSetSinkAllKeyEvents(GuiTextEditCtrl* ctrl, bool sinkAllKeyEvents)
   {
      ctrl->setSinkAllKeyEvents(sinkAllKeyEvents);
   }

   DLL_PUBLIC bool GuiTextEditCtrlGetPassword(GuiTextEditCtrl* ctrl)
   {
      return ctrl->getPasswordText();
   }

   DLL_PUBLIC void GuiTextEditCtrlSetPassword(GuiTextEditCtrl* ctrl, bool isPasswordText)
   {
      ctrl->setPasswordText(isPasswordText);
   }

   DLL_PUBLIC const char* GuiTextEditCtrlGetPasswordMask(GuiTextEditCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getPasswordMask());
   }

   DLL_PUBLIC void GuiTextEditCtrlSetPasswordMask(GuiTextEditCtrl* ctrl, const char* PasswordMaskCommand)
   {
      ctrl->setPasswordMask(PasswordMaskCommand);
   }

   DLL_PUBLIC const char* GuiTextEditCtrlGetText(GuiTextEditCtrl* ctrl)
   {
      char *retBuffer = Con::getReturnBuffer(GuiTextEditCtrl::MAX_STRING_LENGTH);
      ctrl->getText(retBuffer);
      return CInterface::GetMarshallableString(retBuffer);
   }

   DLL_PUBLIC S32 GuiTextEditCtrlGetCursorPos(GuiTextEditCtrl* ctrl)
   {
      return ctrl->getCursorPos();
   }

   DLL_PUBLIC void GuiTextEditCtrlSetCursorPos(GuiTextEditCtrl* ctrl, S32 newPos)
   {
      ctrl->reallySetCursorPos(newPos);
   }

   DLL_PUBLIC void GuiTextEditCtrlSelectAllText(GuiTextEditCtrl* ctrl)
   {
      ctrl->selectAllText();
   }

   DLL_PUBLIC void GuiTextEditCtrlForceValidateText(GuiTextEditCtrl* ctrl)
   {
      ctrl->forceValidateText();
   }
}