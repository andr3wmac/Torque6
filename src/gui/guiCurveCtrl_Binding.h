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
   DLL_PUBLIC GuiCurveCtrl* GuiCurveCtrlCreateInstance()
   {
      return new GuiCurveCtrl();
   }

   DLL_PUBLIC void GuiCurveCtrlGetStart(GuiCurveCtrl* ctrl, CInterface::Point2IParam* outPos)
   {
      *outPos = ctrl->getStart();
   }

   DLL_PUBLIC void GuiCurveCtrlSetStart(GuiCurveCtrl* ctrl, CInterface::Point2IParam pos)
   {
      ctrl->setStart(pos);
   }

   DLL_PUBLIC void GuiCurveCtrlGetEnd(GuiCurveCtrl* ctrl, CInterface::Point2IParam* outPos)
   {
      *outPos = ctrl->getEnd();
   }

   DLL_PUBLIC void GuiCurveCtrlSetEnd(GuiCurveCtrl* ctrl, CInterface::Point2IParam pos)
   {
      ctrl->setEnd(pos);
   }

   DLL_PUBLIC void GuiCurveCtrlGetControlA(GuiCurveCtrl* ctrl, CInterface::Point2IParam* outPos)
   {
      *outPos = ctrl->getControlPointA();
   }

   DLL_PUBLIC void GuiCurveCtrlSetControlA(GuiCurveCtrl* ctrl, CInterface::Point2IParam pos)
   {
      ctrl->setControlPointA(pos);
   }

   DLL_PUBLIC void GuiCurveCtrlGetControlB(GuiCurveCtrl* ctrl, CInterface::Point2IParam* outPos)
   {
      *outPos = ctrl->getControlPointB();
   }

   DLL_PUBLIC void GuiCurveCtrlSetControlB(GuiCurveCtrl* ctrl, CInterface::Point2IParam pos)
   {
      ctrl->setControlPointB(pos);
   }

   DLL_PUBLIC void GuiCurveCtrlGetColor(GuiCurveCtrl* ctrl, CInterface::ColorParam* outCol)
   {
      *outCol = ctrl->getColor();
   }

   DLL_PUBLIC void GuiCurveCtrlSetColor(GuiCurveCtrl* ctrl, CInterface::ColorParam color)
   {
      ctrl->setColor(color);
   }

   DLL_PUBLIC F32 GuiCurveCtrlGetThickness(GuiCurveCtrl* ctrl)
   { 
      return ctrl->getThickness();
   }

   DLL_PUBLIC void GuiCurveCtrlSetThickness(GuiCurveCtrl* ctrl, F32 thickness)
   {
      ctrl->setThickness(thickness);
   }
}