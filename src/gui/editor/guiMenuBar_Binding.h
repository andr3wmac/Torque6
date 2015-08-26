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

ConsoleMethod(GuiMenuBar, clearMenus, void, 2, 2, "() Clears all menus and sub-menus from the menu bar.\n"
   "@return No return value")
{
   object->clearMenus();
}

ConsoleMethod(GuiMenuBar, setMenuMargins, void, 5, 5, "(S32 horizontalMargin, S32 verticalMargin, S32 bitmapToTextSpacing) Sets the menu rendering margins: horizontal, vertical, bitmap spacing.")
{
   object->mHorizontalMargin = dAtoi(argv[2]);
   object->mVerticalMargin = dAtoi(argv[3]);
   object->mBitmapMargin = dAtoi(argv[4]);
}

ConsoleMethod(GuiMenuBar, addMenu, void, 4, 4, "( menuName , menuID ) Adds a new menu to the menu bar.\n"
   "@param menuName The text (name) of the new menu entry.\n"
   "@param menuID The ID of the new menu entry.\n"
   "@return No return value")
{
   if (dIsdigit(argv[2][0]))
   {
      Con::errorf("Cannot add menu %s (id = %s).  First character of a menu's text cannot be a digit.", argv[2], argv[3]);
      return;
   }
   object->addMenu(argv[2], dAtoi(argv[3]));
}

ConsoleMethod(GuiMenuBar, addMenuItem, void, 5, 7, "( menuID | menuName , menuItemName , menuItemID , [ accelerator ] , [ checkGroup ] ) Adds a sub-menu entry to the specified menu.\n"
   "@param menuID The ID of the menu.\n"
   "@param menuName The text (name) of the menu.\n"
   "@param menuItemID The ID of the menu item.\n"
   "@param menuItemName The text (name) of the menu item.\n"
   "@param accelerator A boolean value. If set to true, the sub-menu entry is checked, otherwise it is unchecked.\n"
   "@param checkGroup The check group this item should belong to, if any.\n"
   "@return No return value")
{
   if (dIsdigit(argv[3][0]))
   {
      Con::errorf("Cannot add menu item %s (id = %s).  First character of a menu item's text cannot be a digit.", argv[3], argv[4]);
      return;
   }
   GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
   if (!menu)
   {
      Con::errorf("Cannot find menu %s for addMenuItem.", argv[2]);
      return;
   }
   object->addMenuItem(menu, argv[3], dAtoi(argv[4]), argc == 5 ? "" : argv[5], argc < 7 ? -1 : dAtoi(argv[6]));
}

ConsoleMethod(GuiMenuBar, setMenuItemEnable, void, 5, 5, "( menuID | menuName , menuItemID | menuItemName , enabled ) Sets the menu item to enabled or disabled.\n"
   "@param menuID The ID of the menu.\n"
   "@param menuName The text (name) of the menu.\n"
   "@param menuItemID The ID of the menu item.\n"
   "@param menuItemName The text (name) of the menu item.\n"
   "@param enabled A boolean value. If set to true, the sub-menu entry is enabled, otherwise it is disabled.\n"
   "@return No return value")
{
   GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
   if (!menu)
   {
      Con::errorf("Cannot find menu %s for setMenuItemEnable.", argv[2]);
      return;
   }
   GuiMenuBar::MenuItem *menuItem = object->findMenuItem(menu, argv[3]);
   if (!menuItem)
   {
      Con::errorf("Cannot find menu item %s for setMenuItemEnable.", argv[3]);
      return;
   }
   menuItem->enabled = dAtob(argv[4]);
}

ConsoleMethod(GuiMenuBar, setCheckmarkBitmapIndex, void, 3, 3, "(S32 bitmapindex) - sets the menu bitmap index for the check mark image.\n"
   "@return No Return Value.")
{
   object->mCheckmarkBitmapIndex = dAtoi(argv[2]);
}

ConsoleMethod(GuiMenuBar, setMenuItemChecked, void, 5, 5, "( menuID | menuName , menuItemID | menuItemName , checked ) Sets the menu item bitmap to a check mark, which must be the first element in the bitmap array. Any other menu items in the menu with the same check group become unchecked if they are checked.\n"
   "@param menuID The ID of the menu.\n"
   "@param menuName The text (name) of the menu.\n"
   "@param menuItemID The ID of the menu item.\n"
   "@param menuItemName The text (name) of the menu item.\n"
   "@param checked A boolean value. If set to true, the sub-menu entry is checked, otherwise it is unchecked.\n"
   "@return No return value")
{
   GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
   if (!menu)
   {
      Con::errorf("Cannot find menu %s for setMenuItemChecked.", argv[2]);
      return;
   }
   GuiMenuBar::MenuItem *menuItem = object->findMenuItem(menu, argv[3]);
   if (!menuItem)
   {
      Con::errorf("Cannot find menu item %s for setMenuItemChecked.", argv[3]);
      return;
   }
   bool checked = dAtob(argv[4]);
   if (checked && menuItem->checkGroup != -1)
   {
      // first, uncheck everything in the group:
      for (GuiMenuBar::MenuItem *itemWalk = menu->firstMenuItem; itemWalk; itemWalk = itemWalk->nextMenuItem)
         if (itemWalk->checkGroup == menuItem->checkGroup && itemWalk->bitmapIndex == object->mCheckmarkBitmapIndex)
            itemWalk->bitmapIndex = -1;
   }
   menuItem->bitmapIndex = checked ? object->mCheckmarkBitmapIndex : -1;
}

ConsoleMethod(GuiMenuBar, setMenuText, void, 4, 4, "( menuID | menuName , newMenuText ) Sets the text of the specified menu to the new string.\n"
   "@param menuID The ID of the menu.\n"
   "@param menuName The text (name) of the menu.\n"
   "@param newMenuText The new text to give the menu entry.\n"
   "@return No return value")
{
   if (dIsdigit(argv[3][0]))
   {
      Con::errorf("Cannot name menu %s to %s.  First character of a menu's text cannot be a digit.", argv[2], argv[3]);
      return;
   }
   GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
   if (!menu)
   {
      Con::errorf("Cannot find menu %s for setMenuText.", argv[2]);
      return;
   }
   dFree(menu->text);
   menu->text = dStrdup(argv[3]);
   object->menuBarDirty = true;
}
ConsoleMethod(GuiMenuBar, setMenuBitmapIndex, void, 6, 6, "(string menu, S32 bitmapindex, bool bitmaponly, bool drawborder) Sets the bitmap index for the menu and toggles rendering only the bitmap.\n"
   "@return No return value.")
{
   GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
   if (!menu)
   {
      Con::errorf("Cannot find menu %s for setMenuBitmapIndex.", argv[2]);
      return;
   }

   menu->bitmapIndex = dAtoi(argv[3]);
   menu->drawBitmapOnly = dAtob(argv[4]);
   menu->drawBorder = dAtob(argv[5]);

   object->menuBarDirty = true;
}

ConsoleMethod(GuiMenuBar, setMenuVisible, void, 4, 4, "( menuID | menuName , visible ) Use the setMenuVisible method to enable or disable the visibility of a specific menu entry.\n"
   "@param menuID The ID of the menu.\n"
   "@param menuName The text (name) of the menu.\n"
   "@param visible A boolean value. If set to true, this menu entry will be shown, otherwise it will be hidden.\n"
   "@return No return value")
{
   GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
   if (!menu)
   {
      Con::errorf("Cannot find menu %s for setMenuVisible.", argv[2]);
      return;
   }
   menu->visible = dAtob(argv[3]);
   object->menuBarDirty = true;
   object->setUpdate();
}

ConsoleMethod(GuiMenuBar, setMenuItemText, void, 5, 5, "( menuID | menuName , menuItemID | menuItemName , newMenuItemText ) Sets the text of the specified menu item to the new string.\n"
   "@param menuID The ID of the menu.\n"
   "@param menuName The text (name) of the menu.\n"
   "@param menuItemID The ID of the menu item.\n"
   "@param menuItemName The text (name) of the menu item.\n"
   "@param newMenuItemText The new text for the specified sub-menu entry.\n"
   "@return No return value")
{
   if (dIsdigit(argv[4][0]))
   {
      Con::errorf("Cannot name menu item %s to %s.  First character of a menu item's text cannot be a digit.", argv[3], argv[4]);
      return;
   }
   GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
   if (!menu)
   {
      Con::errorf("Cannot find menu %s for setMenuItemText.", argv[2]);
      return;
   }
   GuiMenuBar::MenuItem *menuItem = object->findMenuItem(menu, argv[3]);
   if (!menuItem)
   {
      Con::errorf("Cannot find menu item %s for setMenuItemText.", argv[3]);
      return;
   }
   dFree(menuItem->text);
   menuItem->text = dStrdup(argv[4]);
}

ConsoleMethod(GuiMenuBar, setMenuItemVisible, void, 5, 5, "( menuID | menuName, menuItemID | menuItemName, visible ) Use the setMenuItemVisible method to enable or disable the visibility of a specific sub-menu entry.\n"
   "@param menuID The ID of the menu.\n"
   "@param menuName The text (name) of the menu.\n"
   "@param menuItemID The ID of the menu item.\n"
   "@param menuItemName The text (name) of the menu item.\n"
   "@param visible A boolean value. If set to true, this sub-menu entry will be shown, otherwise it will be hidden.\n"
   "@return No return value")
{
   GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
   if (!menu)
   {
      Con::errorf("Cannot find menu %s for setMenuItemVisible.", argv[2]);
      return;
   }
   GuiMenuBar::MenuItem *menuItem = object->findMenuItem(menu, argv[3]);
   if (!menuItem)
   {
      Con::errorf("Cannot find menu item %s for setMenuItemVisible.", argv[3]);
      return;
   }
   menuItem->visible = dAtob(argv[4]);
}

ConsoleMethod(GuiMenuBar, setMenuItemBitmap, void, 5, 5, "( menuID | menuName , menuItemID | menuItemName , bitmapIndex ) Sets the specified menu item bitmap index in the bitmap array. Setting the item's index to -1 will remove any bitmap.\n"
   "@param menuID The ID of the menu.\n"
   "@param menuName The text (name) of the menu.\n"
   "@param menuItemID The ID of the menu item.\n"
   "@param menuItemName The text (name) of the menu item.\n"
   "@param bitMapIndex An integer value specifying the row of bitmap entries to use for sub-menu entry.\n"
   "@return No return value")
{
   GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
   if (!menu)
   {
      Con::errorf("Cannot find menu %s for setMenuItemBitmap.", argv[2]);
      return;
   }
   GuiMenuBar::MenuItem *menuItem = object->findMenuItem(menu, argv[3]);
   if (!menuItem)
   {
      Con::errorf("Cannot find menu item %s for setMenuItemBitmap.", argv[3]);
      return;
   }
   menuItem->bitmapIndex = dAtoi(argv[4]);
}

ConsoleMethod(GuiMenuBar, removeMenuItem, void, 4, 4, "( menuID | menuName , menuItemID | menuItemName ) Removes the specified menu item from the menu.\n"
   "@param menuID The ID of the menu.\n"
   "@param menuName The text (name) of the menu.\n"
   "@param menuItemID The ID of the menu item.\n"
   "@param menuItemName The text (name) of the menu item.\n"
   "@return No return value")
{
   GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
   if (!menu)
   {
      Con::errorf("Cannot find menu %s for removeMenuItem.", argv[2]);
      return;
   }
   GuiMenuBar::MenuItem *menuItem = object->findMenuItem(menu, argv[3]);
   if (!menuItem)
   {
      Con::errorf("Cannot find menu item %s for removeMenuItem.", argv[3]);
      return;
   }
   object->removeMenuItem(menu, menuItem);
}

ConsoleMethod(GuiMenuBar, clearMenuItems, void, 3, 3, "( menuID | menuName ) Removes all the sub-menu items from the specified menu.\n"
   "@param menuID The ID of the menu.\n"
   "@param menuName The text (name) of the menu.\n"
   "@return No return value")
{
   GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
   if (!menu)
   {
      //Con::errorf("Cannot find menu %s for clearMenuItems.", argv[2]);
      return;
   }
   object->clearMenuItems(menu);
}

ConsoleMethod(GuiMenuBar, removeMenu, void, 3, 3, "( menuID | menuName ) Removes the specified menu from the menu bar.\n"
   "@param menuID The ID of the menu.\n"
   "@param menuName The text (name) of the menu.\n"
   "@param menuItemID The ID of the menu item.\n"
   "@param menuItemName The text (name) of the menu item.\n"
   "@param checked A boolean value. If set to true, the sub-menu entry is checked, otherwise it is unchecked.\n"
   "@return No return value")
{
   GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
   if (!menu)
   {
      //Con::errorf("Cannot find menu %s for removeMenu.", argv[2]);
      return;
   }
   object->clearMenuItems(menu);
   object->menuBarDirty = true;
}

//------------------------------------------------------------------------------
// DAW: Submenu console methods
//------------------------------------------------------------------------------

ConsoleMethod(GuiMenuBar, setMenuItemSubmenuState, void, 5, 5, "(string menu, string menuItem, bool isSubmenu) Sets the given menu item to be a submenu\n"
   "@param menu The menu where the sub menu is located.\n"
   "@param menuItem The menu item to set as a sub menu.\n"
   "@param inSubmenu A boolean value signifying whether or not it is a submenu.\n"
   "@return No return value.")
{

   GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
   if (!menu)
   {
      Con::errorf("Cannot find menu %s for setMenuItemSubmenuState.", argv[2]);
      return;
   }

   GuiMenuBar::MenuItem *menuitem = object->findMenuItem(menu, argv[3]);
   if (!menuitem)
   {
      Con::errorf("Cannot find menuitem %s for setMenuItemSubmenuState.", argv[3]);
      return;
   }

   menuitem->isSubmenu = dAtob(argv[4]);
}

ConsoleMethod(GuiMenuBar, addSubmenuItem, void, 6, 8, "(string menu, string menuItem, string submenuItemText, int submenuItemId, string accelerator = NULL, int checkGroup = -1) Adds a menu item to the specified menu.  The menu argument can be either the text of a menu or its id.")
{
   if (dIsdigit(argv[4][0]))
   {
      Con::errorf("Cannot add submenu item %s (id = %s).  First character of a menu item's text cannot be a digit.", argv[4], argv[5]);
      return;
   }

   GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
   if (!menu)
   {
      Con::errorf("Cannot find menu %s for addMenuItem.", argv[2]);
      return;
   }

   GuiMenuBar::MenuItem *menuitem = object->findMenuItem(menu, argv[3]);
   if (!menuitem)
   {
      Con::errorf("Cannot find menuitem %s for addSubmenuItem.", argv[3]);
      return;
   }

   object->addSubmenuItem(menu, menuitem, argv[4], dAtoi(argv[5]), argc == 6 ? "" : argv[6], argc < 8 ? -1 : dAtoi(argv[7]));
}

ConsoleMethod(GuiMenuBar, clearSubmenuItems, void, 4, 4, "(string menu, string menuItem) Removes all the menu items from the specified submenu.\n"
   "@return No return value.")
{
   GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
   if (!menu)
   {
      Con::errorf("Cannot find menu %s for clearSubmenuItems.", argv[2]);
      return;
   }

   GuiMenuBar::MenuItem *menuitem = object->findMenuItem(menu, argv[3]);
   if (!menuitem)
   {
      Con::errorf("Cannot find menuitem %s for clearSubmenuItems.", argv[3]);
      return;
   }

   object->clearSubmenuItems(menuitem);
}

ConsoleMethod(GuiMenuBar, setSubmenuItemChecked, void, 6, 6, "(string menu, string menuItem, string submenuItemText, bool checked) Sets the menu item bitmap to a check mark, which by default is the first element in the bitmap array (although this may be changed with setCheckmarkBitmapIndex()).  Any other menu items in the menu with the same check group become unchecked if they are checked.")
{
   // Find the parent menu
   GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
   if (!menu)
   {
      Con::errorf("Cannot find menu %s for setSubmenuItemChecked.", argv[2]);
      return;
   }

   // Find the parent menu item
   GuiMenuBar::MenuItem *menuItem = object->findMenuItem(menu, argv[3]);
   if (!menuItem)
   {
      Con::errorf("Cannot find menu item %s for setSubmenuItemChecked.", argv[3]);
      return;
   }

   // Find the submenu item
   GuiMenuBar::MenuItem *submenuItem = object->findSubmenuItem(menu, argv[3], argv[4]);
   if (!submenuItem)
   {
      Con::errorf("Cannot find submenu item %s for setSubmenuItemChecked.", argv[4]);
      return;
   }

   bool checked = dAtob(argv[5]);
   if (checked && submenuItem->checkGroup != -1)
   {
      // first, uncheck everything in the group:
      for (GuiMenuBar::MenuItem *itemWalk = menuItem->firstSubmenuItem; itemWalk; itemWalk = itemWalk->nextMenuItem)
         if (itemWalk->checkGroup == submenuItem->checkGroup && itemWalk->bitmapIndex == object->mCheckmarkBitmapIndex)
            itemWalk->bitmapIndex = -1;
   }
   submenuItem->bitmapIndex = checked ? object->mCheckmarkBitmapIndex : -1;
}

extern "C"{
   DLL_PUBLIC GuiMenuBar* GuiMenuBarCreateInstance()
   {
      return new GuiMenuBar();
   }

   DLL_PUBLIC S32 GuiMenuBarGetPadding(GuiMenuBar* menuBar)
   {
      return menuBar->mPadding;
   }

   DLL_PUBLIC void GuiMenuBarSetPadding(GuiMenuBar* menuBar, S32 padding)
   {
      menuBar->mPadding = padding;
   }

   DLL_PUBLIC void GuiMenuBarClearMenus(GuiMenuBar* menuBar)
   {
      menuBar->clearMenus();
   }

   DLL_PUBLIC void GuiMenuBarSetMenuMargins(GuiMenuBar* menuBar, S32 horizontalMargin, S32 verticalMargin, S32 bitmapToTextSpacing)
   {
      menuBar->mHorizontalMargin = horizontalMargin;
      menuBar->mVerticalMargin = verticalMargin;
      menuBar->mBitmapMargin = bitmapToTextSpacing;
   }

   DLL_PUBLIC void GuiMenuBarAddMenu(GuiMenuBar* menuBar, const char* menuName, S32 menuID)
   {
      menuBar->addMenu(menuName, menuID);
   }

   // CTODO default values
   DLL_PUBLIC void GuiMenuBarAddMenuItem(GuiMenuBar* menuBar, const char* menuName, const char* menuItemName, S32 menuItemID, const char* accelerator, S32 checkGroup)
   {
      if (dIsdigit(menuItemName[0]))
      {
         Con::errorf("Cannot add menu item %s (id = %s).  First character of a menu item's text cannot be a digit.", menuItemName, menuItemID);
         return;
      }
      GuiMenuBar::Menu *menu = menuBar->findMenu(menuName);
      if (!menu)
      {
         Con::errorf("Cannot find menu %s for addMenuItem.", menuName);
         return;
      }
      menuBar->addMenuItem(menu, menuItemName, menuItemID, accelerator, checkGroup);
   }

   DLL_PUBLIC void GuiMenuBarSetMenuItemEnable(GuiMenuBar* menuBar, const char* menuName, const char* menuItemName, bool enabled)
   {
      GuiMenuBar::Menu *menu = menuBar->findMenu(menuName);
      if (!menu)
      {
         Con::errorf("Cannot find menu %s for setMenuItemEnable.", menuName);
         return;
      }
      GuiMenuBar::MenuItem *menuItem = menuBar->findMenuItem(menu, menuItemName);
      if (!menuItem)
      {
         Con::errorf("Cannot find menu item %s for setMenuItemEnable.", menuItemName);
         return;
      }
      menuItem->enabled = enabled;
   }

   DLL_PUBLIC void GuiMenuBarSetCheckmarkBitmapIndex(GuiMenuBar* menuBar, S32 bitmapIndex)
   {
      menuBar->mCheckmarkBitmapIndex = bitmapIndex;
   }

   DLL_PUBLIC void GuiMenuBarSetMenuItemChecked(GuiMenuBar* menuBar, const char* menuName, const char* menuItemName, bool check)
   {
      GuiMenuBar::Menu *menu = menuBar->findMenu(menuName);
      if (!menu)
      {
         Con::errorf("Cannot find menu %s for setMenuItemChecked.", menuName);
         return;
      }
      GuiMenuBar::MenuItem *menuItem = menuBar->findMenuItem(menu, menuItemName);
      if (!menuItem)
      {
         Con::errorf("Cannot find menu item %s for setMenuItemChecked.", menuItemName);
         return;
      }
      if (check && menuItem->checkGroup != -1)
      {
         // first, uncheck everything in the group:
         for (GuiMenuBar::MenuItem *itemWalk = menu->firstMenuItem; itemWalk; itemWalk = itemWalk->nextMenuItem)
            if (itemWalk->checkGroup == menuItem->checkGroup && itemWalk->bitmapIndex == menuBar->mCheckmarkBitmapIndex)
               itemWalk->bitmapIndex = -1;
      }
      menuItem->bitmapIndex = check ? menuBar->mCheckmarkBitmapIndex : -1;
   }

   DLL_PUBLIC void GuiMenuBarSetMenuText(GuiMenuBar* menuBar, const char* menuName, const char* newMenuText)
   {
      if (dIsdigit(newMenuText[0]))
      {
         Con::errorf("Cannot name menu %s to %s.  First character of a menu's text cannot be a digit.", menuName, newMenuText);
         return;
      }
      GuiMenuBar::Menu *menu = menuBar->findMenu(menuName);
      if (!menu)
      {
         Con::errorf("Cannot find menu %s for setMenuText.", menuName);
         return;
      }
      dFree(menu->text);
      menu->text = dStrdup(newMenuText);
      menuBar->menuBarDirty = true;
   }

   DLL_PUBLIC void GuiMenuBarSetMenuBitmapIndex(GuiMenuBar* menuBar, const char* menuName, S32 bitmapIndex, bool bitmapOnly, bool drawBorder)
   {
      GuiMenuBar::Menu *menu = menuBar->findMenu(menuName);
      if (!menu)
      {
         Con::errorf("Cannot find menu %s for setMenuBitmapIndex.", menuName);
         return;
      }

      menu->bitmapIndex = bitmapIndex;
      menu->drawBitmapOnly = bitmapOnly;
      menu->drawBorder = drawBorder;

      menuBar->menuBarDirty = true;
   }

   DLL_PUBLIC void GuiMenuBarSetMenuVisible(GuiMenuBar* menuBar, const char* menuName, bool visible)
   {
      GuiMenuBar::Menu *menu = menuBar->findMenu(menuName);
      if (!menu)
      {
         Con::errorf("Cannot find menu %s for setMenuBitmapIndex.", menuName);
         return;
      }
      menu->visible = visible;
      menuBar->menuBarDirty = true;
      menuBar->setUpdate();
   }

   DLL_PUBLIC void GuiMenuBarSetMenuItemText(GuiMenuBar* menuBar, const char* menuName, const char* menuItemName, const char* newMenuItemText)
   {
      if (dIsdigit(newMenuItemText[0]))
      {
         Con::errorf("Cannot name menu item %s to %s.  First character of a menu item's text cannot be a digit.", menuItemName, newMenuItemText);
         return;
      }
      GuiMenuBar::Menu *menu = menuBar->findMenu(menuName);
      if (!menu)
      {
         Con::errorf("Cannot find menu %s for setMenuItemChecked.", menuName);
         return;
      }
      GuiMenuBar::MenuItem *menuItem = menuBar->findMenuItem(menu, menuItemName);
      if (!menuItem)
      {
         Con::errorf("Cannot find menu item %s for setMenuItemChecked.", menuItemName);
         return;
      }
      dFree(menuItem->text);
      menuItem->text = dStrdup(newMenuItemText);
   }

   DLL_PUBLIC void GuiMenuBarSetMenuItemVisible(GuiMenuBar* menuBar, const char* menuName, const char* menuItemName, bool visible)
   {
      GuiMenuBar::Menu *menu = menuBar->findMenu(menuName);
      if (!menu)
      {
         Con::errorf("Cannot find menu %s for setMenuItemChecked.", menuName);
         return;
      }
      GuiMenuBar::MenuItem *menuItem = menuBar->findMenuItem(menu, menuItemName);
      if (!menuItem)
      {
         Con::errorf("Cannot find menu item %s for setMenuItemChecked.", menuItemName);
         return;
      }
      menuItem->visible = visible;
   }

   DLL_PUBLIC void GuiMenuBarSetMenuItemBitmap(GuiMenuBar* menuBar, const char* menuName, const char* menuItemName, S32 bitmapIndex)
   {
      GuiMenuBar::Menu *menu = menuBar->findMenu(menuName);
      if (!menu)
      {
         Con::errorf("Cannot find menu %s for setMenuItemChecked.", menuName);
         return;
      }
      GuiMenuBar::MenuItem *menuItem = menuBar->findMenuItem(menu, menuItemName);
      if (!menuItem)
      {
         Con::errorf("Cannot find menu item %s for setMenuItemChecked.", menuItemName);
         return;
      }
      menuItem->bitmapIndex = bitmapIndex;
   }

   DLL_PUBLIC void GuiMenuBarRemoveMenuItem(GuiMenuBar* menuBar, const char* menuName, const char* menuItemName)
   {
      GuiMenuBar::Menu *menu = menuBar->findMenu(menuName);
      if (!menu)
      {
         Con::errorf("Cannot find menu %s for setMenuItemChecked.", menuName);
         return;
      }
      GuiMenuBar::MenuItem *menuItem = menuBar->findMenuItem(menu, menuItemName);
      if (!menuItem)
      {
         Con::errorf("Cannot find menu item %s for setMenuItemChecked.", menuItemName);
         return;
      }
      menuBar->removeMenuItem(menu, menuItem);
   }

   DLL_PUBLIC void GuiMenuBarClearMenuItems(GuiMenuBar* menuBar, const char* menuName)
   {
      GuiMenuBar::Menu *menu = menuBar->findMenu(menuName);
      if (!menu)
      {
         Con::errorf("Cannot find menu %s for clearMenuItems.", menuName);
         return;
      }
      menuBar->clearMenuItems(menu);
   }

   DLL_PUBLIC void GuiMenuBarRemoveMenu(GuiMenuBar* menuBar, const char* menuName)
   {
      GuiMenuBar::Menu *menu = menuBar->findMenu(menuName);
      if (!menu)
      {
         Con::errorf("Cannot find menu %s for clearMenuItems.", menuName);
         return;
      }
      menuBar->clearMenuItems(menu);
      menuBar->menuBarDirty = true;
   }

   DLL_PUBLIC void GuiMenuBarSetMenuItemSubmenuState(GuiMenuBar* menuBar, const char* menuName, const char* menuItemName, bool inSubmenu)
   {
      GuiMenuBar::Menu *menu = menuBar->findMenu(menuName);
      if (!menu)
      {
         Con::errorf("Cannot find menu %s for setMenuItemSubmenuState.", menuName);
         return;
      }

      GuiMenuBar::MenuItem *menuitem = menuBar->findMenuItem(menu, menuItemName);
      if (!menuitem)
      {
         Con::errorf("Cannot find menuitem %s for setMenuItemSubmenuState.", menuItemName);
         return;
      }

      menuitem->isSubmenu = inSubmenu;
   }

   //CTODO - default values
   DLL_PUBLIC void GuiMenuBarAddSubmenuItem(GuiMenuBar* menuBar, const char* menuName, const char* menuItemName, const char* submenuItemText, S32 submenuItemId, const char* accelerator, S32 checkGroup)
   {
      if (dIsdigit(submenuItemText[0]))
      {
         Con::errorf("Cannot add submenu item %s (id = %s).  First character of a menu item's text cannot be a digit.", submenuItemText, submenuItemId);
         return;
      }

      GuiMenuBar::Menu *menu = menuBar->findMenu(menuName);
      if (!menu)
      {
         Con::errorf("Cannot find menu %s for addSubmenuItem.", menuName);
         return;
      }

      GuiMenuBar::MenuItem *menuitem = menuBar->findMenuItem(menu, menuItemName);
      if (!menuitem)
      {
         Con::errorf("Cannot find menuitem %s for addSubmenuItem.", menuItemName);
         return;
      }

      menuBar->addSubmenuItem(menu, menuitem, submenuItemText, submenuItemId, accelerator, checkGroup);
   }

   DLL_PUBLIC void GuiMenuBarClearSubmenuItems(GuiMenuBar* menuBar, const char* menuName, const char* menuItemName)
   {
      GuiMenuBar::Menu *menu = menuBar->findMenu(menuName);
      if (!menu)
      {
         Con::errorf("Cannot find menu %s for clearSubmenuItems.", menuName);
         return;
      }

      GuiMenuBar::MenuItem *menuitem = menuBar->findMenuItem(menu, menuItemName);
      if (!menuitem)
      {
         Con::errorf("Cannot find menuitem %s for clearSubmenuItems.", menuItemName);
         return;
      }

      menuBar->clearSubmenuItems(menuitem);
   }

   DLL_PUBLIC void GuiMenuBarSetSubmenuItemChecked(GuiMenuBar* menuBar, const char* menuName, const char* menuItemName, const char* submenuItemText, bool check)
   {
      // Find the parent menu
      GuiMenuBar::Menu *menu = menuBar->findMenu(menuName);
      if (!menu)
      {
         Con::errorf("Cannot find menu %s for setSubmenuItemChecked.", menuName);
         return;
      }

      // Find the parent menu item
      GuiMenuBar::MenuItem *menuItem = menuBar->findMenuItem(menu, menuItemName);
      if (!menuItem)
      {
         Con::errorf("Cannot find menuitem %s for setSubmenuItemChecked.", menuItemName);
         return;
      }

      // Find the submenu item
      GuiMenuBar::MenuItem *submenuItem = menuBar->findSubmenuItem(menu, menuItemName, submenuItemText);
      if (!submenuItem)
      {
         Con::errorf("Cannot find submenu item %s for setSubmenuItemChecked.", submenuItemText);
         return;
      }

      if (check && submenuItem->checkGroup != -1)
      {
         // first, uncheck everything in the group:
         for (GuiMenuBar::MenuItem *itemWalk = menuItem->firstSubmenuItem; itemWalk; itemWalk = itemWalk->nextMenuItem)
            if (itemWalk->checkGroup == submenuItem->checkGroup && itemWalk->bitmapIndex == menuBar->mCheckmarkBitmapIndex)
               itemWalk->bitmapIndex = -1;
      }
      submenuItem->bitmapIndex = check ? menuBar->mCheckmarkBitmapIndex : -1;
   }
}