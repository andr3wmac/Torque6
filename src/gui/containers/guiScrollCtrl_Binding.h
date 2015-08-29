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

ConsoleMethod(GuiScrollCtrl, scrollToTop, void, 2, 2, "() Use the scrollToTop method to scroll the scroll control to the top of the child content area.\n"
   "@return No return value.\n"
   "@sa scrollToBottom")
{
   object->scrollTo(0, 0);
}

ConsoleMethod(GuiScrollCtrl, scrollToBottom, void, 2, 2, "() Use the scrollToBottom method to scroll the scroll control to the bottom of the child content area.\n"
   "@return No return value.\n"
   "@sa scrollToTop")
{
   object->scrollTo(0, 0x7FFFFFFF);
}

ConsoleMethod(GuiScrollCtrl, setScrollPosition, void, 4, 4, "(x, y) - scrolls the scroll control to the specified position.")
{
   object->scrollTo(dAtoi(argv[2]), dAtoi(argv[3]));
}

ConsoleMethod(GuiScrollCtrl, getScrollPositionX, S32, 2, 2, "() - get the current x scroll position of the scroll control.")
{
   return object->getChildRelPos().x;
}

ConsoleMethod(GuiScrollCtrl, getScrollPositionY, S32, 2, 2, "() - get the current y scroll position of the scroll control.")
{
   return object->getChildRelPos().y;
}

ConsoleMethod(GuiScrollCtrl, computeSizes, void, 2, 2, "() - refresh all the contents in this scroll container.")
{
   return object->computeSizes();
}

ConsoleMethod(GuiScrollCtrl, getUseScrollEvents, bool, 2, 2, "() - get the current scroll callback state of the scroll control.")
{
   return object->getUseScrollEvents();
}

ConsoleMethod(GuiScrollCtrl, setUseScrollEvents, void, 3, 3, "() - set the scroll callback state of the scroll control.")
{
   object->setUseScrollEvents(dAtoi(argv[2]));
}

extern "C"{
   DLL_PUBLIC GuiScrollCtrl* GuiScrollCtrlCreateInstance()
   {
      return new GuiScrollCtrl();
   }

   DLL_PUBLIC bool GuiScrollCtrlGetWillFirstRespond(GuiScrollCtrl* ctrl)
   {
      return ctrl->mWillFirstRespond;
   }

   DLL_PUBLIC void GuiScrollCtrlSetWillFirstRespond(GuiScrollCtrl* ctrl, bool enable)
   {
      ctrl->mWillFirstRespond = enable;
   }

   DLL_PUBLIC S32 GuiScrollCtrlGetHScrollBar(GuiScrollCtrl* ctrl)
   {
      return ctrl->mForceHScrollBar;
   }

   DLL_PUBLIC void GuiScrollCtrlSetHScrollBar(GuiScrollCtrl* ctrl, S32 forceHScrollbar)
   {
      ctrl->mForceHScrollBar = forceHScrollbar;
   }

   DLL_PUBLIC S32 GuiScrollCtrlGetVScrollBar(GuiScrollCtrl* ctrl)
   {
      return ctrl->mForceVScrollBar;
   }

   DLL_PUBLIC void GuiScrollCtrlSetVScrollBar(GuiScrollCtrl* ctrl, S32 forceVScrollbar)
   {
      ctrl->mForceVScrollBar = forceVScrollbar;
   }

   DLL_PUBLIC bool GuiScrollCtrlGetConstantThumbHeight(GuiScrollCtrl* ctrl)
   {
      return ctrl->mUseConstantHeightThumb;
   }

   DLL_PUBLIC void GuiScrollCtrlSetConstantThumbHeight(GuiScrollCtrl* ctrl, bool useConstantThumbHeight)
   {
      ctrl->mUseConstantHeightThumb = useConstantThumbHeight;
   }

   DLL_PUBLIC void GuiScrollCtrlGetChildMargin(GuiScrollCtrl* ctrl, CInterface::Point2IParam* outMargin)
   {
      *outMargin = ctrl->getChildMargin();
   }

   DLL_PUBLIC void GuiScrollCtrlSetChildMargin(GuiScrollCtrl* ctrl, CInterface::Point2IParam margin)
   {
      ctrl->setChildMargin(margin);
   }

   DLL_PUBLIC void GuiScrollCtrlScrollToTop(GuiScrollCtrl* ctrl)
   {
      ctrl->scrollTo(0, 0);
   }

   DLL_PUBLIC void GuiScrollCtrlScrollToBottom(GuiScrollCtrl* ctrl)
   {
      ctrl->scrollTo(0, 0x7FFFFFFF);
   }

   DLL_PUBLIC void GuiScrollCtrlSetScrollPosition(GuiScrollCtrl* ctrl, CInterface::Point2IParam pos)
   {
      ctrl->scrollTo(pos.x, pos.y);
   }

   DLL_PUBLIC S32 GuiScrollCtrlGetScrollPositionX(GuiScrollCtrl* ctrl)
   {
      return ctrl->getChildRelPos().x;
   }

   DLL_PUBLIC S32 GuiScrollCtrlGetScrollPositionY(GuiScrollCtrl* ctrl)
   {
      return ctrl->getChildRelPos().y;
   }

   DLL_PUBLIC void GuiScrollCtrlComputeSizes(GuiScrollCtrl* ctrl)
   {
      ctrl->computeSizes();
   }

   DLL_PUBLIC bool GuiScrollCtrlGetUseScrollEvents(GuiScrollCtrl* ctrl)
   {
      return ctrl->getUseScrollEvents();
   }

   DLL_PUBLIC void GuiScrollCtrlSetUseScrollEvents(GuiScrollCtrl* ctrl, bool useScrollEvents)
   {
      ctrl->setUseScrollEvents(useScrollEvents);
   }
}