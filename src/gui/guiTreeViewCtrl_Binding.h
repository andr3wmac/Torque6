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

//------------------------------------------------------------------------------
ConsoleMethod(GuiTreeViewCtrl, findItemByName, S32, 3, 3, "(name) Find item by name\n"
   "@param name The name of the desired object.\n"
   "@return Returns the ID of the object, or -1 on failure (not found).")
{
   return(object->findItemByName(argv[2]));
}

ConsoleMethod(GuiTreeViewCtrl, insertItem, S32, 4, 8, "(TreeItemId parent, name, value, icon, normalImage=0, expandedImage=0) Adds item to tree control.\n"
   "@param parent The new item's parent.\n"
   "@param name The name of the new item.\n"
   "@param value The new item's value.\n"
   "@param icon The new item's icon\n"
   "@return Returns the new item's ID.")
{
   S32 norm = 0, expand = 0;

   if (argc > 6)
   {
      norm = dAtoi(argv[6]);
      if (argc > 7)
         expand = dAtoi(argv[7]);
   }

   return(object->insertItem(dAtoi(argv[2]), argv[3], argv[4], argv[5], norm, expand));
}

ConsoleMethod(GuiTreeViewCtrl, lockSelection, void, 2, 3, "([bool lock]) Set whether the selection is to be locked."
   "@param lock Boolean flag for whether or not the current selected object should be locked\n"
   "@return No return value.")
{
   bool lock = true;
   if (argc == 3)
      lock = dAtob(argv[2]);
   object->lockSelection(lock);
}

ConsoleMethod(GuiTreeViewCtrl, clearSelection, void, 2, 2, "() Clear selection\n"
   "@return No return value.")
{
   object->clearSelection();
}

ConsoleMethod(GuiTreeViewCtrl, deleteSelection, void, 2, 2, "() Delete all selected items.\n"
   "@return No return value.\n")
{
   object->deleteSelection();
}

ConsoleMethod(GuiTreeViewCtrl, addSelection, void, 3, 3, "(string ID) Select an item"
   "@param ID The ID of the item to select.\n"
   "@return No return value.")
{
   S32 id = dAtoi(argv[2]);
   object->addSelection(id);
}

ConsoleMethod(GuiTreeViewCtrl, addChildSelectionByValue, void, 4, 4, "(TreeItemId parent, value)")
{
   S32 id = dAtoi(argv[2]);
   GuiTreeViewCtrl::Item* parentItem = object->getItem(id);
   if (parentItem)
   {
      GuiTreeViewCtrl::Item* child = parentItem->findChildByValue(argv[3]);
      if (child)
         object->addSelection(child->getID());
   }
}

ConsoleMethod(GuiTreeViewCtrl, removeSelection, void, 3, 3, "(string ID) Deselects given item.\n"
   "@param ID The ID of the item to deselect.\n"
   "@return No return value.")
{
   S32 id = dAtoi(argv[2]);
   object->removeSelection(id);
}

ConsoleMethod(GuiTreeViewCtrl, removeChildSelectionByValue, void, 4, 4, "removeChildSelectionByValue(TreeItemId parent, value)")
{
   S32 id = dAtoi(argv[2]);
   GuiTreeViewCtrl::Item* parentItem = object->getItem(id);
   if (parentItem)
   {
      GuiTreeViewCtrl::Item* child = parentItem->findChildByValue(argv[3]);
      if (child)
         object->removeSelection(child->getID());
   }
}

ConsoleMethod(GuiTreeViewCtrl, selectItem, bool, 3, 4, "(TreeItemId item, [bool select=true]) Selects item.")
{
   S32 id = dAtoi(argv[2]);
   bool select = true;
   if (argc == 4)
      select = dAtob(argv[3]);

   return(object->setItemSelected(id, select));
}

ConsoleMethod(GuiTreeViewCtrl, expandItem, bool, 3, 4, "(TreeItemId item, [bool expand=true]) Deselects item")
{
   S32 id = dAtoi(argv[2]);
   bool expand = true;
   if (argc == 4)
      expand = dAtob(argv[3]);
   return(object->setItemExpanded(id, expand));
}

// Make the given item visible.
ConsoleMethod(GuiTreeViewCtrl, scrollVisible, void, 3, 3, "(TreeItemId item) Make the given item visible.\n"
   "@param ID of the desired item.\n"
   "@return No return value.")
{
   object->scrollVisible(dAtoi(argv[2]));
}


ConsoleMethod(GuiTreeViewCtrl, buildIconTable, bool, 3, 3, "(string icons) Icons should be designated by the bitmap/png file names (minus the file extensions) "
   "and separated by colons (:). This list should be synchronized with the Icons enum.\n"
   "@param icons The list of icons to add sepated by colons.\n"
   "@return Returns true on success, false otherwise.")
{
   const char * icons = argv[2];
   return object->buildIconTable(icons);
}

ConsoleMethod(GuiTreeViewCtrl, open, void, 3, 4, "(SimSet obj, bool okToEdit=true) Set the root of the tree view to the specified object, or to the root set.")
{
   SimSet *treeRoot = NULL;
   SimObject* target = Sim::findObject(argv[2]);

   bool okToEdit = true;

   if (argc == 4)
      okToEdit = dAtob(argv[3]);

   if (target)
      treeRoot = dynamic_cast<SimSet*>(target);

   if (!treeRoot)
      Sim::findObject(RootGroupId, treeRoot);

   object->inspectObject(treeRoot, okToEdit);
}

ConsoleMethod(GuiTreeViewCtrl, getItemText, const char *, 3, 3, "(TreeItemId item)")
{
   return(object->getItemText(dAtoi(argv[2])));
}

ConsoleMethod(GuiTreeViewCtrl, getItemValue, const char *, 3, 3, "(TreeItemId item)")
{
   return(object->getItemValue(dAtoi(argv[2])));
}

ConsoleMethod(GuiTreeViewCtrl, editItem, bool, 5, 5, "(TreeItemId item, string newText, string newValue)")
{
   return(object->editItem(dAtoi(argv[2]), argv[3], argv[4]));
}

ConsoleMethod(GuiTreeViewCtrl, removeItem, bool, 3, 3, "(TreeItemId item)")
{
   return(object->removeItem(dAtoi(argv[2])));
}

ConsoleMethod(GuiTreeViewCtrl, removeAllChildren, void, 3, 3, "removeAllChildren(TreeItemId parent)")
{
   object->removeAllChildren(dAtoi(argv[2]));
}

ConsoleMethod(GuiTreeViewCtrl, clear, void, 2, 2, "() - empty tree")
{
   object->removeItem(0);
}

ConsoleMethod(GuiTreeViewCtrl, getFirstRootItem, S32, 2, 2, "Get id for root item.")
{
   return(object->getFirstRootItem());
}

ConsoleMethod(GuiTreeViewCtrl, getChild, S32, 3, 3, "(TreeItemId item)")
{
   return(object->getChildItem(dAtoi(argv[2])));
}

ConsoleMethod(GuiTreeViewCtrl, buildVisibleTree, void, 3, 3, "Build the visible tree")
{
   object->buildVisibleTree(dAtob(argv[2]));
}

ConsoleMethod(GuiTreeViewCtrl, getParent, S32, 3, 3, "(TreeItemId item)")
{
   return(object->getParentItem(dAtoi(argv[2])));
}

ConsoleMethod(GuiTreeViewCtrl, getNextSibling, S32, 3, 3, "(TreeItemId item)")
{
   return(object->getNextSiblingItem(dAtoi(argv[2])));
}

ConsoleMethod(GuiTreeViewCtrl, getPrevSibling, S32, 3, 3, "(TreeItemId item)")
{
   return(object->getPrevSiblingItem(dAtoi(argv[2])));
}

ConsoleMethod(GuiTreeViewCtrl, getItemCount, S32, 2, 2, "() @return Returns the number of items in control")
{
   return(object->getItemCount());
}

ConsoleMethod(GuiTreeViewCtrl, getSelectedItem, S32, 2, 2, "() @return Returns the ID of the selected item.")
{
   return (object->getSelectedItem());
}

ConsoleMethod(GuiTreeViewCtrl, getSelectedObject, S32, 2, 2, "() @return Returns the currently selected simObject in inspector mode or -1")
{
   GuiTreeViewCtrl::Item *item = object->getItem(object->getSelectedItem());
   if (item != NULL && item->isInspectorData())
   {
      SimObject *obj = item->getObject();
      if (obj != NULL)
         return object->getId();
   }

   return -1;
}

ConsoleMethod(GuiTreeViewCtrl, moveItemUp, void, 3, 3, "(TreeItemId item)")
{
   object->moveItemUp(dAtoi(argv[2]));
}

ConsoleMethod(GuiTreeViewCtrl, getSelectedItemsCount, S32, 2, 2, "")
{
   return (object->getSelectedItemsCount());
}

ConsoleMethod(GuiTreeViewCtrl, moveItemDown, void, 3, 3, "(TreeItemId item)")
{
   object->moveItemDown(dAtoi(argv[2]));
}

//-----------------------------------------------------------------------------

ConsoleMethod(GuiTreeViewCtrl, getTextToRoot, const char*, 4, 4, "(TreeItemId item,Delimiter=none) gets the text from the current node to the root, concatenating at each branch upward, with a specified delimiter optionally")
{
   if (argc < 4)
   {
      Con::warnf("GuiTreeViewCtrl::getTextToRoot - Invalid number of arguments!");
      return ("");
   }
   S32 itemId = dAtoi(argv[2]);
   StringTableEntry delimiter = argv[3];

   return object->getTextToRoot(itemId, delimiter);
}

ConsoleMethod(GuiTreeViewCtrl, getSelectedItemList, const char*, 2, 2, "returns a space seperated list of mulitple item ids")
{
   char* buff = Con::getReturnBuffer(1024);
   dSprintf(buff, 1024, "");

   for (int i = 0; i < object->mSelected.size(); i++)
   {
      S32 id = object->mSelected[i];
      //get the current length of the buffer
      U32	len = dStrlen(buff);
      //the start of the buffer where we want to write
      char* buffPart = buff + len;
      //the size of the remaining buffer (-1 cause dStrlen doesn't count the \0)
      S32 size = 1024 - len - 1;
      //write it:
      if (size < 1)
      {
         Con::errorf("GuiTreeViewCtrl::getSelectedItemList - Not enough room to return our object list");
         return buff;
      }

      dSprintf(buffPart, size, "%d ", id);
   }
   //mSelected

   return buff;
}

//------------------------------------------------------------------------------
ConsoleMethod(GuiTreeViewCtrl, findItemByObjectId, S32, 3, 3, "(find item by object id and returns the mId)")
{
   return(object->findItemByObjectId(dAtoi(argv[2])));
}

//------------------------------------------------------------------------------
ConsoleMethod(GuiTreeViewCtrl, scrollVisibleByObjectId, S32, 3, 3, "(show item by object id. returns true if sucessful.)")
{
   return(object->scrollVisibleByObjectId(dAtoi(argv[2])));
}

extern "C"{
   DLL_PUBLIC GuiTreeViewCtrl* GuiTreeViewCtrlCreateInstance()
   {
      return new GuiTreeViewCtrl();
   }

   DLL_PUBLIC S32 GuiTreeViewCtrlGetTabSize(GuiTreeViewCtrl* ctrl)
   {
      return ctrl->getTabSize();
   }

   DLL_PUBLIC void GuiTreeViewCtrlSetTabSize(GuiTreeViewCtrl* ctrl, S32 size)
   {
      ctrl->setTabSize(size);
   }

   DLL_PUBLIC S32 GuiTreeViewCtrlGetTextOffset(GuiTreeViewCtrl* ctrl)
   {
      return ctrl->getTextOffset();
   }

   DLL_PUBLIC void GuiTreeViewCtrlSetTextOffset(GuiTreeViewCtrl* ctrl, S32 offset)
   {
      ctrl->setTextOffset(offset);
   }

   DLL_PUBLIC bool GuiTreeViewCtrlGetFullRowSelect(GuiTreeViewCtrl* ctrl)
   {
      return ctrl->getFullRowSelect();
   }

   DLL_PUBLIC void GuiTreeViewCtrlSetFullRowSelect(GuiTreeViewCtrl* ctrl, bool fullRowSelect)
   {
      ctrl->setFullRowSelect(fullRowSelect);
   }

   DLL_PUBLIC S32 GuiTreeViewCtrlGetItemHeight(GuiTreeViewCtrl* ctrl)
   {
      return ctrl->getItemHeight();
   }

   DLL_PUBLIC void GuiTreeViewCtrlSetItemHeight(GuiTreeViewCtrl* ctrl, S32 height)
   {
      ctrl->setItemHeight(height);
   }

   DLL_PUBLIC bool GuiTreeViewCtrlGetDestroyTreeOnSleep(GuiTreeViewCtrl* ctrl)
   {
      return ctrl->getDestroyOnSleep();
   }

   DLL_PUBLIC void GuiTreeViewCtrlSetDestroyTreeOnSleep(GuiTreeViewCtrl* ctrl, bool destroyOnSleep)
   {
      ctrl->setDestroyOnSleep(destroyOnSleep);
   }

   DLL_PUBLIC bool GuiTreeViewCtrlGetMouseDragging(GuiTreeViewCtrl* ctrl)
   {
      return ctrl->getSupportMouseDragging();
   }

   DLL_PUBLIC void GuiTreeViewCtrlSetMouseDragging(GuiTreeViewCtrl* ctrl, bool allowDragging)
   {
      ctrl->setSupportMouseDragging(allowDragging);
   }

   DLL_PUBLIC bool GuiTreeViewCtrlGetMultipleSelections(GuiTreeViewCtrl* ctrl)
   {
      return ctrl->getMultipleSelections();
   }

   DLL_PUBLIC void GuiTreeViewCtrlSetMultipleSelections(GuiTreeViewCtrl* ctrl, bool multipleSelections)
   {
      ctrl->setMultipleSelections(multipleSelections);
   }

   DLL_PUBLIC bool GuiTreeViewCtrlGetDeleteObjectAllowed(GuiTreeViewCtrl* ctrl)
   {
      return ctrl->getDeleteObjectAllowed();
   }

   DLL_PUBLIC void GuiTreeViewCtrlSetDeleteObjectAllowed(GuiTreeViewCtrl* ctrl, bool allowDelete)
   {
      ctrl->setDeleteObjectAllowed(allowDelete);
   }

   DLL_PUBLIC bool GuiTreeViewCtrlGetDragToItemAllowed(GuiTreeViewCtrl* ctrl)
   {
      return ctrl->getDragToItemAllowed();
   }

   DLL_PUBLIC void GuiTreeViewCtrlSetDragToItemAllowed(GuiTreeViewCtrl* ctrl, bool allowDrag)
   {
      ctrl->setDragToItemAllowed(allowDrag);
   }

   DLL_PUBLIC S32 GuiTreeViewCtrlFindItemByName(GuiTreeViewCtrl* ctrl, const char* name)
   {
      return ctrl->findItemByName(name);
   }

   //CTODO default values
   DLL_PUBLIC S32 GuiTreeViewCtrlInsertItem(GuiTreeViewCtrl* ctrl, S32 parent, const char* name, const char* value, const char* icon, S32 norm, S32 expand)
   {
      return ctrl->insertItem(parent, name, value, icon, norm, expand);
   }

   DLL_PUBLIC void GuiTreeViewCtrlLockSelection(GuiTreeViewCtrl* ctrl, bool locked)
   {
      ctrl->lockSelection(locked);
   }

   DLL_PUBLIC void GuiTreeViewCtrlClearSelection(GuiTreeViewCtrl* ctrl)
   {
      ctrl->clearSelection();
   }

   DLL_PUBLIC void GuiTreeViewCtrlDeleteSelection(GuiTreeViewCtrl* ctrl)
   {
      ctrl->deleteSelection();
   }

   DLL_PUBLIC void GuiTreeViewCtrlAddSelection(GuiTreeViewCtrl* ctrl, S32 ID)
   {
      ctrl->addSelection(ID);
   }

   DLL_PUBLIC void GuiTreeViewCtrlAddChildSelectionByValue(GuiTreeViewCtrl* ctrl, S32 parent, const char* value)
   {
      GuiTreeViewCtrl::Item* parentItem = ctrl->getItem(parent);
      if (parentItem)
      {
         GuiTreeViewCtrl::Item* child = parentItem->findChildByValue(value);
         if (child)
            ctrl->addSelection(child->getID());
      }
   }

   DLL_PUBLIC void GuiTreeViewCtrlRemoveSelection(GuiTreeViewCtrl* ctrl, S32 ID)
   {
      ctrl->removeSelection(ID);
   }

   DLL_PUBLIC void GuiTreeViewCtrlRemoveChildSelectionByValue(GuiTreeViewCtrl* ctrl, S32 parent, const char* value)
   {
      GuiTreeViewCtrl::Item* parentItem = ctrl->getItem(parent);
      if (parentItem)
      {
         GuiTreeViewCtrl::Item* child = parentItem->findChildByValue(value);
         if (child)
            ctrl->removeSelection(child->getID());
      }
   }

   //CTODO default vlaue
   DLL_PUBLIC bool GuiTreeViewCtrlSelectItem(GuiTreeViewCtrl* ctrl, S32 item, bool select)
   {
      return ctrl->setItemSelected(item, select);
   }

   //CTODO default value
   DLL_PUBLIC bool GuiTreeViewCtrlExpandItem(GuiTreeViewCtrl* ctrl, S32 item, bool expand)
   {
      return ctrl->setItemExpanded(item, expand);
   }

   DLL_PUBLIC void GuiTreeViewCtrlScrollVisible(GuiTreeViewCtrl* ctrl, S32 item)
   {
      ctrl->scrollVisible(item);
   }

   DLL_PUBLIC bool GuiTreeViewCtrlBuildIconTable(GuiTreeViewCtrl* ctrl, const char* icons)
   {
      return ctrl->buildIconTable(icons);
   }

   //CTODO default value
   DLL_PUBLIC void GuiTreeViewCtrlOpen(GuiTreeViewCtrl* ctrl, SimSet* target, bool okToEdit)
   {
      if (!target)
         Sim::findObject(RootGroupId, target);
      ctrl->inspectObject(target, okToEdit);
   }

   DLL_PUBLIC const char* GuiTreeViewCtrlGetItemText(GuiTreeViewCtrl* ctrl, S32 item)
   {
      return CInterface::GetMarshallableString(ctrl->getItemText(item));
   }

   DLL_PUBLIC const char* GuiTreeViewCtrlGetItemValue(GuiTreeViewCtrl* ctrl, S32 item)
   {
      return CInterface::GetMarshallableString(ctrl->getItemValue(item));
   }

   DLL_PUBLIC bool GuiTreeViewCtrlEditItem(GuiTreeViewCtrl* ctrl, S32 item, const char* newText, const char* newValue)
   {
      return ctrl->editItem(item, newText, newValue);
   }

   DLL_PUBLIC bool GuiTreeViewCtrlRemoveItem(GuiTreeViewCtrl* ctrl, S32 item)
   {
      return ctrl->removeItem(item);
   }

   DLL_PUBLIC void GuiTreeViewCtrlRemoveAllChildren(GuiTreeViewCtrl* ctrl, S32 parent)
   {
      ctrl->removeAllChildren(parent);
   }

   DLL_PUBLIC void GuiTreeViewCtrlClear(GuiTreeViewCtrl* ctrl)
   {
      ctrl->removeItem(0);
   }

   DLL_PUBLIC S32 GuiTreeViewCtrlGetFirstRootItem(GuiTreeViewCtrl* ctrl)
   {
      return ctrl->getFirstRootItem();
   }

   DLL_PUBLIC S32 GuiTreeViewCtrlGetChild(GuiTreeViewCtrl* ctrl, S32 item)
   {
      return ctrl->getChildItem(item);
   }

   //CTODO default value
   DLL_PUBLIC void GuiTreeViewCtrlBuildVisibleTree(GuiTreeViewCtrl* ctrl, bool forceFullUpdate)
   {
      ctrl->buildVisibleTree(forceFullUpdate);
   }

   DLL_PUBLIC S32 GuiTreeViewCtrlGetParent(GuiTreeViewCtrl* ctrl, S32 item)
   {
      return ctrl->getParentItem(item);
   }

   DLL_PUBLIC S32 GuiTreeViewCtrlGetNextSibling(GuiTreeViewCtrl* ctrl, S32 item)
   {
      return ctrl->getNextSiblingItem(item);
   }

   DLL_PUBLIC S32 GuiTreeViewCtrlGetPrevSibling(GuiTreeViewCtrl* ctrl, S32 item)
   {
      return ctrl->getPrevSiblingItem(item);
   }

   DLL_PUBLIC S32 GuiTreeViewCtrlGetItemCount(GuiTreeViewCtrl* ctrl)
   {
      return ctrl->getItemCount();
   }

   DLL_PUBLIC S32 GuiTreeViewCtrlGetSelectedItem(GuiTreeViewCtrl* ctrl)
   {
      return ctrl->getSelectedItem();
   }

   DLL_PUBLIC SimObject* GuiTreeViewCtrlGetSelectedObject(GuiTreeViewCtrl* ctrl)
   {
      GuiTreeViewCtrl::Item *item = ctrl->getItem(ctrl->getSelectedItem());
      if (item != NULL && item->isInspectorData())
      {
         SimObject *obj = item->getObject();
         if (obj != NULL)
            return obj;
      }

      return NULL;
   }

   DLL_PUBLIC void GuiTreeViewCtrlMoveItemUp(GuiTreeViewCtrl* ctrl, S32 item)
   {
      ctrl->moveItemUp(item);
   }

   DLL_PUBLIC void GuiTreeViewCtrlMoveItemDown(GuiTreeViewCtrl* ctrl, S32 item)
   {
      ctrl->moveItemDown(item);
   }

   DLL_PUBLIC S32 GuiTreeViewCtrlGetSelectedItemsCount(GuiTreeViewCtrl* ctrl)
   {
      return ctrl->getSelectedItemsCount();
   }

   //CTODO default value
   DLL_PUBLIC const char* GuiTreeViewCtrlGetTextToRoot(GuiTreeViewCtrl* ctrl, S32 item, const char* delimiter)
   {
      return CInterface::GetMarshallableString(ctrl->getTextToRoot(item, delimiter));
   }

   //CTODO list
   DLL_PUBLIC const char* GuiTreeViewCtrlGetSelectedItemList(GuiTreeViewCtrl* ctrl)
   {
      char* buff = Con::getReturnBuffer(1024);
      dSprintf(buff, 1024, "");

      for (int i = 0; i < ctrl->mSelected.size(); i++)
      {
         S32 id = ctrl->mSelected[i];
         //get the current length of the buffer
         U32	len = dStrlen(buff);
         //the start of the buffer where we want to write
         char* buffPart = buff + len;
         //the size of the remaining buffer (-1 cause dStrlen doesn't count the \0)
         S32 size = 1024 - len - 1;
         //write it:
         if (size < 1)
         {
            Con::errorf("GuiTreeViewCtrl::getSelectedItemList - Not enough room to return our object list");
            return buff;
         }

         dSprintf(buffPart, size, "%d ", id);
      }

      return CInterface::GetMarshallableString(buff);
   }

   DLL_PUBLIC S32 GuiTreeViewCtrlFindItemByObject(GuiTreeViewCtrl* ctrl, SimObject* obj)
   {
      return ctrl->findItemByObjectId(obj->getId());
   }

   DLL_PUBLIC S32 GuiTreeViewCtrlScrollVisibleByObject(GuiTreeViewCtrl* ctrl, SimObject* obj)
   {
      return ctrl->scrollVisibleByObjectId(obj->getId());
   }
}