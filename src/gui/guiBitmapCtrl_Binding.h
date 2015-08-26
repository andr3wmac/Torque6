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

ConsoleMethod(GuiBitmapCtrl, setValue, void, 4, 4, "(int xAxis, int yAxis)"
   "Set the offset of the bitmap.\n"
   "@return No return value."
   )
{
   object->setValue(dAtoi(argv[2]), dAtoi(argv[3]));
}

ConsoleMethod(GuiBitmapCtrl, setBitmap, void, 3, 3, "( pathName ) Use the setBitmap method to change the bitmap this control uses.\n"
   "@param pathName A path to a new texture for this control. Limited to 256x256.\n"
   "@return No return value")
{
   object->setBitmap(argv[2]);
}

ConsoleMethod(GuiBitmapCtrl, getTextureWidth, S32, 2, 2, "Gets the Width of the Texture.\n"
   "@return Texture Width"
   )
{
   return object->getWidth();
}

ConsoleMethod(GuiBitmapCtrl, getTextureHeight, S32, 2, 2, "Gets the Height of the Texture.\n"
   "@return Texture Height"
   )
{
   return object->getHeight();
}

extern "C"{
   DLL_PUBLIC GuiBitmapCtrl* GuiBitmapCtrlCreateInstance()
   {
      return new GuiBitmapCtrl();
   }

   DLL_PUBLIC const char* GuiBitmapCtrlGetBitmap(GuiBitmapCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getBitmap());
   }

   DLL_PUBLIC void GuiBitmapCtrlSetBitmap(GuiBitmapCtrl* ctrl, const char* bitmapFile)
   {
      ctrl->setBitmap(bitmapFile);
   }

   DLL_PUBLIC bool GuiBitmapCtrlGetWrap(GuiBitmapCtrl* ctrl)
   {
      return ctrl->getWrap();
   }

   DLL_PUBLIC void GuiBitmapCtrlSetWrap(GuiBitmapCtrl* ctrl, bool wrap)
   {
      ctrl->setWrap(wrap);
   }

   DLL_PUBLIC bool GuiBitmapCtrlGetUseSourceRect(GuiBitmapCtrl* ctrl)
   {
      return ctrl->getUseSourceRect();
   }

   DLL_PUBLIC void GuiBitmapCtrlSetUseSourceRect(GuiBitmapCtrl* ctrl, bool use)
   {
      ctrl->setUseSourceRect(use);
   }

   DLL_PUBLIC void GuiBitmapCtrlGetSourceRect(GuiBitmapCtrl* ctrl, CInterface::RectIParam* outRect)
   {
      *outRect = ctrl->getSourceRect();
   }

   DLL_PUBLIC void GuiBitmapCtrlSetSourceRect(GuiBitmapCtrl* ctrl, CInterface::RectIParam rect)
   {
      ctrl->setSourceRect(rect.point.x, rect.point.y, rect.extent.x, rect.extent.y);
   }

   DLL_PUBLIC void GuiBitmapCtrlSetValue(GuiBitmapCtrl* ctrl, S32 x, S32 y)
   {
      ctrl->setValue(x, y);
   }

   DLL_PUBLIC S32 GuiBitmapCtrlGetTextureWidth(GuiBitmapCtrl* ctrl)
   {
      return ctrl->getWidth();
   }

   DLL_PUBLIC S32 GuiBitmapCtrlGetTextureHeight(GuiBitmapCtrl* ctrl)
   {
      return ctrl->getHeight();
   }
}