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
   DLL_PUBLIC GuiScriptNotifyCtrl* GuiScriptNotifyCtrlCreateInstance()
   {
      return new GuiScriptNotifyCtrl();
   }

   DLL_PUBLIC bool GuiScriptNotifyCtrlGetOnChildAdded(GuiScriptNotifyCtrl* ctrl)
   {
      return ctrl->mOnChildAdded;
   }

   DLL_PUBLIC void GuiScriptNotifyCtrlSetOnChildAdded(GuiScriptNotifyCtrl* ctrl, bool value)
   {
      ctrl->mOnChildAdded = value;
   }

   DLL_PUBLIC bool GuiScriptNotifyCtrlGetOnChildRemoved(GuiScriptNotifyCtrl* ctrl)
   {
      return ctrl->mOnChildRemoved;
   }

   DLL_PUBLIC void GuiScriptNotifyCtrlSetOnChildRemoved(GuiScriptNotifyCtrl* ctrl, bool value)
   {
      ctrl->mOnChildRemoved = value;
   }

   DLL_PUBLIC bool GuiScriptNotifyCtrlGetOnChildResized(GuiScriptNotifyCtrl* ctrl)
   {
      return ctrl->mOnChildResized;
   }

   DLL_PUBLIC void GuiScriptNotifyCtrlSetOnChildResized(GuiScriptNotifyCtrl* ctrl, bool value)
   {
      ctrl->mOnChildResized = value;
   }

   DLL_PUBLIC bool GuiScriptNotifyCtrlGetOnParentResized(GuiScriptNotifyCtrl* ctrl)
   {
      return ctrl->mOnParentResized;
   }

   DLL_PUBLIC void GuiScriptNotifyCtrlSetOnParentResized(GuiScriptNotifyCtrl* ctrl, bool value)
   {
      ctrl->mOnParentResized = value;
   }

   DLL_PUBLIC bool GuiScriptNotifyCtrlGetOnResize(GuiScriptNotifyCtrl* ctrl)
   {
      return ctrl->mOnResize;
   }

   DLL_PUBLIC void GuiScriptNotifyCtrlSetOnResize(GuiScriptNotifyCtrl* ctrl, bool value)
   {
      ctrl->mOnResize = value;
   }

   DLL_PUBLIC bool GuiScriptNotifyCtrlGetOnLoseFirstResponder(GuiScriptNotifyCtrl* ctrl)
   {
      return ctrl->mOnLoseFirstResponder;
   }

   DLL_PUBLIC void GuiScriptNotifyCtrlSetOnLoseFirstResponder(GuiScriptNotifyCtrl* ctrl, bool value)
   {
      ctrl->mOnLoseFirstResponder = value;
   }

   DLL_PUBLIC bool GuiScriptNotifyCtrlGetOnGainFirstResponder(GuiScriptNotifyCtrl* ctrl)
   {
      return ctrl->mOnGainFirstResponder;
   }

   DLL_PUBLIC void GuiScriptNotifyCtrlSetOnGainFirstResponder(GuiScriptNotifyCtrl* ctrl, bool value)
   {
      ctrl->mOnGainFirstResponder = value;
   }
}