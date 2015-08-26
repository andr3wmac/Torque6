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

//-------------------------------------- Console functions
ConsoleMethod(GuiMessageVectorCtrl, attach, bool, 3, 3, "( aVector ) Make this gui control display messages from the specified MessageVector.\n"
   "@param aVector A previously created messageVector instance.\n"
   "@return No return value")
{
   MessageVector* pMV = NULL;
   Sim::findObject(argv[2], pMV);
   if (pMV == NULL) {
      Con::errorf(ConsoleLogEntry::General, "Could not find MessageVector: %s", argv[2]);
      return false;
   }

   return object->attach(pMV);
}

ConsoleMethod(GuiMessageVectorCtrl, detach, void, 2, 2, "() Stop listening to messages from the MessageVector this control was previously attached to.\n"
   "@return No return value")
{
   if (object->isAttached() == false) {
      Con::warnf(ConsoleLogEntry::General, "GuiMessageVectorCtrl: double detach");
      return;
   }

   object->detach();
}

extern "C"{
   DLL_PUBLIC GuiMessageVectorCtrl* GuiMessageVectorCtrlCreateInstance()
   {
      return new GuiMessageVectorCtrl();
   }

   DLL_PUBLIC S32 GuiMessageVectorCtrlGetLineSpacing(GuiMessageVectorCtrl* ctrl)
   {
      return ctrl->getLineSpacingPixels();
   }

   DLL_PUBLIC void GuiMessageVectorCtrlSetLineSpacing(GuiMessageVectorCtrl* ctrl, S32 spacing)
   {
      ctrl->setLineSpacingPixels(spacing);
   }

   DLL_PUBLIC S32 GuiMessageVectorCtrlGetLineContinuedIndex(GuiMessageVectorCtrl* ctrl)
   {
      return ctrl->getLineContinuationIndent();
   }

   DLL_PUBLIC void GuiMessageVectorCtrlSetLineContinuedIndex(GuiMessageVectorCtrl* ctrl, S32 index)
   {
      ctrl->setLineContinuationIndent(index);
   }

   DLL_PUBLIC const char* GuiMessageVectorCtrlGetAllowedMatches(GuiMessageVectorCtrl* ctrl, S32 index)
   {
      return CInterface::GetMarshallableString(ctrl->getAllowedMatches(index));
   }

   DLL_PUBLIC void GuiMessageVectorCtrlSetAllowedMatches(GuiMessageVectorCtrl* ctrl, S32 index, const char* match)
   {
      ctrl->setAllowedMatches(index, match);
   }

   DLL_PUBLIC void GuiMessageVectorCtrlGetMatchColor(GuiMessageVectorCtrl* ctrl, CInterface::ColorParam* outColor)
   {
      *outColor = ctrl->getMatchColor();
   }

   DLL_PUBLIC void GuiMessageVectorCtrlSetMatchColor(GuiMessageVectorCtrl* ctrl, CInterface::ColorParam color)
   {
      ctrl->setMatchColor(color);
   }

   DLL_PUBLIC S32 GuiMessageVectorCtrlGetMaxColorIndex(GuiMessageVectorCtrl* ctrl)
   {
      return ctrl->getMaxColorIndex();
   }

   DLL_PUBLIC void GuiMessageVectorCtrlSetMaxColorIndex(GuiMessageVectorCtrl* ctrl, S32 index)
   {
      ctrl->setMaxColorIndex(index);
   }

   DLL_PUBLIC bool GuiMessageVectorCtrlAttach(GuiMessageVectorCtrl* ctrl, MessageVector* MV)
   {
	   return ctrl->attach(MV);
   }

   DLL_PUBLIC void GuiMessageVectorCtrlDetach(GuiMessageVectorCtrl* ctrl)
   {
	   ctrl->detach();
   }
}