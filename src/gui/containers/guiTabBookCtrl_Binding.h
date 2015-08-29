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

// Empty for now, will implement for handling design time context menu for manipulating pages
ConsoleMethod(GuiTabBookCtrl, addPage, void, 2, 2, "() Empty")
{
   object->addNewPage();
}

//ConsoleMethod( GuiTabBookCtrl, removePage, void, 2, 2, "()")
//{
//}

ConsoleMethod(GuiTabBookCtrl, selectPage, void, 3, 3, "(int pageIndex)")
{
   S32 pageIndex = dAtoi(argv[2]);

   object->selectPage(pageIndex);
}

ConsoleMethod(GuiTabBookCtrl, selectPageName, void, 3, 3, "(pageName)")
{
   object->selectPage(argv[2]);
}

extern "C"{
   DLL_PUBLIC GuiTabBookCtrl* GuiTabBookCtrlCreateInstance()
   {
      return new GuiTabBookCtrl();
   }

   DLL_PUBLIC S32 GuiTabBookCtrlGetTabPosition(GuiTabBookCtrl* ctrl)
   {
      return ctrl->getTabPosition();
   }

   DLL_PUBLIC void GuiTabBookCtrlSetTabPosition(GuiTabBookCtrl* ctrl, S32 tabPosition)
   {
      ctrl->setTabPosition((GuiTabBookCtrl::TabPosition)tabPosition);
   }

   DLL_PUBLIC S32 GuiTabBookCtrlGetTabHeight(GuiTabBookCtrl* ctrl)
   {
      return ctrl->getTabHeight();
   }

   DLL_PUBLIC void GuiTabBookCtrlSetTabHeight(GuiTabBookCtrl* ctrl, S32 height)
   {
      ctrl->setTabHeight(height);
   }

   DLL_PUBLIC S32 GuiTabBookCtrlGetTabMargin(GuiTabBookCtrl* ctrl)
   {
      return ctrl->getTabMargin();
   }

   DLL_PUBLIC void GuiTabBookCtrlSetTabMargin(GuiTabBookCtrl* ctrl, S32 margin)
   {
      ctrl->setTabMargin(margin);
   }

   DLL_PUBLIC S32 GuiTabBookCtrlGetMinTabWidth(GuiTabBookCtrl* ctrl)
   {
      return ctrl->getMinTabWidth();
   }

   DLL_PUBLIC void GuiTabBookCtrlSetMinTabWidth(GuiTabBookCtrl* ctrl, S32 width)
   {
      ctrl->setMinTabWidth(width);
   }

   DLL_PUBLIC void GuiTabBookCtrlAddPage(GuiTabBookCtrl* ctrl)
   {
      ctrl->addNewPage();
   }

   DLL_PUBLIC void GuiTabBookCtrlSelectPage(GuiTabBookCtrl* ctrl, S32 pageIndex)
   {
      ctrl->selectPage(pageIndex);
   }

   DLL_PUBLIC void GuiTabBookCtrlSelectPageName(GuiTabBookCtrl* ctrl, const char* pageName)
   {
      ctrl->selectPage(pageName);
   }
}