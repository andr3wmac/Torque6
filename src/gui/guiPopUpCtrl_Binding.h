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

ConsoleMethod(GuiPopUpMenuCtrl, add, void, 4, 5, "( entryText , entryID [ , entryScheme ] ) Use the add method to add a new entry with text entryText, ID entryID, and using the scheme entryScheme.\n"
   "@param entryText Text to display in menu entry.\n"
   "@param entryID ID to assign to entry. This value may be 1 or greater.\n"
   "@param entryScheme An integer value representing the ID of an optional color scheme to be used for this entry.\n"
   "@return No return value.\n"
   "@sa addScheme, clear")
{
   if (argc > 4)
      object->addEntry(argv[2], dAtoi(argv[3]), dAtoi(argv[4]));
   else
      object->addEntry(argv[2], dAtoi(argv[3]), 0);
}

ConsoleMethod(GuiPopUpMenuCtrl, addScheme, void, 6, 6, "( entryScheme , fontColor , fontColorHL , fontColorSEL ) Use the addScheme method to create a new color scheme or to modify an existing one.\n"
   "An integer color vector contains three integer values, each between 0 and 255 and is organized in this order: 'R G B'.\n"
   "@param entryScheme An integer value representing the ID of the scheme, between 1 and inf.\n"
   "@param fontColor A vector containing an integer representation of the menu entry's normal color.\n"
   "@param fontColorHL A vector containing an integer representation of the menu entry's highlighted color.\n"
   "@param fontColorSEL A vector containing an integer representation of the menu entry's selected color.\n"
   "@return No return value.\n"
   "@sa add")
{
   ColorI fontColor, fontColorHL, fontColorSEL;
   U32 r, g, b;
   char buf[64];

   dStrcpy(buf, argv[3]);
   char* temp = dStrtok(buf, " \0");
   r = temp ? dAtoi(temp) : 0;
   temp = dStrtok(NULL, " \0");
   g = temp ? dAtoi(temp) : 0;
   temp = dStrtok(NULL, " \0");
   b = temp ? dAtoi(temp) : 0;
   fontColor.set(r, g, b);

   dStrcpy(buf, argv[4]);
   temp = dStrtok(buf, " \0");
   r = temp ? dAtoi(temp) : 0;
   temp = dStrtok(NULL, " \0");
   g = temp ? dAtoi(temp) : 0;
   temp = dStrtok(NULL, " \0");
   b = temp ? dAtoi(temp) : 0;
   fontColorHL.set(r, g, b);

   dStrcpy(buf, argv[5]);
   temp = dStrtok(buf, " \0");
   r = temp ? dAtoi(temp) : 0;
   temp = dStrtok(NULL, " \0");
   g = temp ? dAtoi(temp) : 0;
   temp = dStrtok(NULL, " \0");
   b = temp ? dAtoi(temp) : 0;
   fontColorSEL.set(r, g, b);

   object->addScheme(dAtoi(argv[2]), fontColor, fontColorHL, fontColorSEL);
}

ConsoleMethod(GuiPopUpMenuCtrl, setText, void, 3, 3, "( text ) Use the setText method to change the text displayed in the menu bar.\n"
   "Pass the NULL string to clear the menu bar text.\n"
   "@param text New text to display in the menu bar.\n"
   "@return No return value.\n"
   "@sa getText, replaceText")
{
   object->setText(argv[2]);
}

ConsoleMethod(GuiPopUpMenuCtrl, getText, const char*, 2, 2, "() Use the getText method to get the text currently displayed in the menu bar.\n"
   "@return Returns the text in the menu bar or NULL if no text is present.\n"
   "@sa getSelected, setText")
{
   return object->getText();
}

ConsoleMethod(GuiPopUpMenuCtrl, clear, void, 2, 2, "() Use the clear method to remove all entries and schemes from the menu.\n"
   "@return No return value.\n"
   "@sa add, addScheme")
{
   object->clear();
}

ConsoleMethod(GuiPopUpMenuCtrl, sort, void, 2, 2, "() Use the sort method to sort the menu in ascending order.\n"
   "This is a lexicographic sort, so number (1,2,3,...) will come before letters (a,b,c,...)\n"
   "@return No return value.")
{
   object->sort();
}

// DAW: Added to sort the entries by ID
ConsoleMethod(GuiPopUpMenuCtrl, sortID, void, 2, 2, "() Sort the list by ID.\n"
   "@return No return value.")
{
   object->sortID();
}

ConsoleMethod(GuiPopUpMenuCtrl, forceOnAction, void, 2, 2, "() Use the forceOnAction method to force the onAction callback to be triggered.\n"
   "@return No return value.\n"
   "@sa forceClose, onAction (GUIControl callback)")
{
   object->onAction();
}

ConsoleMethod(GuiPopUpMenuCtrl, forceClose, void, 2, 2, "() Use the forceClose method to force the menu to close.\n"
   "This is useful for making menus that fold up after a short delay when the mouse leaves the menu area.\n"
   "@return No return value.\n"
   "@sa forceOnAction")
{
   object->closePopUp();
}

ConsoleMethod(GuiPopUpMenuCtrl, getSelected, S32, 2, 2, "() Use the getSelected method to get the ID of the last selected entry.\n"
   "@return Returns the ID of the currently selected entry, or 0 meaning no menu was selected after the last menu open.")
{
   return object->getSelected();
}

ConsoleMethod(GuiPopUpMenuCtrl, setSelected, void, 3, 4, "(int id, [scriptCallback=true]) Set the object status as selected\n"
   "@param id The object's ID.\n"
   "@param scriptCallback Flag whether to notify\n"
   "@return No return value.")
{
   if (argc > 3)
      object->setSelected(dAtoi(argv[2]), dAtob(argv[3]));
   else
      object->setSelected(dAtoi(argv[2]));
}

ConsoleMethod(GuiPopUpMenuCtrl, setFirstSelected, void, 2, 2, "() \n"
   "@return No return value.")
{
   object->setFirstSelected();
}

ConsoleMethod(GuiPopUpMenuCtrl, setNoneSelected, void, 2, 2, "() Deselects all popup menu controls\n"
   "@return No return value.")
{
   object->setNoneSelected();
}

ConsoleMethod(GuiPopUpMenuCtrl, getTextById, const char*, 3, 3, "( ID ) Use the getTextById method to get the text value for the menu item represented by ID.\n"
   "@param ID An integer value representing the ID of a menu item.\n"
   "@return Returns a string containing the menu item corresponding to ID, or a NULL string if no menu item has the specified ID.\n"
   "@sa add, getText")
{
   return(object->getTextById(dAtoi(argv[2])));
}

ConsoleMethod(GuiPopUpMenuCtrl, setEnumContent, void, 4, 4, "(string class, string enum)"
   "This fills the popup with a classrep's field enumeration type info.\n\n"
   "More of a helper function than anything.   If console access to the field list is added, "
   "at least for the enumerated types, then this should go away.."
   "@param className The class name associated with this enum content.\n"
   "@param enumName The name of the enumerated entry to add to the menu. This value must match an enum string as exposed by the engine for the class. The menu item will have the same text as the enum string name, and the ID will be equal to the enumerated entries value.\n"
   "@return No return value")
{
   AbstractClassRep * classRep = AbstractClassRep::getClassList();

   // walk the class list to get our class
   while (classRep)
   {
      if (!dStricmp(classRep->getClassName(), argv[2]))
         break;
      classRep = classRep->getNextClass();
   }

   // get it?
   if (!classRep)
   {
      Con::warnf(ConsoleLogEntry::General, "failed to locate class rep for '%s'", argv[2]);
      return;
   }

   // walk the fields to check for this one (findField checks StringTableEntry ptrs...)
   U32 i;
   for (i = 0; i < (U32)classRep->mFieldList.size(); i++)
      if (!dStricmp(classRep->mFieldList[i].pFieldname, argv[3]))
         break;

   // found it?   
   if (i == classRep->mFieldList.size())
   {
      Con::warnf(ConsoleLogEntry::General, "failed to locate field '%s' for class '%s'", argv[3], argv[2]);
      return;
   }

   const AbstractClassRep::Field & field = classRep->mFieldList[i];

   // check the type
   if (field.type != TypeEnum)
   {
      Con::warnf(ConsoleLogEntry::General, "field '%s' is not an enumeration for class '%s'", argv[3], argv[2]);
      return;
   }

   AssertFatal(field.table, avar("enumeration '%s' for class '%s' with NULL ", argv[3], argv[2]));

   // fill it
   for (i = 0; i < (U32)field.table->size; i++)
      object->addEntry(field.table->table[i].label, field.table->table[i].index);
}

//------------------------------------------------------------------------------
ConsoleMethod(GuiPopUpMenuCtrl, findText, S32, 3, 3, "( text ) Use the findText method to locate the string text in the list of menu items. It will return the ID of the first entry found.\n"
   "@param text A string containing the text to search for.\n"
   "@return Returns an integer value representing the ID of the menu item, or -1 if the text was not found.")
{
   return(object->findText(argv[2]));
}

//------------------------------------------------------------------------------
ConsoleMethod(GuiPopUpMenuCtrl, size, S32, 2, 2, "() Get the size of the menu, ie the number of entries in it.\n"
   "@return The numbers of entries as an integer.")
{
   return(object->getNumEntries());
}

//------------------------------------------------------------------------------
ConsoleMethod(GuiPopUpMenuCtrl, replaceText, void, 3, 3, "( enable ) Use the replaceText method to enable the updating of the menu bar text when a menu item is selected.\n"
   "This does not prevent changing the menu bar text with setText.\n"
   "@param enable A boolean value enabling or disabling the automatic updating of the menu bar text when a selection is made.\n"
   "@return No return value.\n"
   "@sa getText, setText")
{
   object->replaceText(dAtoi(argv[2]));
}

extern "C"{
   DLL_PUBLIC GuiPopupTextListCtrl* GuiPopupTextListCtrlCreateInstance()
   {
      return new GuiPopupTextListCtrl();
   }

   DLL_PUBLIC GuiPopUpMenuCtrl* GuiPopUpMenuCtrlCreateInstance()
   {
      return new GuiPopUpMenuCtrl();
   }

   DLL_PUBLIC void GuiPopUpMenuCtrlAdd(GuiPopUpMenuCtrl* ctrl, const char* entryText, S32 entryID, S32 entryScheme)
   {
	   ctrl->addEntry(entryText, entryID, entryScheme);
   }

   DLL_PUBLIC void GuiPopUpMenuCtrlAddScheme(GuiPopUpMenuCtrl* ctrl, S32 entryScheme, CInterface::ColorParam fontColor, CInterface::ColorParam fontColorHL, CInterface::ColorParam fontColorSEL)
   {
	   ctrl->addScheme(entryScheme, fontColor, fontColorHL, fontColorSEL);
   }

   DLL_PUBLIC void GuiPopUpMenuCtrlSetText(GuiPopUpMenuCtrl* ctrl, const char* text)
   {
	   ctrl->setText(text);
   }

   DLL_PUBLIC const char* GuiPopUpMenuCtrlGetText(GuiPopUpMenuCtrl* ctrl)
   {
	   return CInterface::GetMarshallableString(ctrl->getText());
   }

   DLL_PUBLIC void GuiPopUpMenuCtrlClear(GuiPopUpMenuCtrl* ctrl)
   {
	   ctrl->clear();
   }

   DLL_PUBLIC void GuiPopUpMenuCtrlSort(GuiPopUpMenuCtrl* ctrl)
   {
	   ctrl->sort();
   }

   DLL_PUBLIC void GuiPopUpMenuCtrlSortID(GuiPopUpMenuCtrl* ctrl)
   {
	   ctrl->sortID();
   }

   DLL_PUBLIC void GuiPopUpMenuCtrlForceOnAction(GuiPopUpMenuCtrl* ctrl)
   {
	   ctrl->onAction();
   }

   DLL_PUBLIC void GuiPopUpMenuCtrlClosePopUp(GuiPopUpMenuCtrl* ctrl)
   {
	   ctrl->closePopUp();
   }

   DLL_PUBLIC S32 GuiPopUpMenuCtrlGetSelected(GuiPopUpMenuCtrl* ctrl)
   {
	   return ctrl->getSelected();
   }

   DLL_PUBLIC void GuiPopUpMenuCtrlSetSelected(GuiPopUpMenuCtrl* ctrl, S32 id, bool scriptCallback)
   {
	   ctrl->setSelected(id, scriptCallback);
   }

   DLL_PUBLIC void GuiPopUpMenuCtrlSetFirstSelected(GuiPopUpMenuCtrl* ctrl)
   {
	   ctrl->setFirstSelected();
   }

   DLL_PUBLIC void GuiPopUpMenuCtrlSetNoneSelected(GuiPopUpMenuCtrl* ctrl)
   {
	   ctrl->setNoneSelected();
   }

   DLL_PUBLIC const char* GuiPopUpMenuCtrlGetTextById(GuiPopUpMenuCtrl* ctrl, S32 ID)
   {
	   return CInterface::GetMarshallableString(ctrl->getTextById(ID));
   }

   DLL_PUBLIC void GuiPopUpMenuCtrlSetEnumContent(GuiPopUpMenuCtrl* ctrl, const char* contentClass, const char* contentEnum)
   {
	   AbstractClassRep * classRep = AbstractClassRep::getClassList();

	   // walk the class list to get our class
	   while (classRep)
	   {
		   if (!dStricmp(classRep->getClassName(), contentClass))
			   break;
		   classRep = classRep->getNextClass();
	   }

	   // get it?
	   if (!classRep)
	   {
		   Con::warnf(ConsoleLogEntry::General, "failed to locate class rep for '%s'", contentClass);
		   return;
	   }

	   // walk the fields to check for this one (findField checks StringTableEntry ptrs...)
	   U32 i;
	   for (i = 0; i < (U32)classRep->mFieldList.size(); i++)
		   if (!dStricmp(classRep->mFieldList[i].pFieldname, contentEnum))
			   break;

	   // found it?   
	   if (i == classRep->mFieldList.size())
	   {
		   Con::warnf(ConsoleLogEntry::General, "failed to locate field '%s' for class '%s'", contentEnum, contentClass);
		   return;
	   }

	   const AbstractClassRep::Field & field = classRep->mFieldList[i];

	   // check the type
	   if (field.type != TypeEnum)
	   {
		   Con::warnf(ConsoleLogEntry::General, "field '%s' is not an enumeration for class '%s'", contentEnum, contentClass);
		   return;
	   }

	   AssertFatal(field.table, avar("enumeration '%s' for class '%s' with NULL ", contentEnum, contentClass));

	   // fill it
	   for (i = 0; i < (U32)field.table->size; i++)
		   ctrl->addEntry(field.table->table[i].label, field.table->table[i].index);
   }

   DLL_PUBLIC S32 GuiPopUpMenuCtrlFindText(GuiPopUpMenuCtrl* ctrl, const char* text)
   {
	   return ctrl->findText(text);
   }

   DLL_PUBLIC S32 GuiPopUpMenuCtrlSize(GuiPopUpMenuCtrl* ctrl)
   {
	   return ctrl->getNumEntries();
   }

   DLL_PUBLIC void GuiPopUpMenuCtrlReplaceText(GuiPopUpMenuCtrl* ctrl, bool enable)
   {
	   ctrl->replaceText(enable);
   }
}