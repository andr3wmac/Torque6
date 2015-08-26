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

ConsoleMethod(GuiIconButtonCtrl, setBitmap, void, 3, 3, "(filepath name) Loads bitmap from file\n"
   "@param name The path of the desired bitmap file\n"
   "@return No Return Value.")
{
   char* argBuffer = Con::getArgBuffer(512);
   Platform::makeFullPathName(argv[2], argBuffer, 512);
   object->setBitmap(argBuffer);
}

extern "C"{
   DLL_PUBLIC GuiIconButtonCtrl* GuiIconButtonCtrlCreateInstance()
   {
      return new GuiIconButtonCtrl();
   }

   DLL_PUBLIC void GuiIconButtonCtrlGetButtonMargin(GuiIconButtonCtrl* ctrl, CInterface::Point2IParam* outMargin)
   {
      *outMargin = ctrl->getButtonMargin();
   }

   DLL_PUBLIC void GuiIconButtonCtrlSetButtonMargin(GuiIconButtonCtrl* ctrl, CInterface::Point2IParam margin)
   {
      ctrl->setButtonMargin(margin);
   }

   DLL_PUBLIC const char* GuiIconButtonCtrlGetIconBitmap(GuiIconButtonCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getIconBitmap());
   }

   DLL_PUBLIC void GuiIconButtonCtrlSetIconBitmap(GuiIconButtonCtrl* ctrl, const char* bitmap)
   {
      ctrl->setIconBitmap(bitmap);
   }

   DLL_PUBLIC S32 GuiIconButtonCtrlGetIconLocation(GuiIconButtonCtrl* ctrl)
   {
      return ctrl->getIconLocation();
   }

   DLL_PUBLIC void GuiIconButtonCtrlSetIconLocation(GuiIconButtonCtrl* ctrl, S32 location)
   {
      ctrl->setIconLocation(location);
   }

   DLL_PUBLIC bool GuiIconButtonCtrlGetSizeIconToButton(GuiIconButtonCtrl* ctrl)
   {
      return ctrl->getFitIconToButton();
   }

   DLL_PUBLIC void GuiIconButtonCtrlSetSizeIconToButton(GuiIconButtonCtrl* ctrl, bool sizeIconToButton)
   {
      ctrl->setFitIconToButton(sizeIconToButton);
   }

   DLL_PUBLIC S32 GuiIconButtonCtrlGetTextLocation(GuiIconButtonCtrl* ctrl)
   {
      return ctrl->getTextLocation();
   }

   DLL_PUBLIC void GuiIconButtonCtrlSetTextLocation(GuiIconButtonCtrl* ctrl, S32 location)
   {
      ctrl->setTextLocation(location);
   }

   DLL_PUBLIC S32 GuiIconButtonCtrlGetTextMargin(GuiIconButtonCtrl* ctrl)
   {
      return ctrl->getTextMargin();
   }

   DLL_PUBLIC void GuiIconButtonCtrlSetTextMargin(GuiIconButtonCtrl* ctrl, S32 margin)
   {
      ctrl->setTextMargin(margin);
   }

   DLL_PUBLIC void GuiIconButtonCtrlSetBitmap(GuiIconButtonCtrl* ctrl, const char* filePath)
   {
      char* argBuffer = Con::getArgBuffer(512);
      Platform::makeFullPathName(filePath, argBuffer, 512);
      ctrl->setBitmap(argBuffer);
   }
}