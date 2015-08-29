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

ConsoleMethod(GuiDynamicCtrlArrayControl, refresh, void, 2, 2, "() Forces the child controls to recalculate")
{
   object->updateChildControls();
}

extern "C"{
   DLL_PUBLIC GuiDynamicCtrlArrayControl* GuiDynamicCtrlArrayControlCreateInstance()
   {
      return new GuiDynamicCtrlArrayControl();
   }

   DLL_PUBLIC S32 GuiDynamicCtrlArrayControlGetColCount(GuiDynamicCtrlArrayControl* ctrl)
   {
      return ctrl->getCols();
   }

   DLL_PUBLIC void GuiDynamicCtrlArrayControlSetColCount(GuiDynamicCtrlArrayControl* ctrl, S32 cols)
   {
      ctrl->setCols(cols);
   }

   DLL_PUBLIC S32 GuiDynamicCtrlArrayControlGetColSize(GuiDynamicCtrlArrayControl* ctrl)
   {
      return ctrl->getColSize();
   }

   DLL_PUBLIC void GuiDynamicCtrlArrayControlSetColSize(GuiDynamicCtrlArrayControl* ctrl, S32 colSize)
   {
      ctrl->setColSize(colSize);
   }

   DLL_PUBLIC S32 GuiDynamicCtrlArrayControlGetRowSize(GuiDynamicCtrlArrayControl* ctrl)
   {
      return ctrl->getRowSize();
   }

   DLL_PUBLIC void GuiDynamicCtrlArrayControlSetRowSize(GuiDynamicCtrlArrayControl* ctrl, S32 size)
   {
      ctrl->setRowSize(size);
   }

   DLL_PUBLIC S32 GuiDynamicCtrlArrayControlGetRowSpacing(GuiDynamicCtrlArrayControl* ctrl)
   {
      return ctrl->getRowSpacing();
   }

   DLL_PUBLIC void GuiDynamicCtrlArrayControlSetRowSpacing(GuiDynamicCtrlArrayControl* ctrl, S32 Spacing)
   {
      ctrl->setRowSpacing(Spacing);
   }

   DLL_PUBLIC S32 GuiDynamicCtrlArrayControlGetColSpacing(GuiDynamicCtrlArrayControl* ctrl)
   {
      return ctrl->getColSpacing();
   }

   DLL_PUBLIC void GuiDynamicCtrlArrayControlSetColSpacing(GuiDynamicCtrlArrayControl* ctrl, S32 Spacing)
   {
      ctrl->setColSpacing(Spacing);
   }

   DLL_PUBLIC void GuiDynamicCtrlArrayControlRefresh(GuiDynamicCtrlArrayControl* ctrl)
   {
      ctrl->updateChildControls();
   }
}