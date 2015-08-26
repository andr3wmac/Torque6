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
   DLL_PUBLIC GuiWindowCtrl* GuiWindowCtrlCreateInstance()
   {
      return new GuiWindowCtrl();
   }

   DLL_PUBLIC bool GuiWindowCtrlGetResizeWidth(GuiWindowCtrl* ctrl)
   {
      return ctrl->getResizeWidth();
   }

   DLL_PUBLIC void GuiWindowCtrlSetResizeWidth(GuiWindowCtrl* ctrl, bool enable)
   {
      ctrl->setResizeWidth(enable);
   }

   DLL_PUBLIC bool GuiWindowCtrlGetResizeHeight(GuiWindowCtrl* ctrl)
   {
      return ctrl->getResizeHeight();
   }

   DLL_PUBLIC void GuiWindowCtrlSetResizeHeight(GuiWindowCtrl* ctrl, bool enable)
   {
      ctrl->setResizeHeight(enable);
   }

   DLL_PUBLIC bool GuiWindowCtrlGetCanMove(GuiWindowCtrl* ctrl)
   {
      return ctrl->getCanMove();
   }

   DLL_PUBLIC void GuiWindowCtrlSetCanMove(GuiWindowCtrl* ctrl, bool enable)
   {
      ctrl->setCanMove(enable);
   }

   DLL_PUBLIC bool GuiWindowCtrlGetCanClose(GuiWindowCtrl* ctrl)
   {
      return ctrl->getCanClose();
   }

   DLL_PUBLIC void GuiWindowCtrlSetCanClose(GuiWindowCtrl* ctrl, bool enable)
   {
      ctrl->setCanClose(enable);
   }

   DLL_PUBLIC bool GuiWindowCtrlGetCanMinimize(GuiWindowCtrl* ctrl)
   {
      return ctrl->getCanMinimize();
   }

   DLL_PUBLIC void GuiWindowCtrlSetCanMinimize(GuiWindowCtrl* ctrl, bool enable)
   {
      ctrl->setCanMinimize(enable);
   }

   DLL_PUBLIC bool GuiWindowCtrlGetCanMaximize(GuiWindowCtrl* ctrl)
   {
      return ctrl->getCanMaximize();
   }

   DLL_PUBLIC void GuiWindowCtrlSetCanMaximize(GuiWindowCtrl* ctrl, bool enable)
   {
      ctrl->setCanMaximize(enable);
   }

   DLL_PUBLIC void GuiWindowCtrlGetMinSize(GuiWindowCtrl* ctrl, CInterface::Point2IParam* outSize)
   {
      *outSize = ctrl->getMinSize();
   }

   DLL_PUBLIC void GuiWindowCtrlSetMinSize(GuiWindowCtrl* ctrl, CInterface::Point2IParam size)
   {
      ctrl->setMinSize(size);
   }

   DLL_PUBLIC const char* GuiWindowCtrlGetCloseCommand(GuiWindowCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getCloseCommand());
   }

   DLL_PUBLIC void GuiWindowCtrlSetCloseCommand(GuiWindowCtrl* ctrl, const char* command)
   {
      ctrl->setCloseCommand(command);
   }
}