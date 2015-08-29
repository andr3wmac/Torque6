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

ConsoleMethod(GuiDragAndDropControl, startDragging, void, 2, 4, "( int x, int y ) "
   "@param x X coordinate relative to the point on the object on which you are clicking\n"
   "@param y Y coordinate relative to the point on the object on which you are clicking\n"
   "@note If no point is passed, or only one coordinate is passed, the method defaults to (0,0)")
{
   Point2I offset = Point2I(0, 0);
   if (argc > 3)
   {
      offset.x = dAtoi(argv[2]);
      offset.y = dAtoi(argv[3]);
   }
   object->startDragging(offset);
}

extern "C"{
   DLL_PUBLIC GuiDragAndDropControl* GuiDragAndDropControlCreateInstance()
   {
      return new GuiDragAndDropControl();
   }

   DLL_PUBLIC bool GuiDragAndDropControlGetDeleteOnMouseUp(GuiDragAndDropControl* ctrl)
   {
      return ctrl->getDeleteOnMouseUp();
   }

   DLL_PUBLIC void GuiDragAndDropControlSetDeleteOnMouseUp(GuiDragAndDropControl* ctrl, bool deleteOnMouseUp)
   {
      ctrl->setDeleteOnMouseUp(deleteOnMouseUp);
   }

   DLL_PUBLIC void GuiDragAndDropControlStartDragging(GuiDragAndDropControl* ctrl, CInterface::Point2IParam offset)
   {
      ctrl->startDragging(offset);
   }
}