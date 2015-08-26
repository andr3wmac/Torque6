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
   
ConsoleMethod(GuiSliderCtrl, getValue, F32, 2, 2, "() Use the getValue method to get the current value of the slider.\n"
   "@return Returns current value of control (position of slider)"){
   return object->getValue();
}

extern "C"{
   DLL_PUBLIC GuiSliderCtrl* GuiSliderCtrlCreateInstance()
   {
      return new GuiSliderCtrl();
   }

   DLL_PUBLIC void GuiSliderCtrlGetRange(GuiSliderCtrl* ctrl, CInterface::Point2FParam* outRange)
   {
      *outRange = ctrl->getRange();
   }

   DLL_PUBLIC void GuiSliderCtrlSetRange(GuiSliderCtrl* ctrl, CInterface::Point2FParam range)
   {
      ctrl->setRange(range);
   }

   DLL_PUBLIC S32 GuiSliderCtrlGetTicks(GuiSliderCtrl* ctrl)
   {
      return ctrl->getTicks();
   }

   DLL_PUBLIC void GuiSliderCtrlSetTicks(GuiSliderCtrl* ctrl, S32 ticks)
   {
      ctrl->setTicks(ticks);
   }

   DLL_PUBLIC F32 GuiSliderCtrlGetValue(GuiSliderCtrl* ctrl)
   {
      return ctrl->getValue();
   }

   DLL_PUBLIC void GuiSliderCtrlSetValue(GuiSliderCtrl* ctrl, F32 value)
   {
      ctrl->setValue(value);
   }
}