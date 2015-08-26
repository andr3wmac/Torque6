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

ConsoleMethod(GuiPaneControl, setCollapsed, void, 3, 3, "(bool collapsed) Sets the controls \"collapsed\" property\n"
   "@param collapsed True if the control is to be collapsed"
   "@return No return value.")
{
   object->setCollapsed(dAtob(argv[2]));
}

extern "C"{
   DLL_PUBLIC GuiPaneControl* GuiPaneControlCreateInstance()
   {
      return new GuiPaneControl();
   }

   DLL_PUBLIC const char* GuiPaneControlGetCaption(GuiPaneControl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getCaption());
   }

   DLL_PUBLIC void GuiPaneControlSetCaption(GuiPaneControl* ctrl, const char* caption)
   {
      ctrl->setCaption(caption);
   }

   DLL_PUBLIC const char* GuiPaneControlGetCaptionID(GuiPaneControl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getCaptionID());
   }

   DLL_PUBLIC void GuiPaneControlSetCaptionID(GuiPaneControl* ctrl, const char* captionID)
   {
      ctrl->setCaptionID(captionID);
   }

   DLL_PUBLIC bool GuiPaneControlGetCollapsable(GuiPaneControl* ctrl)
   {
      return ctrl->getCollapsable();
   }

   DLL_PUBLIC void GuiPaneControlSetCollapsable(GuiPaneControl* ctrl, bool collapsable)
   {
      ctrl->setCollapsable(collapsable);
   }

   DLL_PUBLIC bool GuiPaneControlGetBarBehindText(GuiPaneControl* ctrl)
   {
      return ctrl->getBarBehindText();
   }

   DLL_PUBLIC void GuiPaneControlSetBarBehindText(GuiPaneControl* ctrl, bool enable)
   {
      ctrl->setBarBehindText(enable);
   }

   DLL_PUBLIC void GuiPaneControlSetCollapsed(GuiPaneControl* ctrl, bool collapsed)
   {
      ctrl->setCollapsed(collapsed);
   }
}