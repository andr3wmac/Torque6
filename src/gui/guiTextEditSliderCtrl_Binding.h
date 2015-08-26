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
   DLL_PUBLIC GuiTextEditSliderCtrl* GuiTextEditSliderCtrlCreateInstance()
   {
      return new GuiTextEditSliderCtrl();
   }

   DLL_PUBLIC const char* GuiTextEditSliderCtrlGetFormat(GuiTextEditSliderCtrl* ctrl)
   {
      return ctrl->getFormat();
   }

   DLL_PUBLIC void GuiTextEditSliderCtrlSetFormat(GuiTextEditSliderCtrl* ctrl, const char* format)
   {
      ctrl->setFormat(format);
   }

   DLL_PUBLIC void GuiTextEditSliderCtrlGetRange(GuiTextEditSliderCtrl* ctrl, CInterface::Point2FParam* outRange)
   {
      *outRange = ctrl->getRange();
   }

   DLL_PUBLIC void GuiTextEditSliderCtrlSetRange(GuiTextEditSliderCtrl* ctrl, CInterface::Point2FParam range)
   {
      ctrl->setRange(range);
   }

   DLL_PUBLIC F32 GuiTextEditSliderCtrlGetIncrement(GuiTextEditSliderCtrl* ctrl)
   {
      return ctrl->getIncAmount();
   }

   DLL_PUBLIC void GuiTextEditSliderCtrlSetIncrement(GuiTextEditSliderCtrl* ctrl, F32 incrementAmount)
   {
      ctrl->setIncAmount(incrementAmount);
   }
}