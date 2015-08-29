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

ConsoleMethod(GuiStackControl, updateStack, void, 2, 2, "%stackCtrl.updateStack() - Restacks controls it owns")
{
   object->updatePanes();
}

extern "C"{
   DLL_PUBLIC GuiStackControl* GuiStackControlCreateInstance()
   {
      return new GuiStackControl();
   }

   DLL_PUBLIC S32 GuiStackControlGetStackingType(GuiStackControl* ctrl)
   {
      return ctrl->getStackingType();
   }

   DLL_PUBLIC void GuiStackControlSetStackingType(GuiStackControl* ctrl, S32 type)
   {
      ctrl->setStackingType(type);
   }

   DLL_PUBLIC S32 GuiStackControlGetHorizStacking(GuiStackControl* ctrl)
   {
      return ctrl->getStackHorizSizing();
   }

   DLL_PUBLIC void GuiStackControlSetHorizStacking(GuiStackControl* ctrl, S32 type)
   {
      ctrl->setStackHorizSizing(type);
   }

   DLL_PUBLIC S32 GuiStackControlGetVertStacking(GuiStackControl* ctrl)
   {
      return ctrl->getStackVertSizing();
   }

   DLL_PUBLIC void GuiStackControlSetVertStacking(GuiStackControl* ctrl, S32 type)
   {
      ctrl->setStackVertSizing(type);
   }

   DLL_PUBLIC S32 GuiStackControlGetPadding(GuiStackControl* ctrl)
   {
      return ctrl->getPadding();
   }

   DLL_PUBLIC void GuiStackControlSetPadding(GuiStackControl* ctrl, S32 padding)
   {
      ctrl->setPadding(padding);
   }

   DLL_PUBLIC void GuiStackControlUpdateStack(GuiStackControl* ctrl)
   {
      ctrl->updatePanes();
   }
}