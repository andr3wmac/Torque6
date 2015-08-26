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

ConsoleMethod(GuiBitmapButtonCtrl, setBitmap, void, 3, 3, "(filepath name) Loads a bitmap from a given file\n"
   "@return No return value.")
{
   object->setBitmap(argv[2]);
}

ConsoleMethod(GuiBitmapButtonCtrl, setBitmapNormal, void, 3, 3, "(filepath name) Loads a bitmap from a given file for the \"up\" state\n"
   "@return No return value.")
{
   object->setBitmap(argv[2], NORMAL);
}
ConsoleMethod(GuiBitmapButtonCtrl, setBitmapHilight, void, 3, 3, "(filepath name) Loads a bitmap from a given file for the \"down\" state\n"
   "@return No return value.")
{
   object->setBitmap(argv[2], HILIGHT);
}
ConsoleMethod(GuiBitmapButtonCtrl, setBitmapDepressed, void, 3, 3, "(filepath name) Loads a bitmap from a given file for the \"hover\" state\n"
   "@return No return value.")
{
   object->setBitmap(argv[2], DEPRESSED);
}
ConsoleMethod(GuiBitmapButtonCtrl, setBitmapInactive, void, 3, 3, "(filepath name) Loads a bitmap from a given file for the \"inactive\" state\n"
   "@return No return value.")
{
   object->setBitmap(argv[2], INACTIVE);
}

extern "C"{
   DLL_PUBLIC GuiBitmapButtonCtrl* GuiBitmapButtonCtrlCreateInstance()
   {
      return new GuiBitmapButtonCtrl();
   }

   DLL_PUBLIC bool GuiBitmapButtonCtrlGetIsLegacyVersion(GuiBitmapButtonCtrl* ctrl)
   {
      return ctrl->getIsLegacyVersion();
   }

   DLL_PUBLIC void GuiBitmapButtonCtrlSetIsLegacyVersion(GuiBitmapButtonCtrl* ctrl, bool isLegacy)
   {
      ctrl->setIsLegacyVersion(isLegacy);
   }

   DLL_PUBLIC const char* GuiBitmapButtonCtrlGetBitmap(GuiBitmapButtonCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getBitmap());
   }

   DLL_PUBLIC void GuiBitmapButtonCtrlSetBitmap(GuiBitmapButtonCtrl* ctrl, const char* bitmap)
   {
      ctrl->setBitmap(bitmap);
   }

   DLL_PUBLIC const char* GuiBitmapButtonCtrlGetBitmapNormal(GuiBitmapButtonCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getBitmapNormal());
   }

   DLL_PUBLIC void GuiBitmapButtonCtrlSetBitmapNormal(GuiBitmapButtonCtrl* ctrl, const char* bitmap)
   {
      ctrl->setBitmapNormal(bitmap);
   }

   DLL_PUBLIC const char* GuiBitmapButtonCtrlGetBitmapHilight(GuiBitmapButtonCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getBitmapHilight());
   }

   DLL_PUBLIC void GuiBitmapButtonCtrlSetBitmapHilight(GuiBitmapButtonCtrl* ctrl, const char* bitmap)
   {
      ctrl->setBitmapHilight(bitmap);
   }

   DLL_PUBLIC const char* GuiBitmapButtonCtrlGetBitmapDepressed(GuiBitmapButtonCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getBitmapDepressed());
   }

   DLL_PUBLIC void GuiBitmapButtonCtrlSetBitmapDepressed(GuiBitmapButtonCtrl* ctrl, const char* bitmap)
   {
      ctrl->setBitmapDepressed(bitmap);
   }

   DLL_PUBLIC const char* GuiBitmapButtonCtrlGetBitmapInactive(GuiBitmapButtonCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getBitmapInactive());
   }

   DLL_PUBLIC void GuiBitmapButtonCtrlSetBitmapInactive(GuiBitmapButtonCtrl* ctrl, const char* bitmap)
   {
      ctrl->setBitmapInactive(bitmap);
   }

   DLL_PUBLIC GuiBitmapButtonTextCtrl* GuiBitmapButtonTextCtrlCreateInstance()
   {
      return new GuiBitmapButtonTextCtrl();
   }
}