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

ConsoleMethod(GuiMLTextCtrl, setText, void, 3, 3, "(text) Use the setText method to change the current text content of the control to text. This replaces all old content.\n"
   "@param text The new contents for this control.\n"
   "@return No return value.\n"
   "@sa addText, getText")
{
   object->setText(argv[2], dStrlen(argv[2]));
}

ConsoleMethod(GuiMLTextCtrl, getText, const char*, 2, 2, "() Use the getText method to return the current text contents of the control, including all formatting characters.\n"
   "@return Returns the entire text contents of the control or indicating no contents.\n"
   "@sa addText")
{
   return(object->getTextContent());
}

ConsoleMethod(GuiMLTextCtrl, addText, void, 4, 4, "( text , reformat ) Use the addText method to add new text to the control. You may optionally request that the control be reformatted.\n"
   "@param text Text to add to control.\n"
   "@param reformat A boolean value that when set to true forces the control to re-evaluate the entire contents and to redisplay it.\n"
   "@return No return value.\n"
   "@sa getText, setText, forceReflow")
{
   object->addText(argv[2], dStrlen(argv[2]), dAtob(argv[3]));
}

ConsoleMethod(GuiMLTextCtrl, setCursorPosition, bool, 3, 3, "(newPos) Use the setCursorPosition method to offset the cursor by newPos characters into the current text contents of the control.\n"
   "@param newPos An integer value indicating the character position at which to place the cursor.\n"
   "@return No return value.\n"
   "@sa scrollToTag, scrollToTop")
{
   return object->setCursorPosition(dAtoi(argv[2]));
}

ConsoleMethod(GuiMLTextCtrl, scrollToTag, void, 3, 3, "( tagID ) Use the scrollToTag method to scroll to the first instance of a tag if it exists.\n"
   "@param tagID A tag number to search for. These tags are specified by embedding TorqueML <tag:tag_number> entries in text.\n"
   "@return No return value.\n"
   "@sa scrollToTop, setCursorPosition")
{
   object->scrollToTag(dAtoi(argv[2]));
}

ConsoleMethod(GuiMLTextCtrl, scrollToTop, void, 2, 2, "() Use the scrollToTop method to scroll to the top of the text.\n"
   "@return No return value.\n"
   "@sa scrollToTag, setCursorPosition, scrollToBottom")
{
   object->scrollToTop();
}

ConsoleMethod(GuiMLTextCtrl, scrollToBottom, void, 2, 2, "() Use the scrollToBottom method to scroll to the bottom of the text.\n"
   "@return No return value.\n"
   "@sa scrollToTag, setCursorPosition, scrollToTop")
{
   object->scrollToBottom();
}

ConsoleFunction(StripMLControlChars, const char*, 2, 2, "( sourceString ) Use the stripMLControlChars function to remove all Torque Markup-Language (ML) symbols from sourceString.\n"
   "This may not remove <br> correctly, so check before you trust this function.\n"
   "@param sourceString The string to be modified.\n"
   "@return Returns a copy of sourceString with all the ML symbols removed, or the original string if no ML symbols were present.\n"
   "@sa stripChars")
{
   return GuiMLTextCtrl::stripControlChars(argv[1]);
}

ConsoleMethod(GuiMLTextCtrl, forceReflow, void, 2, 2, "() Use the forceReflow method to force the text control to re-evaluate the entire contents and to redisplay it, possibly resizing the control.\n"
   "@return No return value.\n"
   "@sa addText")
{
   if (!object->isAwake())
      Con::errorf("GuiMLTextCtrl::forceReflow can only be called on visible controls.");
   else
      object->reflow();
}

ConsoleMethod(GuiMLTextCtrl, setAlpha, void, 3, 3, "( alpha ) Use the setAlpha method to set alpha of this control to between [0.0 , 1.0].\n"
   "@param alpha A floating point value between 0.0 and 1.0 indicating the control's new alpha setting.\n"
   "@return No return value")
{
   object->setAlpha(dAtof(argv[2]));
}

extern "C"{
   DLL_PUBLIC GuiMLTextCtrl* GuiMLTextCtrlCreateInstance()
   {
      return new GuiMLTextCtrl();
   }

   DLL_PUBLIC S32 GuiMLTextCtrlGetLineSpacing(GuiMLTextCtrl* ctrl)
   {
      return ctrl->getLineSpacingPixels();
   }

   DLL_PUBLIC void GuiMLTextCtrlSetLineSpacing(GuiMLTextCtrl* ctrl, S32 spacing)
   {
      ctrl->setLineSpacingPixels(spacing);
   }

   DLL_PUBLIC bool GuiMLTextCtrlGetAllowColorChars(GuiMLTextCtrl* ctrl)
   {
      return ctrl->getAllowColorChars();
   }

   DLL_PUBLIC void GuiMLTextCtrlSetAllowColorChars(GuiMLTextCtrl* ctrl, bool allow)
   {
      ctrl->setAllowColorChars(allow);
   }

   DLL_PUBLIC S32 GuiMLTextCtrlGetMaxChars(GuiMLTextCtrl* ctrl)
   {
      return ctrl->getMaxBufferSize();
   }

   DLL_PUBLIC void GuiMLTextCtrlSetMaxChars(GuiMLTextCtrl* ctrl, S32 count)
   {
      ctrl->setMaxBufferSize(count);
   }

   DLL_PUBLIC AudioAsset* GuiMLTextCtrlGetDeniedSound(GuiMLTextCtrl* ctrl)
   {
      return ctrl->getDeniedSound();
   }

   DLL_PUBLIC void GuiMLTextCtrlSetDeniedSound(GuiMLTextCtrl* ctrl, AudioAsset* sound)
   {
      ctrl->setDeniedSound(sound);
   }

   DLL_PUBLIC const char* GuiMLTextCtrlGetText(GuiMLTextCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getInitialText());
   }

   DLL_PUBLIC void GuiMLTextCtrlSetText(GuiMLTextCtrl* ctrl, const char* text)
   {
      ctrl->setInitialText(text);
   }

   DLL_PUBLIC void GuiMLTextCtrlSetCurrentText(GuiMLTextCtrl* ctrl, const char* text)
   {
      ctrl->setText(text, dStrlen(text));
   }

   DLL_PUBLIC const char* GuiMLTextCtrlGetCurrentText(GuiMLTextCtrl* ctrl)
   {
      return ctrl->getTextContent();
   }

   DLL_PUBLIC void GuiMLTextCtrlAddText(GuiMLTextCtrl* ctrl, const char* text, bool reformat)
   {
      ctrl->addText(text, dStrlen(text), reformat);
   }

   DLL_PUBLIC bool GuiMLTextCtrlSetCursorPosition(GuiMLTextCtrl* ctrl, S32 newPos)
   {
      return ctrl->setCursorPosition(newPos);
   }

   DLL_PUBLIC void GuiMLTextCtrlScrollToTag(GuiMLTextCtrl* ctrl, S32 tagID)
   {
      ctrl->scrollToTag(tagID);
   }

   DLL_PUBLIC void GuiMLTextCtrlScrollToBottom(GuiMLTextCtrl* ctrl)
   {
      ctrl->scrollToBottom();
   }

   DLL_PUBLIC void GuiMLTextCtrlScrollToTop(GuiMLTextCtrl* ctrl)
   {
      ctrl->scrollToTop();
   }

   DLL_PUBLIC void GuiMLTextCtrlForceReflow(GuiMLTextCtrl* ctrl)
   {
	   if (!ctrl->isAwake())
		   Con::errorf("GuiMLTextCtrl::forceReflow can only be called on visible controls.");
	   else
		   ctrl->reflow();
   }

   DLL_PUBLIC void GuiMLTextCtrlSetAlpha(GuiMLTextCtrl* ctrl, F32 alpha)
   {
	   ctrl->setAlpha(alpha);
   }

   DLL_PUBLIC const char* Engine_StripMLControlChars(const char* sourceString)
   {
	   return CInterface::GetMarshallableString(GuiMLTextCtrl::stripControlChars(sourceString));
   }
}