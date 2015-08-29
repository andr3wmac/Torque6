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

ConsoleMethod(GuiFrameSetCtrl, frameBorder, void, 3, 4, "( index [ , enable = true ] ) Use the frameBorder method to change the frame's enable state.\n"
   "This function is not working as of this writing\n"
   "@param index Frame index to enable/disable/enable - Currently a boolean, but should actually be a string: alwaysOn, alwaysOff, dynamic.\n"
   "@return No return value.")
{
   S32 index = dAtoi(argv[2]);
   if (argc == 3)
      object->frameBorderEnable(index);
   else
      object->frameBorderEnable(index, argv[3]);
}

ConsoleMethod(GuiFrameSetCtrl, frameMovable, void, 3, 4, "( index [ , enable = true ] ) Use the frameMovable method to change the frame's draggable state.\n"
   "This function is not working as of this writing\n"
   "@param index Frame index to enable/disable/enable - Currently a boolean, but should actually be a string: alwaysOn, alwaysOff, dynamic.\n"
   "@return No return value.")
{
   S32 index = dAtoi(argv[2]);
   if (argc == 3)
      object->frameBorderMovable(index);
   else
      object->frameBorderMovable(index, argv[3]);
}

ConsoleMethod(GuiFrameSetCtrl, frameMinExtent, void, 5, 5, "(index, w, h ) Use the frameMinExtent method to set the minimum extent allowed for a frame.\n"
   "These minimum extents do not prevent a parent control from collapsing the frame control and its frames. These limits apply to dragging and resizing as is done with the frames' draggable borders\n"
   "@param index The frame number.\n"
   "@param w Minimum width in pixels.\n"
   "@param h Minimum height in pixels.\n"
   "@return No return value.")
{
   Point2I extent(getMax(0, dAtoi(argv[3])), getMax(0, dAtoi(argv[4])));
   object->frameMinExtent(dAtoi(argv[2]), extent);
}

ConsoleMethod(GuiFrameSetCtrl, addColumn, void, 2, 2, "() Use the addColumn method to add another column to the control.\n"
   "The current contents of the GUIFrameCtrl may shift to fill the new column. New columns are added on the right of the control.\n"
   "@return No return value.\n"
   "@sa addRow, removeColumn, removeRow")
{
   Vector<S32> * columns = object->columnOffsets();
   columns->push_back(0);
   object->balanceFrames();
}

ConsoleMethod(GuiFrameSetCtrl, addRow, void, 2, 2, "() Use the addRow method to add another row to the control.\n"
   "The current contents of the GUIFrameCtrl may shift to fill the new row. New rows are added on the bottom of the control.\n"
   "@return No return value.\n"
   "@sa addColumn, removeColumn, removeRow")
{
   Vector<S32> * rows = object->rowOffsets();
   rows->push_back(0);
   object->balanceFrames();
}

ConsoleMethod(GuiFrameSetCtrl, removeColumn, void, 2, 2, "() Use the removeColumn method to remove a column from the right side of the control.\n"
   "Columns are removed right to left.\n"
   "@return No return value.\n"
   "@sa addColumn, addRow, removeRow")
{
   Vector<S32> * columns = object->columnOffsets();

   if (columns->size() > 0)
   {
      columns->setSize(columns->size() - 1);
      object->balanceFrames();
   }
   else
      Con::errorf(ConsoleLogEntry::General, "No columns exist to remove");
}

ConsoleMethod(GuiFrameSetCtrl, removeRow, void, 2, 2, "() Use the removeRow method to remove the bottom row from the control.\n"
   "Rows are removed bottom to top.\n"
   "@return No return value.\n"
   "@sa addColumn, addRow, removeColumn")
{
   Vector<S32> * rows = object->rowOffsets();

   if (rows->size() > 0)
   {
      rows->setSize(rows->size() - 1);
      object->balanceFrames();
   }
   else
      Con::errorf(ConsoleLogEntry::General, "No rows exist to remove");
}

ConsoleMethod(GuiFrameSetCtrl, getColumnCount, S32, 2, 2, "() Use the getColumnCount method to determine the number of columns in this control.\n"
   "@return Returns an integer value equal to the number of columns in this frame.\n"
   "@sa getRowCount")
{
   return(object->columnOffsets()->size());
}

ConsoleMethod(GuiFrameSetCtrl, getRowCount, S32, 2, 2, "() Use the getRowCount method to determine the number of rows in this control.\n"
   "@return Returns an integer value equal to the number of rows in this frame.\n"
   "@sa getColumnCount")
{
   return(object->rowOffsets()->size());
}

ConsoleMethod(GuiFrameSetCtrl, getColumnOffset, S32, 3, 3, "( column ) Use the getColumnOffset method to determine the current pixel location of the specified column.\n"
   "Column 0 is the first column on the left side of frame 0. Column 1 is on the right side of frame 0 and the left side of frame 1, etc.\n"
   "@param column An integer value specifying the column to examine.\n"
   "@return Returns the pixel offset for the specified column.\n"
   "@sa getRowOffset, setColumnOffset, setRowOffset")
{
   S32 index = dAtoi(argv[2]);
   if (index < 0 || index > object->columnOffsets()->size())
   {
      Con::errorf(ConsoleLogEntry::General, "Column index out of range");
      return(0);
   }
   return((*object->columnOffsets())[index]);
}

ConsoleMethod(GuiFrameSetCtrl, getRowOffset, S32, 3, 3, "( row ) Use the getRowOffset method to determine the current pixel location of the specified row.\n"
   "Row 0 is the first row on the top of the first row of frames. Row 1 is below the first row of frames and above the second row of frames, etc. 1, etc.\n"
   "@param row An integer value specifying the row to examine.\n"
   "@return Returns the pixel offset for the specified row.\n"
   "@sa getColumnOffset, setColumnOffset, setRowOffset")
{
   S32 index = dAtoi(argv[2]);
   if (index < 0 || index > object->rowOffsets()->size())
   {
      Con::errorf(ConsoleLogEntry::General, "Row index out of range");
      return(0);
   }
   return((*object->rowOffsets())[index]);
}

ConsoleMethod(GuiFrameSetCtrl, setColumnOffset, void, 4, 4, "( column , offset ) Use the setColumnOffset method to determine the current pixel location of the specified column.\n"
   "Column 0 is the first column on the left side of frame 0. Column 1 is on the right side of frame 0 and the left side of frame 1, etc. The left-most and right-most columns cannot be moved.\n"
   "@param column An integer value specifying the column to examine.\n"
   "@param offset An integer value specifying the new column offset in pixels.\n"
   "@return No return value.\n"
   "@sa getColumnOffset, getRowOffset, setRowOffset")
{
   Vector<S32> & columns = *(object->columnOffsets());

   S32 index = dAtoi(argv[2]);
   if (index < 0 || index > columns.size())
   {
      Con::errorf(ConsoleLogEntry::General, "Column index out of range");
      return;
   }

   //
   S32 offset = dAtoi(argv[3]);

   // check the offset
   if (((index > 0) && (offset < columns[index - 1])) ||
      ((index < (columns.size() - 1)) && (offset > columns[index + 1])))
   {
      Con::errorf(ConsoleLogEntry::General, "Invalid column offset");
      return;
   }

   columns[index] = offset;
   object->updateSizes();
}

ConsoleMethod(GuiFrameSetCtrl, setRowOffset, void, 4, 4, "( row , offset ) Use the setRowOffset method to set the current pixel location of the specified row.\n"
   "Row 0 is the first row on the top of the first row of frames. Row 1 is below the first row of frames and above the second row of frames, etc. 1, etc. The bottom-most and top-most rows cannot be moved.\n"
   "@param row An integer value specifying the row to modify.\n"
   "@param offset An integer value specifying the new row offset in pixels.\n"
   "@return No return value.\n"
   "@sa getColumnOffset, getRowOffset, setColumnOffset")
{
   Vector<S32> & rows = *(object->rowOffsets());

   S32 index = dAtoi(argv[2]);
   if (index < 0 || index > rows.size())
   {
      Con::errorf(ConsoleLogEntry::General, "Row index out of range");
      return;
   }

   //
   S32 offset = dAtoi(argv[3]);

   // check the offset
   if (((index > 0) && (offset < rows[index - 1])) ||
      ((index < (rows.size() - 1)) && (offset > rows[index + 1])))
   {
      Con::errorf(ConsoleLogEntry::General, "Invalid row offset");
      return;
   }

   rows[index] = offset;
   object->updateSizes();
}

extern "C"{
   DLL_PUBLIC GuiFrameSetCtrl* GuiFrameSetCtrlCreateInstance()
   {
      return new GuiFrameSetCtrl();
   }

   //C-TODO implement columns and rows

   DLL_PUBLIC S32 GuiFrameSetCtrlGetBorderWidth(GuiFrameSetCtrl* ctrl)
   {
      return ctrl->getBorderWidth();
   }

   DLL_PUBLIC void GuiFrameSetCtrlSetBorderWidth(GuiFrameSetCtrl* ctrl, S32 width)
   {
      ctrl->setBorderWidth(width);
   }

   DLL_PUBLIC void GuiFrameSetCtrlGetBorderColor(GuiFrameSetCtrl* ctrl, CInterface::ColorParam* outCol)
   {
      *outCol = ctrl->getBorderColor();
   }

   DLL_PUBLIC void GuiFrameSetCtrlSetBorderColor(GuiFrameSetCtrl* ctrl, CInterface::ColorParam color)
   {
      ctrl->setBorderColor(color);
   }

   DLL_PUBLIC S32 GuiFrameSetCtrlGetBorderEnable(GuiFrameSetCtrl* ctrl)
   {
      return ctrl->getBorderEnable();
   }

   DLL_PUBLIC void GuiFrameSetCtrlSetBorderEnable(GuiFrameSetCtrl* ctrl, S32 enable)
   {
      ctrl->setBorderEnable(enable);
   }

   DLL_PUBLIC S32 GuiFrameSetCtrlGetBorderMovable(GuiFrameSetCtrl* ctrl)
   {
      return ctrl->getBorderMovable();
   }

   DLL_PUBLIC void GuiFrameSetCtrlSetBorderMovable(GuiFrameSetCtrl* ctrl, S32 movable)
   {
      ctrl->setBorderMovable(movable);
   }

   DLL_PUBLIC bool GuiFrameSetCtrlGetAutoBalance(GuiFrameSetCtrl* ctrl)
   {
      return ctrl->getAutoBalance();
   }

   DLL_PUBLIC void GuiFrameSetCtrlSetAutoBalance(GuiFrameSetCtrl* ctrl, bool enable)
   {
      ctrl->setAutoBalance(enable);
   }

   DLL_PUBLIC S32 GuiFrameSetCtrlGetFudgeFactor(GuiFrameSetCtrl* ctrl)
   {
      return ctrl->getFudgeFactor();
   }

   DLL_PUBLIC void GuiFrameSetCtrlSetFudgeFactor(GuiFrameSetCtrl* ctrl, S32 fudgeFactor)
   {
      ctrl->setFudgeFactor(fudgeFactor);
   }

   //CTODO default value
   DLL_PUBLIC void GuiFrameSetCtrlFrameBorder(GuiFrameSetCtrl* ctrl, S32 index, const char* state)
   {
      ctrl->frameBorderEnable(index, state);
   }

   //CTODO default value
   DLL_PUBLIC void GuiFrameSetCtrlFrameMovable(GuiFrameSetCtrl* ctrl, S32 index, const char* state)
   {
      ctrl->frameBorderMovable(index, state);
   }

   DLL_PUBLIC void GuiFrameSetCtrlFrameMinExtent(GuiFrameSetCtrl* ctrl, S32 index, S32 width, S32 height)
   {
      Point2I extent(getMax(0, width), getMax(0, height));
      ctrl->frameMinExtent(index, extent);
   }

   DLL_PUBLIC void GuiFrameSetCtrlAddColumn(GuiFrameSetCtrl* ctrl)
   {
      Vector<S32> * columns = ctrl->columnOffsets();
      columns->push_back(0);
      ctrl->balanceFrames();
   }

   DLL_PUBLIC void GuiFrameSetCtrlAddRow(GuiFrameSetCtrl* ctrl)
   {
      Vector<S32> * rows = ctrl->rowOffsets();
      rows->push_back(0);
      ctrl->balanceFrames();
   }

   DLL_PUBLIC void GuiFrameSetCtrlRemoveColumn(GuiFrameSetCtrl* ctrl)
   {
      Vector<S32> * columns = ctrl->columnOffsets();

      if (columns->size() > 0)
      {
         columns->setSize(columns->size() - 1);
         ctrl->balanceFrames();
      }
      else
         Con::errorf(ConsoleLogEntry::General, "No columns exist to remove");
   }

   DLL_PUBLIC void GuiFrameSetCtrlRemoveRow(GuiFrameSetCtrl* ctrl)
   {
      Vector<S32> * rows = ctrl->rowOffsets();

      if (rows->size() > 0)
      {
         rows->setSize(rows->size() - 1);
         ctrl->balanceFrames();
      }
      else
         Con::errorf(ConsoleLogEntry::General, "No rows exist to remove");
   }

   DLL_PUBLIC S32 GuiFrameSetCtrlGetColumnCount(GuiFrameSetCtrl* ctrl)
   {
      return ctrl->columnOffsets()->size();
   }

   DLL_PUBLIC S32 GuiFrameSetCtrlGetRowCount(GuiFrameSetCtrl* ctrl)
   {
      return ctrl->rowOffsets()->size();
   }

   DLL_PUBLIC S32 GuiFrameSetCtrlGetColumnOffset(GuiFrameSetCtrl* ctrl, S32 column)
   {
      if (column < 0 || column > ctrl->columnOffsets()->size())
      {
         Con::errorf(ConsoleLogEntry::General, "Column index out of range");
         return 0;
      }
      return (*ctrl->columnOffsets())[column];
   }

   DLL_PUBLIC S32 GuiFrameSetCtrlGetRowOffset(GuiFrameSetCtrl* ctrl, S32 row)
   {
      if (row < 0 || row > ctrl->rowOffsets()->size())
      {
         Con::errorf(ConsoleLogEntry::General, "Column index out of range");
         return 0;
      }
      return (*ctrl->rowOffsets())[row];
   }

   DLL_PUBLIC void GuiFrameSetCtrlSetColumnOffset(GuiFrameSetCtrl* ctrl, S32 column, S32 offset)
   {
      Vector<S32> & columns = *(ctrl->columnOffsets());

      if (column < 0 || column > columns.size())
      {
         Con::errorf(ConsoleLogEntry::General, "Column index out of range");
         return;
      }

      // check the offset
      if (((column > 0) && (offset < columns[column - 1])) ||
         ((column < (columns.size() - 1)) && (offset > columns[column + 1])))
      {
         Con::errorf(ConsoleLogEntry::General, "Invalid column offset");
         return;
      }

      columns[column] = offset;
      ctrl->updateSizes();
   }

   DLL_PUBLIC void GuiFrameSetCtrlSetRowOffset(GuiFrameSetCtrl* ctrl, S32 row, S32 offset)
   {
      Vector<S32> & rows = *(ctrl->rowOffsets());

      if (row < 0 || row > rows.size())
      {
         Con::errorf(ConsoleLogEntry::General, "Column index out of range");
         return;
      }

      // check the offset
      if (((row > 0) && (offset < rows[row - 1])) ||
         ((row < (rows.size() - 1)) && (offset > rows[row + 1])))
      {
         Con::errorf(ConsoleLogEntry::General, "Invalid column offset");
         return;
      }

      rows[row] = offset;
      ctrl->updateSizes();
   }
}