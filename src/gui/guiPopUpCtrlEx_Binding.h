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

ConsoleMethod(GuiPopUpMenuCtrlEx, add, void, 4, 5, "(string name, int idNum, int scheme=0) Adds a new pop up menu control\n"
   "@param name The control's name\n"
   "@param idNum The control's ID\n"
   "@param scheme The selected scheme (default 0)\n"
   "@return No Return Value")
{
   if (argc > 4)
      object->addEntry(argv[2], dAtoi(argv[3]), dAtoi(argv[4]));
   else
      object->addEntry(argv[2], dAtoi(argv[3]), 0);
}

ConsoleMethod(GuiPopUpMenuCtrlEx, addCategory, void, 3, 3, "(string text) Adds the given category to the list\n"
   "@param text The category to add\n"
   "@return No return value.")
{
   object->addEntry(argv[2], -1, 0);
}

ConsoleMethod(GuiPopUpMenuCtrlEx, addScheme, void, 6, 6, "(int id, ColorI fontColor, ColorI fontColorHL, ColorI fontColorSEL) Adds the defined scheme\n"
   "@param id The object id of the scheme.\n"
   "@param fontcolor The desired color of the font.\n"
   "@param fontColorHL The desired color on highlight.\n"
   "@param fontColorSEL The desired color on select.\n"
   "@return No return value.")
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

ConsoleMethod(GuiPopUpMenuCtrlEx, setText, void, 3, 3, "(string text) Set control text to given string.\n"
   "@param text The desired text.\n"
   "@return No return vlaue.")
{
   object->setText(argv[2]);
}

ConsoleMethod(GuiPopUpMenuCtrlEx, getText, const char*, 2, 2, "()\n @return Returns the control's text")
{
   return object->getText();
}

ConsoleMethod(GuiPopUpMenuCtrlEx, clear, void, 2, 2, "() Clear the popup list.\n"
   "@return No return value.")
{
   object->clear();
}

ConsoleMethod(GuiPopUpMenuCtrlEx, sort, void, 2, 2, "() Sort the list alphabetically.\n"
   "@return No return value.")
{
   object->sort();
}

// DAW: Added to sort the entries by ID
ConsoleMethod(GuiPopUpMenuCtrlEx, sortID, void, 2, 2, "() Sort the list by ID.\n"
   "@return No return value.")
{
   object->sortID();
}

ConsoleMethod(GuiPopUpMenuCtrlEx, forceOnAction, void, 2, 2, "")
{
   object->onAction();
}

ConsoleMethod(GuiPopUpMenuCtrlEx, forceClose, void, 2, 2, "")
{
   object->closePopUp();
}

ConsoleMethod(GuiPopUpMenuCtrlEx, getSelected, S32, 2, 2, "")
{
   return object->getSelected();
}

ConsoleMethod(GuiPopUpMenuCtrlEx, setSelected, void, 3, 3, "(int id)")
{
   object->setSelected(dAtoi(argv[2]));
}

ConsoleMethod(GuiPopUpMenuCtrlEx, setFirstSelected, void, 2, 2, "")
{
   object->setFirstSelected();
}

ConsoleMethod(GuiPopUpMenuCtrlEx, setNoneSelected, void, 2, 2, "")
{
   object->setNoneSelected();
}

ConsoleMethod(GuiPopUpMenuCtrlEx, getTextById, const char*, 3, 3, "(int id)")
{
   return(object->getTextById(dAtoi(argv[2])));
}

ConsoleMethod(GuiPopUpMenuCtrlEx, setEnumContent, void, 4, 4, "(string class, string enum)"
   "This fills the popup with a classrep's field enumeration type info.\n\n"
   "More of a helper function than anything.   If console access to the field list is added, "
   "at least for the enumerated types, then this should go away..")
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
ConsoleMethod(GuiPopUpMenuCtrlEx, findText, S32, 3, 3, "(string text)"
   "Returns the position of the first entry containing the specified text.")
{
   return(object->findText(argv[2]));
}

//------------------------------------------------------------------------------
ConsoleMethod(GuiPopUpMenuCtrlEx, size, S32, 2, 2, "Get the size of the menu - the number of entries in it.")
{
   return(object->getNumEntries());
}

//------------------------------------------------------------------------------
ConsoleMethod(GuiPopUpMenuCtrlEx, replaceText, void, 3, 3, "(bool doReplaceText)")
{
   object->replaceText(dAtoi(argv[2]));
}

extern "C"{


}