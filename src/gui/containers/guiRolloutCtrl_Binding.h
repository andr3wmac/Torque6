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

ConsoleMethod(GuiRolloutCtrl, isExpanded, bool, 2, 2, "() @return Returns true if object is expanded, false otherwise")
{
   return object->isExpanded();
}

ConsoleMethod(GuiRolloutCtrl, collapse, void, 2, 2, "() Collapses control.\n @return No return value.")
{
   object->collapse();
}

ConsoleMethod(GuiRolloutCtrl, expand, void, 2, 2, "() Expands control\n @return No return value.")
{
   object->expand();
}

ConsoleMethod(GuiRolloutCtrl, instantCollapse, void, 2, 2, "() Collapses control without animation.\n @return No return value.")
{
   object->instantCollapse();
}

ConsoleMethod(GuiRolloutCtrl, instantExpand, void, 2, 2, "() Expands control without animation.\n @return No return value.")
{
   object->instantExpand();
}

ConsoleMethod(GuiRolloutCtrl, sizeToContents, void, 2, 2, "() Resizes control to fit its contents\n @return No return value.")
{
   object->sizeToContents();
}

extern "C"{
   DLL_PUBLIC GuiRolloutCtrl* GuiRolloutCtrlCreateInstance()
   {
      return new GuiRolloutCtrl();
   }

   DLL_PUBLIC const char* GuiRolloutCtrlGetCaption(GuiRolloutCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getCaption());
   }

   DLL_PUBLIC void GuiRolloutCtrlSetCaption(GuiRolloutCtrl* ctrl, const char* caption)
   {
      ctrl->setCaption(caption);
   }

   DLL_PUBLIC void GuiRolloutCtrlGetMargin(GuiRolloutCtrl* ctrl, CInterface::Point2IParam* outMargin)
   {
      *outMargin = ctrl->getMargin();
   }

   DLL_PUBLIC void GuiRolloutCtrlSetMargin(GuiRolloutCtrl* ctrl, CInterface::Point2IParam margin)
   {
      ctrl->setMargin(margin);
   }

   DLL_PUBLIC S32 GuiRolloutCtrlGetDefaultHeight(GuiRolloutCtrl* ctrl)
   {
      return ctrl->getDefaultHeight();
   }

   DLL_PUBLIC void GuiRolloutCtrlSetDefaultHeight(GuiRolloutCtrl* ctrl, S32 height)
   {
      ctrl->setDefaultHeight(height);
   }

   DLL_PUBLIC bool GuiRolloutCtrlGetCollapsed(GuiRolloutCtrl* ctrl)
   {
      return ctrl->getCollapsed();
   }

   DLL_PUBLIC void GuiRolloutCtrlSetCollapsed(GuiRolloutCtrl* ctrl, bool enable)
   {
      if (enable)
         ctrl->instantCollapse();
      else
         ctrl->instantExpand();
   }

   DLL_PUBLIC bool GuiRolloutCtrlGetClickCollapse(GuiRolloutCtrl* ctrl)
   {
      return ctrl->getCanCollapse();
   }

   DLL_PUBLIC void GuiRolloutCtrlSetClickCollapse(GuiRolloutCtrl* ctrl, bool enable)
   {
      ctrl->setCanCollapse(enable);
   }

   DLL_PUBLIC bool GuiRolloutCtrlIsExpanded(GuiRolloutCtrl* ctrl)
   {
      return ctrl->isExpanded();
   }

   DLL_PUBLIC void GuiRolloutCtrlCollapse(GuiRolloutCtrl* ctrl)
   {
      ctrl->collapse();
   }

   DLL_PUBLIC void GuiRolloutCtrlExpand(GuiRolloutCtrl* ctrl)
   {
      ctrl->expand();
   }

   DLL_PUBLIC void GuiRolloutCtrlInstantCollapse(GuiRolloutCtrl* ctrl)
   {
      ctrl->instantCollapse();
   }

   DLL_PUBLIC void GuiRolloutCtrlInstantExpand(GuiRolloutCtrl* ctrl)
   {
      ctrl->instantExpand();
   }

   DLL_PUBLIC void GuiRolloutCtrlSizeToContents(GuiRolloutCtrl* ctrl)
   {
      ctrl->sizeToContents();
   }
}