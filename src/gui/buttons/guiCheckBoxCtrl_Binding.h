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

ConsoleMethod(GuiCheckBoxCtrl, setStateOn, void, 3, 3, "(state) Sets the control as active and updates siblings of the same group."
   "@param state This argument may be a boolean value or an integer."
   "state < 0: Parent::setStateOn(false), obj::setActive(false)\n"
   "state == 0 (or false): Parent::setStateOn(false), obj::setActive(true)\n"
   "state > 0 (or true): Parent::setStateOn(true), obj::setActive(true)")
{
   if (dStricmp(argv[2], "true") == 0)
      object->setStateOn(1);
   else if (dStricmp(argv[2], "false") == 0)
      object->setStateOn(0);
   else
      object->setStateOn(dAtoi(argv[2]));
}

extern "C"{
   DLL_PUBLIC GuiCheckBoxCtrl* GuiCheckBoxCtrlCreateInstance()
   {
      return new GuiCheckBoxCtrl();
   }

   DLL_PUBLIC bool GuiCheckBoxCtrlGetUseInactiveState(GuiCheckBoxCtrl* ctrl)
   {
      return ctrl->getUseInactiveState();
   }

   DLL_PUBLIC void GuiCheckBoxCtrlSetUseInactiveState(GuiCheckBoxCtrl* ctrl, bool useInactiveState)
   {
      ctrl->setUseInactiveState(useInactiveState);
   }

   DLL_PUBLIC void GuiCheckBoxCtrlSetStateOn(GuiCheckBoxCtrl* ctrl, S32 state)
   {
      ctrl->setStateOn(state);
   }
}