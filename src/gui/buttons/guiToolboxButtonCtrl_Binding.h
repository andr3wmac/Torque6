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

ConsoleMethod(GuiToolboxButtonCtrl, setNormalBitmap, void, 3, 3, "( filepath name ) Sets the bitmap that shows when the button is active"
   "@param name The path of the desired bitmap file\n"
   "@return No Return Value.")
{
   object->setNormalBitmap(argv[2]);
}

ConsoleMethod(GuiToolboxButtonCtrl, setLoweredBitmap, void, 3, 3, "( filepath name ) Sets the bitmap that shows when the button is disabled"
   "@param name The path of the desired bitmap file\n"
   "@return No Return Value.")
{
   object->setLoweredBitmap(argv[2]);
}

ConsoleMethod(GuiToolboxButtonCtrl, setHoverBitmap, void, 3, 3, "( filepath name ) Sets the bitmap that shows when the button is disabled"
   "@param name The path of the desired bitmap file\n"
   "@return No Return Value.")
{
   object->setHoverBitmap(argv[2]);
}

extern "C"{
   DLL_PUBLIC GuiToolboxButtonCtrl* GuiToolboxButtonCtrlCreateInstance()
   {
      return new GuiToolboxButtonCtrl();
   }

   DLL_PUBLIC const char* GuiToolboxButtonCtrlGetNormalBitmap(GuiToolboxButtonCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getNormalBitmap());
   }

   DLL_PUBLIC void GuiToolboxButtonCtrlSetNormalBitmap(GuiToolboxButtonCtrl* ctrl, const char* bitmap)
   {
      ctrl->setNormalBitmap(bitmap);
   }

   DLL_PUBLIC const char* GuiToolboxButtonCtrlGetLoweredBitmap(GuiToolboxButtonCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getLoweredBitmap());
   }

   DLL_PUBLIC void GuiToolboxButtonCtrlSetLoweredBitmap(GuiToolboxButtonCtrl* ctrl, const char* bitmap)
   {
      ctrl->setLoweredBitmap(bitmap);
   }

   DLL_PUBLIC const char* GuiToolboxButtonCtrlGetHoverBitmap(GuiToolboxButtonCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getHoverBitmap());
   }

   DLL_PUBLIC void GuiToolboxButtonCtrlSetHoverBitmap(GuiToolboxButtonCtrl* ctrl, const char* bitmap)
   {
      ctrl->setHoverBitmap(bitmap);
   }
}