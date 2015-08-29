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

extern "C"{
   DLL_PUBLIC GuiControlArrayControl* GuiControlArrayControlCreateInstance()
   {
      return new GuiControlArrayControl();
   }

   DLL_PUBLIC S32 GuiControlArrayControlGetColCount(GuiControlArrayControl* ctrl)
   {
      return ctrl->getCols();
   }

   DLL_PUBLIC void GuiControlArrayControlSetColCount(GuiControlArrayControl* ctrl, S32 cols)
   {
      ctrl->setCols(cols);
   }

   //C-TODO colSizes

   DLL_PUBLIC S32 GuiControlArrayControlGetRowSize(GuiControlArrayControl* ctrl)
   {
      return ctrl->getRowSize();
   }

   DLL_PUBLIC void GuiControlArrayControlSetRowSize(GuiControlArrayControl* ctrl, S32 size)
   {
      ctrl->setRowSize(size);
   }

   DLL_PUBLIC S32 GuiControlArrayControlGetRowSpacing(GuiControlArrayControl* ctrl)
   {
      return ctrl->getRowSpacing();
   }

   DLL_PUBLIC void GuiControlArrayControlSetRowSpacing(GuiControlArrayControl* ctrl, S32 spacing)
   {
      ctrl->setRowSpacing(spacing);
   }

   DLL_PUBLIC S32 GuiControlArrayControlGetColSpacing(GuiControlArrayControl* ctrl)
   {
      return ctrl->getRowSpacing();
   }

   DLL_PUBLIC void GuiControlArrayControlSetColSpacing(GuiControlArrayControl* ctrl, S32 spacing)
   {
      ctrl->setColSpacing(spacing);
   }
}