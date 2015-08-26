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

ConsoleMethod(GuiFilterCtrl, getValue, const char*, 2, 2, "() Use the getValue method to get all the values in the filter.\n"
   "@return Returns a n-tuple floating-point vector of values for each knot (left to right)")
{
   static char buffer[512];
   const Filter *filter = object->get();
   *buffer = 0;

   for (U32 i = 0; i < (U32)filter->size(); i++)
   {
      char value[32];
      dSprintf(value, 31, "%1.5f ", *(filter->begin() + i));
      dStrcat(buffer, value);
   }

   return buffer;
}

ConsoleMethod(GuiFilterCtrl, setValue, void, 3, 20, "( knots ) Sets the knot value for each knot by position.\n"
   "@param knots a vector containing the knot positions for each knot. Each knot has a value between 0.0 and 1.0.\n"
   "@return No return value")
{
   Filter filter;

   argc -= 2;
   argv += 2;

   filter.set(argc, argv);
   object->set(filter);
}

ConsoleMethod(GuiFilterCtrl, identity, void, 2, 2, "() Resets the filter and places all knots on a line following a 45 degree angle from 0.0 (bottom) on the left to 1.0 (top) on the right.\n"
   "@return No return value")
{
   object->identity();
}

extern "C"{
   DLL_PUBLIC GuiFilterCtrl* GuiFilterCtrlCreateInstance()
   {
      return new GuiFilterCtrl();
   }

   DLL_PUBLIC S32 GuiFilterCtrlGetControlPoints(GuiFilterCtrl* ctrl)
   {
      return ctrl->getControlPointRequest();
   }

   DLL_PUBLIC void GuiFilterCtrlSetControlPoints(GuiFilterCtrl* ctrl, S32 points)
   {
      ctrl->setControlPointRequest(points);
   }

   DLL_PUBLIC const char* GuiFilterCtrlGetValue(GuiFilterCtrl* ctrl)
   {
      static char buffer[512];
      const Filter *filter = ctrl->get();
      *buffer = 0;

      for (U32 i = 0; i < (U32)filter->size(); i++)
      {
         char value[32];
         dSprintf(value, 31, "%1.5f ", *(filter->begin() + i));
         dStrcat(buffer, value);
      }

      return CInterface::GetMarshallableString(buffer);
   }

   //CTODO - list
   DLL_PUBLIC void GuiFilterCtrlSetValue(GuiFilterCtrl* ctrl, S32 argc, const char** argv)
   {
      Filter filter;

      filter.set(argc, argv);
      ctrl->set(filter);
   }

   DLL_PUBLIC void GuiFilterCtrlIdentity(GuiFilterCtrl* ctrl)
   {
      ctrl->identity();
   }
}