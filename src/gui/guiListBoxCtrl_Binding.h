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

ConsoleMethod(GuiListBoxCtrl, setMultipleSelection, void, 3, 3, "(bool setMS) Set the MultipleSelection field.\n"
   "@return No return value.\n"
   "@note %listBox.setMultipleSelection([true/false])")
{
   object->setMultipleSelection(dAtob(argv[2]));
}

ConsoleMethod(GuiListBoxCtrl, clearItems, void, 2, 2, "() Clears all the items in the listbox\n"
   "@return No return value.")
{
   object->clearItems();
}

ConsoleMethod(GuiListBoxCtrl, clearSelection, void, 2, 2, "() Sets all currently selected items to unselected\n"
   "@return No return value.")
{
   object->clearSelection();
}

ConsoleMethod(GuiListBoxCtrl, setSelected, void, 3, 4, "(S32 index, bool setting) Sets the item at the index specified to selected or not"
   "@param index The index of the item you wish to modify.\n"
   "@param setting A boolean value. True sets it as selected; false, not selected.\n"
   "@return No return value.")
{
   bool value = true;
   if (argc == 4)
      value = dAtob(argv[3]);

   if (value == true)
      object->addSelection(dAtoi(argv[2]));
   else
      object->removeSelection(dAtoi(argv[2]));
}

ConsoleMethod(GuiListBoxCtrl, getItemCount, S32, 2, 2, "()\n @return Returns the number of items in the list")
{
   return object->getItemCount();
}

ConsoleMethod(GuiListBoxCtrl, getSelCount, S32, 2, 2, "()\n @return Returns the number of items currently selected")
{
   return object->getSelCount();
}

ConsoleMethod(GuiListBoxCtrl, getSelectedItem, S32, 2, 2, "()\n @return Returns the selected items index or -1 if none. "
   "If multiple selections exist it returns the first selected item")
{
   return object->getSelectedItem();
}

ConsoleMethod(GuiListBoxCtrl, getSelectedItems, const char*, 2, 2, "()\n @return Returns a space delimited list "
   "of the selected items indexes in the list")
{
   S32 selCount = object->getSelCount();
   if (selCount == -1 || selCount == 0)
      return StringTable->lookup("-1");
   else if (selCount == 1)
      return Con::getIntArg(object->getSelectedItem());

   Vector<S32> selItems;
   object->getSelectedItems(selItems);

   if (selItems.empty())
      return StringTable->lookup("-1");

   UTF8 *retBuffer = Con::getReturnBuffer(selItems.size() * 4);
   dMemset(retBuffer, 0, selItems.size() * 4);
   Vector<S32>::iterator i = selItems.begin();
   for (; i != selItems.end(); i++)
   {
      UTF8 retFormat[12];
      dSprintf(retFormat, 12, "%d ", (*i));
      dStrcat(retBuffer, retFormat);
   }

   return retBuffer;
}

ConsoleMethod(GuiListBoxCtrl, findItemText, S32, 3, 4, " (string itemText, bool caseSensitive) Find the item with the given text.\n"
   "@param itemText The text to search for.\n"
   "@param caseSensitive Sets whether or not to ignore text in the search.\n"
   "@return Returns index of item with matching text or -1 if none"
   "@note %listBox.findItemText( myItemText, [?caseSensitive - false] )")
{
   bool bCaseSensitive = false;

   if (argc == 4)
      bCaseSensitive = dAtob(argv[3]);

   return object->findItemText(argv[2], bCaseSensitive);
}

ConsoleMethod(GuiListBoxCtrl, setCurSel, void, 3, 3, "(index) Sets the currently selected item at the specified index\n"
   "@return No return value.")
{
   object->setCurSel(dAtoi(argv[2]));
}

ConsoleMethod(GuiListBoxCtrl, setCurSelRange, void, 3, 4, "(start,[stop]) Sets the current selection range from"
   "index start to stop.\n"
   "@param start The start of the selection range.\n"
   "@param stop The stopping point of the selection range. If no stop is specified it sets from start index to the end of the list\n"
   "@return No return value.")
{
   if (argc == 4)
      object->setCurSelRange(dAtoi(argv[2]), dAtoi(argv[3]));
   else
      object->setCurSelRange(dAtoi(argv[2]), 999999);
}

ConsoleMethod(GuiListBoxCtrl, addItem, void, 3, 4, "(text, [color]) Adds an item to the end of the list with an optional color\n"
   "@param text The object text.\n"
   "@param color Optional color setting.\n"
   "@return No return value.")
{
   if (argc == 3)
   {
      object->addItem(argv[2]);
   }
   else if (argc == 4)
   {
      U32 elementCount = GuiListBoxCtrl::getStringElementCount(argv[3]);

      if (elementCount == 3)
      {
         F32 red, green, blue;

         red = dAtof(GuiListBoxCtrl::getStringElement(argv[3], 0));
         green = dAtof(GuiListBoxCtrl::getStringElement(argv[3], 1));
         blue = dAtof(GuiListBoxCtrl::getStringElement(argv[3], 2));

         object->addItemWithColor(argv[2], ColorF(red, green, blue));
      }
      else
      {
         Con::warnf("GuiListBoxCtrl::addItem() - Invalid number of parameters for the color!");
      }

   }
   else
   {
      Con::warnf("GuiListBoxCtrl::addItem() - Invalid number of parameters!");
   }
}

ConsoleMethod(GuiListBoxCtrl, setItemColor, void, 4, 4, "(index, color) Sets the color of the item at given index.\n"
   "@param index The index of the item you wish to modify.\n"
   "@param color The color you wish to set the object to.\n"
   "@return No return value.")
{
   U32 elementCount = GuiListBoxCtrl::getStringElementCount(argv[3]);

   if (elementCount == 3)
   {
      F32 red = dAtof(GuiListBoxCtrl::getStringElement(argv[3], 0));
      F32 green = dAtof(GuiListBoxCtrl::getStringElement(argv[3], 1));
      F32 blue = dAtof(GuiListBoxCtrl::getStringElement(argv[3], 2));

      object->setItemColor(dAtoi(argv[2]), ColorF(red, green, blue));
   }
   else
      Con::warnf("GuiListBoxCtrl::addItem() - Invalid number of parameters for the color!");
}

ConsoleMethod(GuiListBoxCtrl, clearItemColor, void, 3, 3, "(index) Clears the color of the item at index.\n"
   "@param index The index of the item to modify.\n"
   "@return No return value.")
{
   object->clearItemColor(dAtoi(argv[2]));
}

ConsoleMethod(GuiListBoxCtrl, insertItem, void, 4, 4, "( text, index ) Inserts an item into the list at the specified index.\n"
   "@param text The desired object text.\n"
   "@param index The index to set the object at.\n"
   "@return Returns the index assigned or -1 on error")
{
   object->insertItem(dAtoi(argv[3]), argv[2]);
}

ConsoleMethod(GuiListBoxCtrl, deleteItem, void, 3, 3, "(index) Deletes the item at the given index.\n"
   "@param index The index of the item to delete.\n"
   "@return No return value.")
{
   object->deleteItem(dAtoi(argv[2]));
}

ConsoleMethod(GuiListBoxCtrl, setItemText, void, 4, 4, "(index, newtext) Sets the item's text at the specified index\n"
   "@param index The index of the item to modify.\n"
   "@param newtext The new text to set to the object.\n"
   "@return No return value.")
{
   object->setItemText(dAtoi(argv[2]), argv[3]);
}


ConsoleMethod(GuiListBoxCtrl, getItemText, const char*, 3, 3, "(index) \n @return Returns the text of the item at the specified index")
{
   return object->getItemText(dAtoi(argv[2]));
}

extern "C"{
   DLL_PUBLIC GuiListBoxCtrl* GuiListBoxCtrlCreateInstance()
   {
      return new GuiListBoxCtrl();
   }

   DLL_PUBLIC bool GuiListBoxCtrlGetAllowMultipleSelections(GuiListBoxCtrl* ctrl)
   {
      return ctrl->mMultipleSelections;
   }

   DLL_PUBLIC void GuiListBoxCtrlSetAllowMultipleSelections(GuiListBoxCtrl* ctrl, bool allow)
   {
      ctrl->setMultipleSelection(allow);
   }

   DLL_PUBLIC bool GuiListBoxCtrlGetFitParentWidth(GuiListBoxCtrl* ctrl)
   {
      return ctrl->mFitParentWidth;
   }

   DLL_PUBLIC void GuiListBoxCtrlSetFitParentWidth(GuiListBoxCtrl* ctrl, bool fit)
   {
      ctrl->mFitParentWidth = fit;
   }

   DLL_PUBLIC void GuiListBoxCtrlSetMultipleSelection(GuiListBoxCtrl* ctrl, bool enable)
   {
	   ctrl->setMultipleSelection(enable);
   }

   DLL_PUBLIC void GuiListBoxCtrlClearItems(GuiListBoxCtrl* ctrl)
   {
	   ctrl->clearItems();
   }

   DLL_PUBLIC void GuiListBoxCtrlClearSelection(GuiListBoxCtrl* ctrl)
   {
	   ctrl->clearSelection();
   }

   DLL_PUBLIC void GuiListBoxCtrlSetSelected(GuiListBoxCtrl* ctrl, S32 index, bool setting)
   {
	   if (setting)
		   ctrl->addSelection(index);
	   else
		   ctrl->removeSelection(index);
   }

   DLL_PUBLIC S32 GuiListBoxCtrlGetItemCount(GuiListBoxCtrl* ctrl)
   {
	   return ctrl->getItemCount();
   }

   DLL_PUBLIC S32 GuiListBoxCtrlGetSelCount(GuiListBoxCtrl* ctrl)
   {
	   return ctrl->getSelCount();
   }

   DLL_PUBLIC S32 GuiListBoxCtrlGetSelectedItem(GuiListBoxCtrl* ctrl)
   {
      return ctrl->getSelectedItem();
   }

   DLL_PUBLIC S32* GuiListBoxCtrlGetSelectedItems(GuiListBoxCtrl* ctrl)
   {
      Vector<S32> selItems;
      ctrl->getSelectedItems(selItems);
      return selItems.address();
      //ctodo confirm this works
   }

   DLL_PUBLIC S32 GuiListBoxCtrlFindItemText(GuiListBoxCtrl* ctrl, const char* itemText, bool caseSensitive)
   {
	   return ctrl->findItemText(itemText, caseSensitive);
   }

   DLL_PUBLIC void GuiListBoxCtrlSetCurSel(GuiListBoxCtrl* ctrl, S32 index)
   {
	   ctrl->setCurSel(index);
   }

   DLL_PUBLIC void GuiListBoxCtrlSetCurSelRange(GuiListBoxCtrl* ctrl, S32 start, S32 stop)
   {
	   ctrl->setCurSelRange(start, stop);
   }

   DLL_PUBLIC void GuiListBoxCtrlAddItem(GuiListBoxCtrl* ctrl, const char* text, CInterface::ColorParam color)
   {
	   ctrl->addItemWithColor(text, color);
   }

   DLL_PUBLIC void GuiListBoxCtrlSetItemColor(GuiListBoxCtrl* ctrl, S32 index, CInterface::ColorParam color)
   {
	   ctrl->setItemColor(index, color);
   }

   DLL_PUBLIC void GuiListBoxCtrlClearItemColor(GuiListBoxCtrl* ctrl, S32 index)
   {
	   ctrl->clearItemColor(index);
   }

   DLL_PUBLIC void GuiListBoxCtrlInsertItem(GuiListBoxCtrl* ctrl, const char* text, S32 index)
   {
	   ctrl->insertItem(index, text);
   }

   DLL_PUBLIC void GuiListBoxCtrlDeleteItem(GuiListBoxCtrl* ctrl, S32 index)
   {
	   ctrl->deleteItem(index);
   }

   DLL_PUBLIC void GuiListBoxCtrlSetItemtext(GuiListBoxCtrl* ctrl, S32 index, const char* newText)
   {
	   ctrl->setItemText(index, newText);
   }

   DLL_PUBLIC const char* GuiListBoxCtrlGetItemtext(GuiListBoxCtrl* ctrl, S32 index)
   {
	   return CInterface::GetMarshallableString(ctrl->getItemText(index));
   }
}