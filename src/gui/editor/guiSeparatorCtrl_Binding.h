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

extern "C"{
   DLL_PUBLIC GuiSeparatorCtrl* GuiSeparatorCtrlCreateInstance()
   {
      return new GuiSeparatorCtrl();
   }

   DLL_PUBLIC const char* GuiSeparatorCtrlGetCaption(GuiSeparatorCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getText());
   }

   DLL_PUBLIC void GuiSeparatorCtrlSetCaption(GuiSeparatorCtrl* ctrl, const char* caption)
   {
      return ctrl->setText(StringTable->insert(caption));
   }

   DLL_PUBLIC S32 GuiSeparatorCtrlGetType(GuiSeparatorCtrl* ctrl)
   {
      return ctrl->getSeperatorType();
   }

   DLL_PUBLIC void GuiSeparatorCtrlSetType(GuiSeparatorCtrl* ctrl, S32 type)
   {
      ctrl->setSeperatorType(type);
   }

   DLL_PUBLIC S32 GuiSeparatorCtrlGetBorderMargin(GuiSeparatorCtrl* ctrl)
   {
      return ctrl->getMargin();
   }

   DLL_PUBLIC void GuiSeparatorCtrlSetBorderMargin(GuiSeparatorCtrl* ctrl, S32 margin)
   {
      ctrl->setMargin(margin);
   }

   DLL_PUBLIC bool GuiSeparatorCtrlGetInvisible(GuiSeparatorCtrl* ctrl)
   {
      return ctrl->getInvisible();
   }

   DLL_PUBLIC void GuiSeparatorCtrlSetInvisible(GuiSeparatorCtrl* ctrl, bool invisible)
   {
      ctrl->setInvisible(invisible);
   }

   DLL_PUBLIC S32 GuiSeparatorCtrlGetLeftMargin(GuiSeparatorCtrl* ctrl)
   {
      return ctrl->getTextLeftMargin();
   }

   DLL_PUBLIC void GuiSeparatorCtrlSetLeftMargin(GuiSeparatorCtrl* ctrl, S32 leftMargin)
   {
      ctrl->setTextLeftMargin(leftMargin);
   }
}