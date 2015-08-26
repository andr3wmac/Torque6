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

ConsoleMethod(GuiColorPickerCtrl, getSelectorPos, const char*, 2, 2, "() Gets the current position of the selector\n"
   "@return Returns the position of the selector as space-separted x,y coordinates.")
{
   char *temp = Con::getReturnBuffer(256);
   Point2I pos;
   pos = object->getSelectorPos();
   dSprintf(temp, 256, "%d %d", pos.x, pos.y);
   return temp;
}

ConsoleMethod(GuiColorPickerCtrl, getSelectorPos2, const char*, 2, 2, "() Gets the current position of the selector\n"
   "@return Returns the position of the selector as space-separted x,y coordinates.")
{
   char *temp = Con::getReturnBuffer(256);
   Point2I pos;
   pos = object->getSelectorPos();
   dSprintf(temp, 256, "%d %d", pos.x, pos.y);
   return temp;
}

ConsoleMethod(GuiColorPickerCtrl, getSelectorPosForColor, const char*, 3, 6, "(float red, float green, float blue, [float alpha = 1.0]) - Gets the selector position for the specified color."
   "The display mode must be pHorizColorRange, pVertColorRange, or pBlendColorRange.\n"
   "@param red The red value.\n"
   "@param green The green value.\n"
   "@param blue The blue value.\n"
   "@param alpha The alpha value.\n"
   "@return Returns the position of the selector as space-separted x,y coordinates.")
{
   // The colors.
   F32 red;
   F32 green;
   F32 blue;
   F32 alpha = 1.0f;

   // Grab the element count.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // Space separated.
   if (argc < 4)
   {
      // ("R G B [A]")
      if ((elementCount == 3) || (elementCount == 4))
      {
         // Extract the color.
         red = dAtof(Utility::mGetStringElement(argv[2], 0));
         green = dAtof(Utility::mGetStringElement(argv[2], 1));
         blue = dAtof(Utility::mGetStringElement(argv[2], 2));

         // Grab the alpha if it's there.
         if (elementCount > 3)
            alpha = dAtof(Utility::mGetStringElement(argv[2], 3));
      }

      // Invalid.
      else
      {
         Con::warnf("GuiColorPickerCtrl::getSelectorPosForColor() - Invalid Number of parameters!");
         return StringTable->EmptyString;
      }
   }

   // (R, G, B)
   else if (argc >= 5)
   {
      red = dAtof(argv[2]);
      green = dAtof(argv[3]);
      blue = dAtof(argv[4]);

      // Grab the alpha if it's there.
      if (argc > 5)
         alpha = dAtof(argv[5]);
   }

   // Invalid.
   else
   {
      Con::warnf("GuiColorPickerCtrl::getSelectorPosForColor() - Invalid Number of parameters!");
      return StringTable->EmptyString;
   }

   char *temp = Con::getReturnBuffer(256);
   Point2I pos;
   pos = object->getSelectorPositionForColor(object->mBounds, ColorF(red, green, blue, alpha));
   dSprintf(temp, 256, "%d %d", pos.x, pos.y);
   return temp;
}

ConsoleMethod(GuiColorPickerCtrl, setSelectorPos, void, 3, 3, "(\"x y\")Sets the current position of the selector"
   "@param The coordinates with space-separated formating.\n"
   "@return No return value.")
{
   Point2I newPos;
   dSscanf(argv[2], "%d %d", &newPos.x, &newPos.y);
   object->setSelectorPos(newPos);
}

ConsoleMethod(GuiColorPickerCtrl, updateColor, void, 2, 2, "() Forces update of pick color\n"
   "@return No return value.")
{
   object->updateColor();
}

extern "C"{
   DLL_PUBLIC GuiColorPickerCtrl* GuiColorPickerCtrlCreateInstance()
   {
      return new GuiColorPickerCtrl();
   }

   DLL_PUBLIC void GuiColorPickerCtrlGetBaseColor(GuiColorPickerCtrl* ctrl, CInterface::ColorParam* outColor)
   {
      *outColor = ctrl->getBaseColor();
   }

   DLL_PUBLIC void GuiColorPickerCtrlSetBaseColor(GuiColorPickerCtrl* ctrl, CInterface::ColorParam color)
   {
      ctrl->setBaseColor(color);
   }

   DLL_PUBLIC void GuiColorPickerCtrlGetPickColor(GuiColorPickerCtrl* ctrl, CInterface::ColorParam* outColor)
   {
      *outColor = ctrl->getPickColor();
   }

   DLL_PUBLIC void GuiColorPickerCtrlSetPickColor(GuiColorPickerCtrl* ctrl, CInterface::ColorParam color)
   {
      ctrl->setPickColor(color);
   }

   DLL_PUBLIC S32 GuiColorPickerCtrlGetSelectorGap(GuiColorPickerCtrl* ctrl)
   {
      return ctrl->getSelectorGap();
   }

   DLL_PUBLIC void GuiColorPickerCtrlSetSelectorGap(GuiColorPickerCtrl* ctrl, S32 gap)
   {
      ctrl->setSelectorGap(gap);
   }

   DLL_PUBLIC S32 GuiColorPickerCtrlGetDisplayMode(GuiColorPickerCtrl* ctrl)
   {
      return ctrl->getDisplayMode();
   }

   DLL_PUBLIC void GuiColorPickerCtrlSetDisplayMode(GuiColorPickerCtrl* ctrl, S32 mode)
   {
      ctrl->setDisplayMode((GuiColorPickerCtrl::PickMode)mode);
   }

   DLL_PUBLIC bool GuiColorPickerCtrlGetActionOnMove(GuiColorPickerCtrl* ctrl)
   {
      return ctrl->getActionOnMove();
   }

   DLL_PUBLIC void GuiColorPickerCtrlSetActionOnMove(GuiColorPickerCtrl* ctrl, bool actionOnMove)
   {
      ctrl->setActionOnMove(actionOnMove);
   }

   DLL_PUBLIC bool GuiColorPickerCtrlGetShowSelector(GuiColorPickerCtrl* ctrl)
   {
      return ctrl->getShowSelector();
   }

   DLL_PUBLIC void GuiColorPickerCtrlSetShowSelector(GuiColorPickerCtrl* ctrl, bool showSelector)
   {
      ctrl->setShowSelector(showSelector);
   }

   DLL_PUBLIC void GuiColorPickerCtrlGetSelectorPos(GuiColorPickerCtrl* ctrl, CInterface::Point2IParam* outPos)
   {
      *outPos = ctrl->getSelectorPos();
   }

   DLL_PUBLIC void GuiColorPickerCtrlGetSelectorPosForColor(GuiColorPickerCtrl* ctrl, CInterface::ColorParam col, CInterface::Point2IParam* outPos)
   {
      *outPos = ctrl->getSelectorPositionForColor(ctrl->mBounds, col);
   }

   DLL_PUBLIC void GuiColorPickerCtrlSetSelectorPos(GuiColorPickerCtrl* ctrl, CInterface::Point2IParam pos)
   {
      ctrl->setSelectorPos(pos);
   }

   DLL_PUBLIC void GuiColorPickerCtrlUpdateColor(GuiColorPickerCtrl* ctrl)
   {
      ctrl->updateColor();
   }
}