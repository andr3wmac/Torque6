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

ConsoleMethod(GuiTextListCtrl, getSelectedId, S32, 2, 2, "() Use the getSelectedId method to return the ID value of the currently selected entry (if any).\n"
   "@return Returns the integer ID of the currently selected row or -1 if no row is selected.\n"
   "@sa addRow, clearSelected, getRowID, getRowNumByID, getRowTextByID")
{
   return object->getSelectedId();
}

ConsoleMethod(GuiTextListCtrl, setSelectedById, void, 3, 3, "( ID ) Use the setSelectedById method to selected a row by a specified ID. This will select the first row found to have an ID matching the specified ID.\n"
   "No selection will be made if no row has a matching ID. Additionally, if no selection is made and a prior row was selected, that selection will stay in effect.\n"
   "@param ID An integer value equal to the entry ID of the row to select.\n"
   "@return No return value.\n"
   "@sa setSelectedRow")
{
   S32 index = object->findEntryById(dAtoi(argv[2]));
   if (index < 0)
      return;

   object->setSelectedCell(Point2I(0, index));
}

ConsoleMethod(GuiTextListCtrl, setSelectedRow, void, 3, 3, "(int rowNum) "
   "Selects the specified row.\n"
   "@return No return value.")
{
   object->setSelectedCell(Point2I(0, dAtoi(argv[2])));
}

ConsoleMethod(GuiTextListCtrl, getSelectedRow, S32, 2, 2, "( row ) Use the setSelectedRow method to select a specified row in the list.\n"
   "Row numbers start at 0. No selection will be made if the row number is out of bounds. Additionally, if no selection is made and a prior row was selected, that selection will stay in effect.\n"
   "@param row The number of the list row to set as selected.\n"
   "@return No return value.\n"
   "@sa setSelectedByID")
{
   return object->getSelectedRow();
}
ConsoleMethod(GuiTextListCtrl, clearSelection, void, 2, 2, "() Use the clearSelection method to deselect the current selection (if any).\n"
   "@return No return value.\n"
   "@sa clear, setSelection")
{
   object->setSelectedCell(Point2I(-1, -1));
}

ConsoleMethod(GuiTextListCtrl, addRow, S32, 4, 5, "( ID , text [ , row ] ) Use the addRow method to add a new entry to the text list.\n"
   "@param ID The integer ID to assign to this entry. May be between 0 and inf and can be repeated. i.e., multiple entries may have the same ID.\n"
   "@param text The text to display for this entry in the list.\n"
   "@param row An optional integer value representing the position at which to add this entry, where 0 is the top of the list, 1 is after them first entry (if any), etc.\n"
   "@return Returns the row number of the new entry.\n"
   "@sa clear, removeRow")
{
   S32 ret = object->mList.size();
   if (argc < 5)
      object->addEntry(dAtoi(argv[2]), argv[3]);
   else
      object->insertEntry(dAtoi(argv[2]), argv[3], dAtoi(argv[4]));

   return ret;
}

ConsoleMethod(GuiTextListCtrl, setRowById, void, 4, 4, "( ID , text ) Use the setRowById method to update the text if the first row fond with an ID matching the specified ID.\n"
   "@param ID An integer value equal to the entry ID of the row to change the text for.\n"
   "@param text The text to replace the found row value with.\n"
   "@return No return value")
{
   object->setEntry(dAtoi(argv[2]), argv[3]);
}

ConsoleMethod(GuiTextListCtrl, sort, void, 3, 4, "( columnID [ , ascending = false ] ) Use the sort method to sort the list using a lexicographic sort. The sort order may be either ascending or descending (default).\n"
   "Columns may be specified when setting up the list, by default most lists have one column so the columnID should be 0.\n"
   "@param columnID The column to sort on.\n"
   "@param ascending An optional boolean value, which when true means to do an ascending sort, otherwise the sort will be descending.\n"
   "@return No return value.\n"
   "@sa sortNumerical")
{
   if (argc == 3)
      object->sort(dAtoi(argv[2]));
   else
      object->sort(dAtoi(argv[2]), dAtob(argv[3]));
}

ConsoleMethod(GuiTextListCtrl, sortNumerical, void, 3, 4, "( columnID [ , ascending = false] ) Use the sortNumerical method to sort the list using a numeric sort. The sort order may be either ascending or descending (default).\n"
   "Columns may be specified when setting up the list, by default most lists have one column so the columnID should be 0.\n"
   "@param columnID The column to sort on.\n"
   "@param ascending An optional boolean value, which when true means to do an ascending sort, otherwise the sort will be descending.\n"
   "@return No return value.\n"
   "@sa sort")
{
   if (argc == 3)
      object->sortNumerical(dAtoi(argv[2]));
   else
      object->sortNumerical(dAtoi(argv[2]), dAtob(argv[3]));
}

ConsoleMethod(GuiTextListCtrl, clear, void, 2, 2, "() Use the clear method to remove all entries from the list.\n"
   "@return No return value.\n"
   "@sa addRow, clearSelection, removeRow")
{
   object->clear();
}

ConsoleMethod(GuiTextListCtrl, rowCount, S32, 2, 2, "() Use the rowCount method to determine how many entries are in the list.\n"
   "@return Returns 0 if the list is entry or a positive integer value equal to the number of rows in the list if it is not empty.\n"
   "@sa add, removeRow")
{
   return object->getNumEntries();
}

ConsoleMethod(GuiTextListCtrl, getRowId, S32, 3, 3, "( row ) Use the getRowId method to get the ID value for a specified row.\n"
   "Row numbers start at 0.\n"
   "@param row The row in the list to check the ID for.\n"
   "@return Returns the ID of the specified row, or -1 if row is out of range.\n"
   "@sa addRow, getRowNumByID, getRowText, getRowTextByID")
{
   U32 index = dAtoi(argv[2]);
   if (index >= object->getNumEntries())
      return -1;

   return object->mList[index].id;
}

ConsoleMethod(GuiTextListCtrl, getRowTextById, const char*, 3, 3, "( ID ) Use the getRowTextById method to get the text of the first row with an ID matching the passed ID.\n"
   "@param ID An integer value equal to the entry ID to search for.\n"
   "@return Returns a string containing the text of the first row with a matching ID, or the NULL string if no row matches the specified ID.\n"
   "@sa addRow, findTextIndex, getRowID, getRowNumByID, getRowTextByID")
{
   S32 index = object->findEntryById(dAtoi(argv[2]));
   if (index < 0)
      return "";
   return object->mList[index].text;
}

ConsoleMethod(GuiTextListCtrl, getRowNumById, S32, 3, 3, "( ID ) Use the getRowNumById method to get the row number of the first entry in the list with the specified ID.\n"
   "Row numbers start at 0.\n"
   "@param ID An integer value equal to an the entry ID to search for.\n"
   "@return Returns the number of the first row found with a matching ID, or -1 if no row contains the specified ID.\n"
   "@sa addRow, getRowID, getRowText, getRowTextByID")
{
   S32 index = object->findEntryById(dAtoi(argv[2]));
   if (index < 0)
      return -1;
   return index;
}

ConsoleMethod(GuiTextListCtrl, getRowText, const char*, 3, 3, "( row ) Use the getRowText method to retrieve the text value of an entry in the list at the specified row.\n"
   "Row numbers start at 0.\n"
   "@param row The number of the list row from which to retrieve the text.\n"
   "@return Returns the text found at the specified row, or the NULL string if the row number is out of bounds.\n"
   "@sa addRow, getRowID, getRowNumByID, getRowTextByID")
{
   S32 index = dAtoi(argv[2]);
   if (index >= object->mList.size())
      return "";
   return object->mList[index].text;
}

ConsoleMethod(GuiTextListCtrl, removeRowById, void, 3, 3, "( ID ) Use the removeRowById method to remove the first row containing a matching ID.\n"
   "@param ID An integer value equal to the entry ID of the row to delete.\n"
   "@return No return value.\n"
   "@sa add, removeRow, rowCount")
{
   object->removeEntry(dAtoi(argv[2]));
}

ConsoleMethod(GuiTextListCtrl, removeRow, void, 3, 3, "( row ) Use the removeRow method to remove the specified row from the list.\n"
   "Row numbers start at 0. Nothing is removed if row is out of bounds.\n"
   "@param row The number of the list row to be removed.\n"
   "@return No return value.\n"
   "@sa add, removeRowbyID")
{
   U32 index = dAtoi(argv[2]);
   object->removeEntryByIndex(index);
}

ConsoleMethod(GuiTextListCtrl, scrollVisible, void, 3, 3, "( row ) Use the scrollVisible method to force the scrollList containing this text list to scroll so that the specified row is visible.\n"
   "Row numbers start at 0.\n"
   "@param row The number of the list row to be scrolled to.\n"
   "@return No return value.\n"
   "@sa rowCount")
{
   object->scrollCellVisible(Point2I(0, dAtoi(argv[2])));
}

ConsoleMethod(GuiTextListCtrl, findTextIndex, S32, 3, 3, "( text ) Use the findTextIndex method to do an exact-match search for text in the list of items.\n"
   "@param text The text to search for. Must match exactly or no match will occur.\n"
   "@return No return value.\n"
   "@sa getRowText, getRowTextByID")
{
   return(object->findEntryByText(argv[2]));
}

ConsoleMethod(GuiTextListCtrl, setRowActive, void, 4, 4, "( row , active ) Use the setRowActive method to activate or deactivate the specified row.\n"
   "Row numbers start at 0. The row will not change visibly, but we can check if a selected row is active later to determine whether to respond or not to this selection.\n"
   "@param row The number of the list row to activate or deactivate.\n"
   "@param active A boolean value specifying whether this row is active or inactive.\n"
   "@return No return value.\n"
   "@sa isRowActive")
{
   object->setEntryActive(U32(dAtoi(argv[2])), dAtob(argv[3]));
}

ConsoleMethod(GuiTextListCtrl, isRowActive, bool, 3, 3, "( row ) Use the isRowActive method to determine if the specified row is active.\n"
   "Row numbers start at 0.\n"
   "@param row The row to check the active status for.\n"
   "@return Returns 1 if the row is active, or 0 if the row is inactive or the specified row is out of bounds.\n"
   "@sa setRowActive")
{
   return(object->isEntryActive(U32(dAtoi(argv[2]))));
}

extern "C"{
   DLL_PUBLIC GuiTextListCtrl* GuiTextListCtrlCreateInstance()
   {
      return new GuiTextListCtrl();
   }

   DLL_PUBLIC bool GuiTextListCtrlGetEnumerate(GuiTextListCtrl* ctrl)
   {
      return ctrl->getEnumerate();
   }
   
   DLL_PUBLIC void GuiTextListCtrlSetEnumerate(GuiTextListCtrl* ctrl, bool enumerate)
   {
      ctrl->setEnumerate(enumerate);
   }
   
   DLL_PUBLIC bool GuiTextListCtrlGetResizeCell(GuiTextListCtrl* ctrl)
   {
      return ctrl->getResizeCell();
   }
   
   DLL_PUBLIC void GuiTextListCtrlSetResizeCell(GuiTextListCtrl* ctrl, bool resize)
   {
      ctrl->setResizeCell(resize);
   }

   // C-TODO Handle variable sized FieldVectors
   
   DLL_PUBLIC bool GuiTextListCtrlGetFitParentWidth(GuiTextListCtrl* ctrl)
   {
      return ctrl->getFitParentWidth();
   }
   
   DLL_PUBLIC void GuiTextListCtrlSetFitParentWidth(GuiTextListCtrl* ctrl, bool fitParentWidth)
   {
      ctrl->setFitParentWidth(fitParentWidth);
   }
   
   DLL_PUBLIC bool GuiTextListCtrlGetClipColumnText(GuiTextListCtrl* ctrl)
   {
      return ctrl->getClipColumnText();
   }
   
   DLL_PUBLIC void GuiTextListCtrlSetClipColumnText(GuiTextListCtrl* ctrl, bool clip)
   {
      ctrl->setClipColumnText(clip);
   }
   
   DLL_PUBLIC U32 GuiTextListCtrlGetSelectedId(GuiTextListCtrl* ctrl)
   {
      return ctrl->getSelectedId();
   }
   
   DLL_PUBLIC void GuiTextListCtrlSetSelectedById(GuiTextListCtrl* ctrl, U32 ID)
   {
      S32 index = ctrl->findEntryById(ID);
      if (index >= 0)
         ctrl->setSelectedCell(Point2I(0, index));
   }
   
   DLL_PUBLIC void GuiTextListCtrlSetSelectedRow(GuiTextListCtrl* ctrl, U32 rowNum)
   {
      ctrl->setSelectedCell(Point2I(0, rowNum));
   }
   
   DLL_PUBLIC U32 GuiTextListCtrlGetSelectedRow(GuiTextListCtrl* ctrl)
   {
      return ctrl->getSelectedRow();
   }
   
   DLL_PUBLIC void GuiTextListCtrlClearSelection(GuiTextListCtrl* ctrl)
   {
      return ctrl->setSelectedCell(Point2I(-1,-1));
   }
   
   //CTODO optional param
   DLL_PUBLIC S32 GuiTextListCtrlAddRow(GuiTextListCtrl* ctrl, U32 ID, const char* text, S32 row)
   {
      S32 ret = ctrl->mList.size();
      if (row < 0)
         ctrl->addEntry(ID, text);
      else
         ctrl->insertEntry(ID, text, row);
      return ret;
   }

   DLL_PUBLIC void GuiTextListCtrlSetRowById(GuiTextListCtrl* ctrl, U32 ID, const char* text)
   {
      ctrl->setEntry(ID, text);
   }

   //CTODO optional param
   DLL_PUBLIC void GuiTextListCtrlSort(GuiTextListCtrl* ctrl, U32 columnID, bool ascending)
   {
      ctrl->sort(columnID, ascending);
   }

   DLL_PUBLIC void GuiTextListCtrlSortNumerical(GuiTextListCtrl* ctrl, U32 columnID, bool ascending)
   {
      ctrl->sortNumerical(columnID, ascending);
   }

   DLL_PUBLIC void GuiTextListCtrlClear(GuiTextListCtrl* ctrl)
   {
      ctrl->clear();
   }

   DLL_PUBLIC U32 GuiTextListCtrlRowCount(GuiTextListCtrl* ctrl)
   {
      return ctrl->getNumEntries();
   }

   DLL_PUBLIC U32 GuiTextListCtrlGetRowId(GuiTextListCtrl* ctrl, U32 row)
   {
      if (row >= ctrl->getNumEntries())
         return -1;
      return ctrl->mList[row].id;
   }

   DLL_PUBLIC const char* GuiTextListCtrlGetRowTextById(GuiTextListCtrl* ctrl, U32 ID)
   {
      S32 index = ctrl->findEntryById(ID);
      if (index < 0)
         return NULL;
      return CInterface::GetMarshallableString(ctrl->mList[index].text);
   }

   DLL_PUBLIC S32 GuiTextListCtrlGetRowNumById(GuiTextListCtrl* ctrl, U32 ID)
   {
      return ctrl->findEntryById(ID);
   }

   DLL_PUBLIC const char* GuiTextListCtrlGetRowText(GuiTextListCtrl* ctrl, U32 row)
   {
      if (row >= ctrl->mList.size())
         return NULL;
      return CInterface::GetMarshallableString(ctrl->mList[row].text);
   }

   DLL_PUBLIC void GuiTextListCtrlRemoveRowById(GuiTextListCtrl* ctrl, U32 ID)
   {
      ctrl->removeEntry(ID);
   }

   DLL_PUBLIC void GuiTextListCtrlRemoveRow(GuiTextListCtrl* ctrl, S32 row)
   {
      ctrl->removeEntryByIndex(row);
   }

   DLL_PUBLIC void GuiTextListCtrlScrollVisible(GuiTextListCtrl* ctrl, U32 row)
   {
      ctrl->scrollCellVisible(Point2I(0,row));
   }

   DLL_PUBLIC S32 GuiTextListCtrlFindTextIndex(GuiTextListCtrl* ctrl, const char* text)
   {
      return ctrl->findEntryByText(text);
   }

   DLL_PUBLIC void GuiTextListCtrlSetRowActive(GuiTextListCtrl* ctrl, U32 row, bool active)
   {
      ctrl->setEntryActive(row, active);
   }

   DLL_PUBLIC bool GuiTextListCtrlIsRowActive(GuiTextListCtrl* ctrl, U32 row)
   {
      return ctrl->isEntryActive(row);
   }
}