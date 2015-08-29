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

#include "console/consoleTypes.h"
#include "console/console.h"
#include "graphics/dgl.h"
#include "gui/guiCanvas.h"
#include "gui/guiDefaultControlRender.h"
#include "gui/guiTextListCtrl.h"
#include "input/actionMap.h"
#include "gui/editor/guiMenuBar.h"
#include "gui/editor/guiMenuBar_Binding.h"

// menu bar:
// basic idea - fixed height control bar at the top of a window, placed and sized in gui editor
// menu text for menus or menu items should not begin with a digit
// all menus can be removed via the clearMenus() console command
// each menu is added via the addMenu(menuText, menuId) console command
// each menu is added with a menu id
// menu items are added to menus via that addMenuItem(menu, menuItemText, menuItemId, accelerator, checkGroup) console command
// each menu item is added with a menu item id and an optional accelerator
// menu items are initially enabled, but can be disabled/re-enabled via the setMenuItemEnable(menu,menuItem,bool)
// menu text can be set via the setMenuText(menu, newMenuText) console method
// menu item text can be set via the setMenuItemText console method
// menu items can be removed via the removeMenuItem(menu, menuItem) console command
// menu items can be cleared via the clearMenuItems(menu) console command
// menus can be hidden or shown via the setMenuVisible(menu, bool) console command
// menu items can be hidden or shown via the setMenuItemVisible(menu, menuItem, bool) console command
// menu items can be check'd via the setMenuItemChecked(menu, menuItem, bool) console command
//    if the bool is true, any other items in that menu item's check group become unchecked.
//
// menu items can have a bitmap set on them via the setMenuItemBitmap(menu, menuItem, bitmapIndex)
//    passing -1 for the bitmap index will result in no bitmap being shown
//    the index paramater is an index into the bitmap array of the associated profile
//    this can be used, for example, to display a check next to a selected menu item
//    bitmap indices are actually multiplied by 3 when indexing into the bitmap
//    since bitmaps have normal, selected and disabled states.
//
// menus can be removed via the removeMenu console command
// specification arguments for menus and menu items can be either the id or the text of the menu or menu item
// adding the menu item "-" will add an un-selectable seperator to the menu
// callbacks:
// when a menu is clicked, before it is displayed, the menu calls its onMenuSelect(menuId, menuText) method -
//    this allows the callback to enable/disable menu items, or add menu items in a context-sensitive way
// when a menu item is clicked, the menu removes itself from display, then calls onMenuItemSelect(menuId, menuText, menuItemId, menuItemText)

// the initial implementation does not support:
//    hierarchal menus
//    keyboard accelerators on menu text (i.e. via alt-key combos)

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(GuiMenuBar);

//------------------------------------------------------------------------------
// menu management methods
//------------------------------------------------------------------------------

void GuiMenuBar::addMenu(const char *menuText, U32 menuId)
{
   // allocate the menu
   Menu *newMenu = new Menu;
   newMenu->text = dStrdup(menuText);
   newMenu->id = menuId;
   newMenu->nextMenu = NULL;
   newMenu->firstMenuItem = NULL;
   newMenu->visible = true;

   // Menu bitmap variables
   newMenu->bitmapIndex = -1;
   newMenu->drawBitmapOnly = false;
   newMenu->drawBorder = true;
   
   // add it to the menu list
   menuBarDirty = true;
   Menu **walk;
    for(walk = &menuList; *walk; walk = &(*walk)->nextMenu)
      ;
   *walk = newMenu;
}

GuiMenuBar::Menu *GuiMenuBar::findMenu(const char *menu)
{
    if(dIsdigit(menu[0]))
    {
        U32 id = dAtoi(menu);
        for(Menu *walk = menuList; walk; walk = walk->nextMenu)
            if(id == walk->id)
                return walk;
        return NULL;
    }
    else
    {
        for(Menu *walk = menuList; walk; walk = walk->nextMenu)
            if(!dStricmp(menu, walk->text))
                return walk;
        return NULL;
    }
}

GuiMenuBar::MenuItem *GuiMenuBar::findMenuItem(Menu *menu, const char *menuItem)
{
   if(dIsdigit(menuItem[0]))
   {
      U32 id = dAtoi(menuItem);
      for(MenuItem *walk = menu->firstMenuItem; walk; walk = walk->nextMenuItem)
         if(id == walk->id)
            return walk;
      return NULL;
   }
   else
   {
      for(MenuItem *walk = menu->firstMenuItem; walk; walk = walk->nextMenuItem)
         if(!dStricmp(menuItem, walk->text))
            return walk;
      return NULL;
   }
}

void GuiMenuBar::removeMenu(Menu *menu)
{
   menuBarDirty = true;
   clearMenuItems(menu);
   for(Menu **walk = &menuList; *walk; walk = &(*walk)->nextMenu)
   {
      if(*walk == menu)
      {
         *walk = menu->nextMenu;
         break;
      }
   }
   dFree(menu->text);
   delete menu;
}

void GuiMenuBar::removeMenuItem(Menu *menu, MenuItem *menuItem)
{
   for(MenuItem **walk = &menu->firstMenuItem; *walk; walk = &(*walk)->nextMenuItem)
   {
      if(*walk == menuItem)
      {
         *walk = menuItem->nextMenuItem;
         break;
      }
   }

   // DAW: If this is a submenu, then be sure to clear the submenu's items
   if(menuItem->isSubmenu)
   {
      clearSubmenuItems(menuItem);
   }

   dFree(menuItem->text);
   dFree(menuItem->accelerator);
   delete menuItem;
}

void GuiMenuBar::addMenuItem(Menu *menu, const char *text, U32 id, const char *accelerator, S32 checkGroup)
{
   // allocate the new menu item
   MenuItem *newMenuItem = new MenuItem;
   newMenuItem->text = dStrdup(text);
   if(accelerator[0])
      newMenuItem->accelerator = dStrdup(accelerator);
   else
      newMenuItem->accelerator = NULL;
   newMenuItem->id = id;
   newMenuItem->checkGroup = checkGroup;
   newMenuItem->nextMenuItem = NULL;
   newMenuItem->acceleratorIndex = 0;
   newMenuItem->enabled = text[0] != '-';
   newMenuItem->visible = true;
   newMenuItem->bitmapIndex = -1;

   // DAW: Default to not having a submenu
   newMenuItem->isSubmenu = false;
   newMenuItem->firstSubmenuItem = NULL;
   newMenuItem->submenuParentMenu = NULL;

   // link it into the menu's menu item list
   MenuItem **walk = &menu->firstMenuItem;
   while(*walk)
      walk = &(*walk)->nextMenuItem;
   *walk = newMenuItem;

}

void GuiMenuBar::clearMenuItems(Menu *menu)
{
   while(menu->firstMenuItem)
      removeMenuItem(menu, menu->firstMenuItem);
}

void GuiMenuBar::clearMenus()
{
   while(menuList)
      removeMenu(menuList);
}

//------------------------------------------------------------------------------
// DAW: Submenu methods
//------------------------------------------------------------------------------

// DAW: This method will return the MenuItem class of of a submenu's menu item given
// its parent menu and parent menuitem.  If the menuitem ID is used, then the submenu
// ID must also be used.
GuiMenuBar::MenuItem *GuiMenuBar::findSubmenuItem(Menu *menu, const char *menuItem, const char *submenuItem)
{
   if(dIsdigit(menuItem[0]))
   {
      // DAW: Search by ID
      U32 id = dAtoi(menuItem);
      for(MenuItem *walk = menu->firstMenuItem; walk; walk = walk->nextMenuItem)
         if(id == walk->id)
         {
            if(walk->isSubmenu)
            {
               U32 subid = dAtoi(submenuItem);
               for(MenuItem *subwalk = walk->firstSubmenuItem; subwalk; subwalk = subwalk->nextMenuItem)
               {
                  if(subid == walk->id)
                  {
                     return subwalk;
                  }
               }
            }
            return NULL;
         }
      return NULL;
   }
   else
   {
      // DAW: Search by name
      for(MenuItem *walk = menu->firstMenuItem; walk; walk = walk->nextMenuItem)
         if(!dStricmp(menuItem, walk->text))
         {
            if(walk->isSubmenu)
            {
               for(MenuItem *subwalk = walk->firstSubmenuItem; subwalk; subwalk = subwalk->nextMenuItem)
               {
                  if(!dStricmp(submenuItem, subwalk->text))
                      return subwalk;
               }
            }
            return NULL;
         }
      return NULL;
   }
}

// DAW: Add a menuitem to the given submenu
void GuiMenuBar::addSubmenuItem(Menu *menu, MenuItem *submenu, const char *text, U32 id, const char *accelerator, S32 checkGroup)
{
   // Check that the given menu item supports a submenu
   if(submenu && !submenu->isSubmenu)
   {
      Con::errorf("GuiMenuBar::addSubmenuItem: Attempting to add menuitem '%s' to an invalid submenu",text);
      return;
   }

   // allocate the new menu item
   MenuItem *newMenuItem = new MenuItem;
   newMenuItem->text = dStrdup(text);
   if(accelerator[0])
      newMenuItem->accelerator = dStrdup(accelerator);
   else
      newMenuItem->accelerator = NULL;
   newMenuItem->id = id;
   newMenuItem->checkGroup = checkGroup;
   newMenuItem->nextMenuItem = NULL;
   newMenuItem->acceleratorIndex = 0;
   newMenuItem->enabled = text[0] != '-';
   newMenuItem->visible = true;
   newMenuItem->bitmapIndex = -1;

   // DAW: Default to not having a submenu
   newMenuItem->isSubmenu = false;
   newMenuItem->firstSubmenuItem = NULL;

   // DAW: Point back to the submenu's menu
   newMenuItem->submenuParentMenu = menu;

   // link it into the menu's menu item list
   MenuItem **walk = &submenu->firstSubmenuItem;
   while(*walk)
      walk = &(*walk)->nextMenuItem;
   *walk = newMenuItem;

}

// DAW: Remove a submenu item
void GuiMenuBar::removeSubmenuItem(MenuItem *menuItem, MenuItem *submenuItem)
{
   // Check that the given menu item supports a submenu
   if(menuItem && !menuItem->isSubmenu)
   {
      Con::errorf("GuiMenuBar::removeSubmenuItem: Attempting to remove submenuitem '%s' from an invalid submenu",submenuItem->text);
      return;
   }

   for(MenuItem **subwalk = &menuItem->firstSubmenuItem; *subwalk; subwalk = &(*subwalk)->nextMenuItem)
   {
      if(*subwalk == submenuItem)
      {
         *subwalk = submenuItem->nextMenuItem;
         break;
      }
   }
   dFree(submenuItem->text);
   dFree(submenuItem->accelerator);
   delete submenuItem;
}

// DAW: Clear all menuitems from a submenu
void GuiMenuBar::clearSubmenuItems(MenuItem *menuitem)
{
   // Check that the given menu item supports a submenu
   if(menuitem && !menuitem->isSubmenu)
   {
      Con::errorf("GuiMenuBar::clearSubmenuItems: Attempting to clear an invalid submenu");
      return;
   }

   while(menuitem->firstSubmenuItem)
      removeSubmenuItem(menuitem, menuitem->firstSubmenuItem);
}

//------------------------------------------------------------------------------
// initialization, input and render methods
//------------------------------------------------------------------------------

GuiMenuBar::GuiMenuBar()
{
    menuList = NULL;
   menuBarDirty = true;
   mouseDownMenu = NULL;
   mouseOverMenu = NULL;
   mCurAcceleratorIndex = 0;
   mBackground = NULL;
   mPadding = 0;

   mCheckmarkBitmapIndex = 0; // Default to the first image in the bitmap array for the check mark

   mHorizontalMargin = 6; // Default number of pixels on the left and right side of a manu's text
   mVerticalMargin = 1;   // Default number of pixels on the top and bottom of a menu's text
   mBitmapMargin = 2;     // Default number of pixels between a menu's bitmap and text

   // DAW: Added:
   mouseDownSubmenu = NULL;
   mouseOverSubmenu = NULL;
   mSubmenuBackground = NULL;
   mSubmenuTextList = NULL;
   mMouseOverCounter = 0;
   mCountMouseOver = false;
   mMouseHoverAmount = 30;
   setProcessTicks(false);
}

void GuiMenuBar::initPersistFields()
{
   Parent::initPersistFields();

   addField("Padding", TypeS32, Offset( mPadding, GuiMenuBar ) );
}

bool GuiMenuBar::onWake()
{
   if(!Parent::onWake())
      return false;
   mProfile->constructBitmapArray();  // if a bitmap was specified...
   maxBitmapSize.set(0,0);
   S32 numBitmaps = mProfile->mBitmapArrayRects.size();
   if(numBitmaps)
   {
      RectI *bitmapBounds = mProfile->mBitmapArrayRects.address();
      for(S32 i = 0; i < numBitmaps; i++)
      {
         if(bitmapBounds[i].extent.x > maxBitmapSize.x)
            maxBitmapSize.x = bitmapBounds[i].extent.x;
         if(bitmapBounds[i].extent.y > maxBitmapSize.y)
            maxBitmapSize.y = bitmapBounds[i].extent.y;
      }
   }
   return true;
}

GuiMenuBar::Menu *GuiMenuBar::findHitMenu(Point2I mousePoint)
{
   Point2I pos = globalToLocalCoord(mousePoint);

   for(Menu *walk = menuList; walk; walk = walk->nextMenu)
      if(walk->visible && walk->bounds.pointInRect(pos))
         return walk;
   return NULL;
}

void GuiMenuBar::onPreRender()
{
   Parent::onPreRender();
   if(menuBarDirty)
   {
      menuBarDirty = false;
      U32 curX = mPadding;
      for(Menu *walk = menuList; walk; walk = walk->nextMenu)
      {
         if(!walk->visible)
            continue;

         // Bounds depends on if there is a bitmap to be drawn or not
         if(walk->bitmapIndex == -1)
         {
            // Text only
            walk->bounds.set(curX, 0, mProfile->mFont->getStrWidth(walk->text) + (mHorizontalMargin * 2), mBounds.extent.y - (mVerticalMargin * 2));

         } else
         {
            // Will the bitmap and text be draw?
            if(!walk->drawBitmapOnly)
            {
               // Draw the bitmap and the text
               RectI *bitmapBounds = mProfile->mBitmapArrayRects.address();
               walk->bounds.set(curX, 0, bitmapBounds[walk->bitmapIndex].extent.x + mProfile->mFont->getStrWidth(walk->text) + (mHorizontalMargin * 2), mBounds.extent.y + (mVerticalMargin * 2));

            } else
            {
               // Only the bitmap will be drawn
               RectI *bitmapBounds = mProfile->mBitmapArrayRects.address();
               walk->bounds.set(curX, 0, bitmapBounds[walk->bitmapIndex].extent.x + mBitmapMargin + (mHorizontalMargin * 2), mBounds.extent.y + (mVerticalMargin * 2));
            }
         }

         curX += walk->bounds.extent.x;
      }
        mouseOverMenu = NULL;
        mouseDownMenu = NULL;
   }
}

void GuiMenuBar::checkMenuMouseMove(const GuiEvent &event)
{
   Menu *hit = findHitMenu(event.mousePoint);
   if(hit && hit != mouseDownMenu)
   {
      // gotta close out the current menu...
      mTextList->setSelectedCell(Point2I(-1, -1));
      closeMenu();
      mouseOverMenu = mouseDownMenu = hit;
      setUpdate();
      onAction();
   }
}

void GuiMenuBar::onMouseMove(const GuiEvent &event)
{
   Menu *hit = findHitMenu(event.mousePoint);
    if(hit != mouseOverMenu)
    {
        // DAW: If we need to, reset the mouse over menu counter and indicate
        // that we should track it.
        if(hit)
           mMouseOverCounter = 0;
        if(!mCountMouseOver)
        {
           // DAW: We've never started the counter, so start it.
           if(hit)
              mCountMouseOver = true;
        }

        mouseOverMenu = hit;
        setUpdate();
    }
}

void GuiMenuBar::onMouseLeave(const GuiEvent &event)
{
   if(mouseOverMenu)
        setUpdate();
    mouseOverMenu = NULL;

   // DAW: As we've left the control, don't track how long the mouse has been
   // within it.
   if(mCountMouseOver && mMouseOverCounter >= mMouseHoverAmount)
   {
      Con::executef( this, 3, "onMouseInMenu", "0"); // Last parameter indicates if we've entered or left the menu
   }
   mCountMouseOver = false;
   mMouseOverCounter = 0;
}

void GuiMenuBar::onMouseDragged(const GuiEvent &event)
{
   Menu *hit = findHitMenu(event.mousePoint);
    
    if(hit != mouseOverMenu)
    {
        // DAW: If we need to, reset the mouse over menu counter and indicate
        // that we should track it.
        if(hit)
           mMouseOverCounter = 0;
        if(!mCountMouseOver)
        {
           // DAW: We've never started the counter, so start it.
           if(hit)
              mCountMouseOver = true;
        }

        mouseOverMenu = hit;
      mouseDownMenu = hit;
        setUpdate();
      onAction();
    }
}

void GuiMenuBar::onMouseDown(const GuiEvent &event)
{
   mouseDownMenu = mouseOverMenu = findHitMenu(event.mousePoint);
    setUpdate();
   onAction();
}

void GuiMenuBar::onMouseUp(const GuiEvent &event)
{
   mouseDownMenu = NULL;
    setUpdate();
}

void GuiMenuBar::onRender(Point2I offset, const RectI &updateRect)
{

   RectI ctrlRect(offset, mBounds.extent);

   //if opaque, fill the update rect with the fill color
   if (mProfile->mOpaque)
      dglDrawRectFill(RectI(offset, mBounds.extent), mProfile->mFillColor);

   //if there's a border, draw the border
   if (mProfile->mBorder)
      renderBorder(ctrlRect, mProfile);

   for(Menu *walk = menuList; walk; walk = walk->nextMenu)
   {
      if(!walk->visible)
         continue;
      ColorI fontColor = mProfile->mFontColor;
      RectI bounds = walk->bounds;
      bounds.point += offset;
      
      Point2I start;

      start.x = walk->bounds.point.x + mHorizontalMargin;
      start.y = walk->bounds.point.y + ( walk->bounds.extent.y - mProfile->mFont->getHeight() ) / 2;

      // Draw the border
      if(walk->drawBorder)
      {
        RectI highlightBounds = bounds;
        highlightBounds.inset(1,1);
         if(walk == mouseDownMenu)
            renderFilledBorder(highlightBounds, mProfile->mBorderColorHL, mProfile->mFillColorHL );
         else if(walk == mouseOverMenu && mouseDownMenu == NULL)
            renderFilledBorder(highlightBounds, mProfile->mBorderColor, mProfile->mFillColor );
      }

      // Do we draw a bitmap?
      if(walk->bitmapIndex != -1)
      {
         S32 index = walk->bitmapIndex * 3;
         if(walk == mouseDownMenu)
            ++index;
         else if(walk == mouseOverMenu && mouseDownMenu == NULL)
            index += 2;

         RectI rect = mProfile->mBitmapArrayRects[index];

         Point2I bitmapstart(start);
         bitmapstart.y = walk->bounds.point.y + ( walk->bounds.extent.y - rect.extent.y ) / 2;

         dglClearBitmapModulation();
         dglDrawBitmapSR(mProfile->mTextureHandle, offset + bitmapstart, rect);

         // Should we also draw the text?
         if(!walk->drawBitmapOnly)
         {
            start.x += mBitmapMargin;
            dglSetBitmapModulation( fontColor );
            dglDrawText( mProfile->mFont, start + offset, walk->text, mProfile->mFontColors );
         }
      } else
      {
         dglSetBitmapModulation( fontColor );
         dglDrawText( mProfile->mFont, start + offset, walk->text, mProfile->mFontColors );
      }
   }

   renderChildControls( offset, updateRect );
}

void GuiMenuBar::buildAcceleratorMap()
{
   Parent::buildAcceleratorMap();
   // ok, accelerator map is cleared...
   // add all our keys:
   mCurAcceleratorIndex = 1;

   for(Menu *menu = menuList; menu; menu = menu->nextMenu)
   {
      for(MenuItem *item = menu->firstMenuItem; item; item = item->nextMenuItem)
      {
         if(!item->accelerator)
         {
            item->accelerator = 0;
            continue;
         }
         EventDescriptor accelEvent;
            ActionMap::createEventDescriptor(item->accelerator, &accelEvent);
   
         //now we have a modifier, and a key, add them to the canvas
         GuiCanvas *root = getRoot();
         if (root)
            root->addAcceleratorKey(this, mCurAcceleratorIndex, accelEvent.eventCode, accelEvent.flags);
         item->acceleratorIndex = mCurAcceleratorIndex;
         mCurAcceleratorIndex++;
      }
   }
}

void GuiMenuBar::acceleratorKeyPress(U32 index)
{
   // loop through all the menus
   // and find the item that corresponds to the accelerator index
   for(Menu *menu = menuList; menu; menu = menu->nextMenu)
   {
      if(!menu->visible)
         continue;

      for(MenuItem *item = menu->firstMenuItem; item; item = item->nextMenuItem)
      {
         if(item->acceleratorIndex == index)
         {
            // first, call the script callback for menu selection:
            Con::executef( this, 4, "onMenuSelect", Con::getIntArg(menu->id), 
                        menu->text);
            if(item->visible)
               menuItemSelected(menu, item);
            return;
         }
      }
   }
}

//------------------------------------------------------------------------------
// Menu display class methods
//------------------------------------------------------------------------------

GuiMenuBackgroundCtrl::GuiMenuBackgroundCtrl(GuiMenuBar *ctrl, GuiMenuTextListCtrl *textList)
{
   mMenuBarCtrl = ctrl;
   mTextList = textList;
}

void GuiMenuBackgroundCtrl::onMouseDown(const GuiEvent &event)
{
   mTextList->setSelectedCell(Point2I(-1,-1));
   mMenuBarCtrl->closeMenu();
}

void GuiMenuBackgroundCtrl::onMouseMove(const GuiEvent &event)
{
   GuiCanvas *root = getRoot();
   GuiControl *ctrlHit = root->findHitControl(event.mousePoint, mLayer - 1);
   if(ctrlHit == mMenuBarCtrl)  // see if the current mouse over menu is right...
      mMenuBarCtrl->checkMenuMouseMove(event);
}

void GuiMenuBackgroundCtrl::onMouseDragged(const GuiEvent &event)
{
   GuiCanvas *root = getRoot();
   GuiControl *ctrlHit = root->findHitControl(event.mousePoint, mLayer - 1);
   if(ctrlHit == mMenuBarCtrl)  // see if the current mouse over menu is right...
      mMenuBarCtrl->checkMenuMouseMove(event);
}

GuiMenuTextListCtrl::GuiMenuTextListCtrl(GuiMenuBar *ctrl)
{
   mMenuBarCtrl = ctrl;
   isSubMenu = false; // DAW: Added
}

void GuiMenuTextListCtrl::onRenderCell(Point2I offset, Point2I cell, bool selected, bool mouseOver)
{
   if(dStrcmp(mList[cell.y].text + 3, "-\t")) // DAW: Was: dStrcmp(mList[cell.y].text + 2, "-\t")) but has been changed to take into account the submenu flag
      Parent::onRenderCell(offset, cell, selected, mouseOver);
   else
   {
      S32 yp = offset.y + mCellSize.y / 2;
      dglDrawLine(offset.x, yp, offset.x + mCellSize.x, yp, ColorI(128,128,128));
      dglDrawLine(offset.x, yp+1, offset.x + mCellSize.x, yp+1, ColorI(255,255,255));
   }
   // now see if there's a bitmap...
   U8 idx = mList[cell.y].text[0];
   if(idx != 1)
   {
      // there's a bitmap...
      U32 index = U32(idx - 2) * 3;
      if(!mList[cell.y].active)
         index += 2;
      else if(selected || mouseOver)
         index ++;

      RectI rect = mProfile->mBitmapArrayRects[index];
      Point2I off = mMenuBarCtrl->maxBitmapSize - rect.extent;
      off /= 2;

      dglClearBitmapModulation();
      dglDrawBitmapSR(mProfile->mTextureHandle, offset + off, rect);

   } 

   // DAW: Check if this is a submenu
   idx = mList[cell.y].text[1];
   if(idx != 1)
   {
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
// PUAP -Mat untested	
//How are these used/made? cannot create in TGB GUI editor
       if(selected || mouseOver)
       {
           glColor4f(mProfile->mFontColorHL.red,mProfile->mFontColorHL.green,mProfile->mFontColorHL.blue, 255 );//full alpha
       }
            else
       {
           glColor4f(mProfile->mFontColor.red,mProfile->mFontColor.green,mProfile->mFontColor.blue, 255);
       }
       glDrawArrays(GL_TRIANGLES, 0, 3 );
#else
      // This is a submenu, so draw an arrow
      F32 left = (F32)(offset.x + mCellSize.x - 12);
      F32 right = (F32)(left + 8);
      F32 top = (F32)(mCellSize.y / 2 + offset.y - 4);
      F32 bottom = (F32)(top + 8);
      F32 middle = (F32)(top + 4);
      /*
      glBegin(GL_TRIANGLES);
         if(selected || mouseOver)
         {
            glColor3ub(mProfile->mFontColorHL.red,mProfile->mFontColorHL.green,mProfile->mFontColorHL.blue);

         } else
         {
            glColor3ub(mProfile->mFontColor.red,mProfile->mFontColor.green,mProfile->mFontColor.blue);
         }
         glVertex2fv( Point3F(left, top, 0.0f) );
         glVertex2fv( Point3F(right, middle, 0.0f) );
         glVertex2fv( Point3F(left, bottom, 0.0f) );
      glEnd();
      */
#endif
   }
}

bool GuiMenuTextListCtrl::onKeyDown(const GuiEvent &event)
{
   //if the control is a dead end, don't process the input:
   if ( !mVisible || !mActive || !mAwake )
      return false;
   
   //see if the key down is a <return> or not
   if ( event.modifier == 0 )
   {
      if ( event.keyCode == KEY_RETURN )
      {
         mMenuBarCtrl->closeMenu();
         return true;
      }
      else if ( event.keyCode == KEY_ESCAPE )
      {
         mSelectedCell.set( -1, -1 );
         mMenuBarCtrl->closeMenu();
         return true;
      }
   }
   
   //otherwise, pass the event to it's parent
   return Parent::onKeyDown(event);
}

void GuiMenuTextListCtrl::onMouseDown(const GuiEvent &event)
{
   Parent::onMouseDown(event);
   mMenuBarCtrl->closeMenu();
}

void GuiMenuTextListCtrl::onMouseUp(const GuiEvent &event)
{
   // ok, this is kind of strange... but!
   // here's the deal: if we get a mouse up in this control
   // it means the mouse was dragged from the initial menu mouse click
   // so: activate the menu result as though this event were,
   // instead, a mouse down.

   onMouseDown(event);
}

void GuiMenuTextListCtrl::onCellHighlighted(Point2I cell)
{
    // If this text list control is part of a submenu, then don't worry about
    // passing this along
    if(!isSubMenu)
    {
        RectI globalbounds(mBounds);
        Point2I globalpoint = localToGlobalCoord(globalbounds.point);
        globalbounds.point = globalpoint;
        mMenuBarCtrl->highlightedMenuItem(cell.y, globalbounds, mCellSize);
    }
}

//------------------------------------------------------------------------------
// Submenu display class methods
//------------------------------------------------------------------------------

GuiSubmenuBackgroundCtrl::GuiSubmenuBackgroundCtrl(GuiMenuBar *ctrl, GuiMenuTextListCtrl *textList) : GuiMenuBackgroundCtrl(ctrl, textList)
{
}

void GuiSubmenuBackgroundCtrl::onMouseDown(const GuiEvent &event)
{
   mTextList->setSelectedCell(Point2I(-1,-1));
   mMenuBarCtrl->closeMenu();
}

bool GuiSubmenuBackgroundCtrl::pointInControl(const Point2I& parentCoordPoint)
{
   S32 xt = parentCoordPoint.x - mBounds.point.x;
   S32 yt = parentCoordPoint.y - mBounds.point.y;

   if(findHitControl(Point2I(xt,yt)) == this)
       return false;
   else
       return true;
//   return xt >= 0 && yt >= 0 && xt < mBounds.extent.x && yt < mBounds.extent.y;
}

//------------------------------------------------------------------------------

void GuiMenuBar::menuItemSelected(GuiMenuBar::Menu *menu, GuiMenuBar::MenuItem *item)
{
   if(item->enabled)
      Con::executef( this, 6, "onMenuItemSelect", Con::getIntArg(menu->id), 
               menu->text, Con::getIntArg(item->id), item->text);
}

void GuiMenuBar::onSleep()
{
   if(mBackground) // a menu is up?
   {
      mTextList->setSelectedCell(Point2I(-1, -1));
      closeMenu();
   }
   Parent::onSleep();
}

void GuiMenuBar::closeMenu()
{
   // DAW: First close any open submenu
    closeSubmenu();

   // Get the selection from the text list:
   S32 selectionIndex = mTextList->getSelectedCell().y;

   // Pop the background:
   if( getRoot() )
      getRoot()->popDialogControl(mBackground);
   else
      return;
   
   // Kill the popup:
   mBackground->deleteObject();
   mBackground = NULL;
   
   // Now perform the popup action:
   if ( selectionIndex != -1 )
   {
      MenuItem *list = mouseDownMenu->firstMenuItem;

      while(selectionIndex && list)
      {
         list = list->nextMenuItem;
         selectionIndex--;
      }
      if(list)
         menuItemSelected(mouseDownMenu, list);
   }
   mouseDownMenu = NULL;
}

// DAW: Called when a menu item is highlighted by the mouse
void GuiMenuBar::highlightedMenuItem(S32 selectionIndex, RectI bounds, Point2I cellSize)
{
   S32 selstore = selectionIndex;

   // Now perform the popup action:
   if ( selectionIndex != -1 )
   {
      MenuItem *list = mouseDownMenu->firstMenuItem;

      while(selectionIndex && list)
      {
         list = list->nextMenuItem;
         selectionIndex--;
      }

      if(list)
      {
         // If the highlighted item has changed...
         if(mouseOverSubmenu != list)
         {
            closeSubmenu();
            mouseOverSubmenu = NULL;

            // Check if this is a submenu.  If so, open the submenu.
            if(list->isSubmenu)
            {
               // If there are submenu items, then open the submenu
               if(list->firstSubmenuItem)
               {
                   mouseOverSubmenu = list;
                   onSubmenuAction(selstore, bounds, cellSize);
               }
            }
         }
      }
   }
}

//------------------------------------------------------------------------------
void GuiMenuBar::onAction()
{
   if(!mouseDownMenu)
      return;

   // first, call the script callback for menu selection:
   Con::executef( this, 4, "onMenuSelect", Con::getIntArg(mouseDownMenu->id), 
               mouseDownMenu->text);

   MenuItem *visWalk = mouseDownMenu->firstMenuItem;
   while(visWalk)
   {
      if(visWalk->visible)
         break;
      visWalk = visWalk->nextMenuItem;
   }
   if(!visWalk)
   {
      mouseDownMenu = NULL;
      return;
   }

   mTextList = new GuiMenuTextListCtrl(this);
   mTextList->mProfile = mProfile;

   mBackground = new GuiMenuBackgroundCtrl(this, mTextList);

   GuiCanvas *root = getRoot();
   Point2I windowExt = root->mBounds.extent;

   mBackground->mBounds.point.set(0,0);
   mBackground->mBounds.extent = root->mBounds.extent;

   S32 textWidth = 0, width = 0;
   S32 acceleratorWidth = 0;

   GFont *font = mProfile->mFont;

   for(MenuItem *walk = mouseDownMenu->firstMenuItem; walk; walk = walk->nextMenuItem)
   {
      if(!walk->visible)
         continue;

      S32 iTextWidth = font->getStrWidth(walk->text);
      S32 iAcceleratorWidth = walk->accelerator ? font->getStrWidth(walk->accelerator) : 0;

      if(iTextWidth > textWidth)
         textWidth = iTextWidth;
      if(iAcceleratorWidth > acceleratorWidth)
         acceleratorWidth = iAcceleratorWidth;
   }
   width = textWidth + acceleratorWidth + maxBitmapSize.x * 2 + 2 + 4;

   mTextList->setCellSize(Point2I(width, font->getHeight()+2));
   mTextList->clearColumnOffsets();
   mTextList->addColumnOffset(-1); // add an empty column in for the bitmap index.
   mTextList->addColumnOffset(maxBitmapSize.x + 1);
   mTextList->addColumnOffset(maxBitmapSize.x + 1 + textWidth + 4);

   U32 entryCount = 0;

   for(MenuItem *walk = mouseDownMenu->firstMenuItem; walk; walk = walk->nextMenuItem)
   {
      if(!walk->visible)
         continue;

      char buf[512];

      // DAW: If this menu item is a submenu, then set the isSubmenu to 2 to indicate
      // an arrow should be drawn.  Otherwise set the isSubmenu normally.
      char isSubmenu = 1;
      if(walk->isSubmenu)
          isSubmenu = 2;

      char bitmapIndex = 1;
      if(walk->bitmapIndex >= 0 && (walk->bitmapIndex * 3 <= mProfile->mBitmapArrayRects.size()))
         bitmapIndex = walk->bitmapIndex + 2;
      dSprintf(buf, sizeof(buf), "%c%c\t%s\t%s", bitmapIndex, isSubmenu, walk->text, walk->accelerator ? walk->accelerator : "");
      mTextList->addEntry(entryCount, buf);

      if(!walk->enabled)
         mTextList->setEntryActive(entryCount, false);

      entryCount++;
   }
   Point2I menuPoint = localToGlobalCoord(mouseDownMenu->bounds.point);
   menuPoint.y += mouseDownMenu->bounds.extent.y; // DAW: Used to have this at the end: + 2;

   GuiControl *ctrl = new GuiControl;
   ctrl->mBounds.point = menuPoint;
   ctrl->mBounds.extent = mTextList->mBounds.extent + Point2I(6, 6);
   ctrl->mProfile = mProfile;
   mTextList->mBounds.point += Point2I(3,3);

   // DAW: Make sure the menu doesn't go beyond the Canvas' bottom edge.
   if((ctrl->mBounds.point.y+ctrl->mBounds.extent.y) > windowExt.y)
   {
      // DAW: Pop the menu above the menu bar
      Point2I menuBar = localToGlobalCoord(mouseDownMenu->bounds.point);
      ctrl->mBounds.point.y = menuBar.y - ctrl->mBounds.extent.y;
   }

   //mTextList->mBounds.point = Point2I(3,3);

   mTextList->registerObject();
   mBackground->registerObject();
   ctrl->registerObject();

   mBackground->addObject( ctrl );
   ctrl->addObject( mTextList );

   root->pushDialogControl(mBackground, mLayer + 1);
   mTextList->setFirstResponder();
}

//------------------------------------------------------------------------------
// DAW: Performs an action when a menu item that is a submenu is selected/highlighted
void GuiMenuBar::onSubmenuAction(S32 selectionIndex, RectI bounds, Point2I cellSize)
{
   if(!mouseOverSubmenu)
      return;

   // first, call the script callback for menu selection:
   Con::executef( this, 4, "onSubmenuSelect", Con::getIntArg(mouseOverSubmenu->id), 
               mouseOverSubmenu->text);

   MenuItem *visWalk = mouseOverSubmenu->firstSubmenuItem;
   while(visWalk)
   {
      if(visWalk->visible)
         break;
      visWalk = visWalk->nextMenuItem;
   }
   if(!visWalk)
   {
      mouseOverSubmenu = NULL;
      return;
   }

   mSubmenuTextList = new GuiMenuTextListCtrl(this);
   mSubmenuTextList->mProfile = mProfile;
   mSubmenuTextList->isSubMenu = true; // Indicate that this text list is part of a submenu

   mSubmenuBackground = new GuiSubmenuBackgroundCtrl(this, mSubmenuTextList);

   GuiCanvas *root = getRoot();
   Point2I windowExt = root->mBounds.extent;

   mSubmenuBackground->mBounds.point.set(0,0);
   mSubmenuBackground->mBounds.extent = root->mBounds.extent;

   S32 textWidth = 0, width = 0;
   S32 acceleratorWidth = 0;

   GFont *font = mProfile->mFont;

   for(MenuItem *walk = mouseOverSubmenu->firstSubmenuItem; walk; walk = walk->nextMenuItem)
   {
      if(!walk->visible)
         continue;

      S32 iTextWidth = font->getStrWidth(walk->text);
      S32 iAcceleratorWidth = walk->accelerator ? font->getStrWidth(walk->accelerator) : 0;

      if(iTextWidth > textWidth)
         textWidth = iTextWidth;
      if(iAcceleratorWidth > acceleratorWidth)
         acceleratorWidth = iAcceleratorWidth;
   }
   width = textWidth + acceleratorWidth + maxBitmapSize.x * 2 + 2 + 4;

   mSubmenuTextList->setCellSize(Point2I(width, font->getHeight()+3));
   mSubmenuTextList->clearColumnOffsets();
   mSubmenuTextList->addColumnOffset(-1); // add an empty column in for the bitmap index.
   mSubmenuTextList->addColumnOffset(maxBitmapSize.x + 1);
   mSubmenuTextList->addColumnOffset(maxBitmapSize.x + 1 + textWidth + 4);

   U32 entryCount = 0;

   for(MenuItem *walk = mouseOverSubmenu->firstSubmenuItem; walk; walk = walk->nextMenuItem)
   {
      if(!walk->visible)
         continue;

      char buf[512];

      // DAW: Can't have submenus within submenus.
      char isSubmenu = 1;

      char bitmapIndex = 1;
      if(walk->bitmapIndex >= 0 && (walk->bitmapIndex * 3 <= mProfile->mBitmapArrayRects.size()))
         bitmapIndex = walk->bitmapIndex + 2;
      dSprintf(buf, sizeof(buf), "%c%c\t%s\t%s", bitmapIndex, isSubmenu, walk->text, walk->accelerator ? walk->accelerator : "");
      mSubmenuTextList->addEntry(entryCount, buf);

      if(!walk->enabled)
         mSubmenuTextList->setEntryActive(entryCount, false);

      entryCount++;
   }
   Point2I menuPoint = bounds.point; //localToGlobalCoord(bounds.point);
   menuPoint.x += bounds.extent.x;
   menuPoint.y += cellSize.y * selectionIndex - 6;

   GuiControl *ctrl = new GuiControl;
   ctrl->mBounds.point = menuPoint;
   ctrl->mBounds.extent = mSubmenuTextList->mBounds.extent + Point2I(6, 6);
   ctrl->mProfile = mProfile;
   mSubmenuTextList->mBounds.point += Point2I(3,3);

   // DAW: Make sure the menu doesn't go beyond the Canvas' bottom edge.
   if((ctrl->mBounds.point.y+ctrl->mBounds.extent.y) > windowExt.y)
   {
      // DAW: Pop the menu above the menu bar
      ctrl->mBounds.point.y -= mSubmenuTextList->mBounds.extent.y - cellSize.y - 6 - 3;
   }

   // DAW: And the same for the right edge
   if((ctrl->mBounds.point.x+ctrl->mBounds.extent.x) > windowExt.x)
   {
      // DAW: Pop the submenu to the left of the menu
      ctrl->mBounds.point.x -= mSubmenuTextList->mBounds.extent.x + cellSize.x + 6;
   }

   //mSubmenuTextList->mBounds.point = Point2I(3,3);

   mSubmenuTextList->registerObject();
   mSubmenuBackground->registerObject();
   ctrl->registerObject();

   mSubmenuBackground->addObject( ctrl );
   ctrl->addObject( mSubmenuTextList );

   root->pushDialogControl(mSubmenuBackground, mLayer + 1);
   mSubmenuTextList->setFirstResponder();
}

// DAW: Close down the submenu controls
void GuiMenuBar::closeSubmenu()
{
   if(!mSubmenuBackground || !mSubmenuTextList)
       return;

   // Get the selection from the text list:
   S32 selectionIndex = mSubmenuTextList->getSelectedCell().y;

   // Pop the background:
   if( getRoot() )
      getRoot()->popDialogControl(mSubmenuBackground);
   
   // Kill the popup:
   mSubmenuBackground->deleteObject();
   mSubmenuBackground = NULL;
   mSubmenuTextList = NULL;
   
   // Now perform the popup action:
   if ( selectionIndex != -1 )
   {
      MenuItem *list = NULL;
      if(mouseOverSubmenu)
      {
         list = mouseOverSubmenu->firstSubmenuItem;

         while(selectionIndex && list)
         {
            list = list->nextMenuItem;
            selectionIndex--;
         }
      }
      if(list)
         menuItemSelected(list->submenuParentMenu, list);
   }
   mouseOverSubmenu = NULL;
}

// DAW: Find if the mouse pointer is within a menu item
GuiMenuBar::MenuItem *GuiMenuBar::findHitMenuItem(Point2I mousePoint)
{
   //Point2I pos = globalToLocalCoord(mousePoint);

//   for(Menu *walk = menuList; walk; walk = walk->nextMenu)
//      if(walk->visible && walk->bounds.pointInRect(pos))
//         return walk;
   return NULL;
}

// DAW: Checks if the mouse has been moved to a new menu item
void GuiMenuBar::checkSubmenuMouseMove(const GuiEvent &event)
{
   MenuItem *hit = findHitMenuItem(event.mousePoint);
   if(hit && hit != mouseOverSubmenu)
   {
      // gotta close out the current menu...
      mSubmenuTextList->setSelectedCell(Point2I(-1, -1));
//      closeSubmenu();
      setUpdate();
   }
}

// DAW: Process a tick
void GuiMenuBar::processTick()
{
   // If we are to track a tick, then do so.
   if(mCountMouseOver)
   {
      // DAW: If we're at a particular number of ticks, notify the script function
      if(mMouseOverCounter < mMouseHoverAmount)
      {
         ++mMouseOverCounter;

      } else if(mMouseOverCounter == mMouseHoverAmount)
      {
         ++mMouseOverCounter;
         Con::executef( this, 3, "onMouseInMenu", "1"); // Last parameter indicates if we've entered or left the menu
      }
   }
}
