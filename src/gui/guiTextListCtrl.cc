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

#include "platform/platform.h"
#include "console/consoleTypes.h"
#include "console/console.h"
#include "graphics/dgl.h"
#include "gui/guiTextListCtrl.h"
#include "gui/containers/guiScrollCtrl.h"
#include "gui/guiDefaultControlRender.h"
#include "gui/guiTextListCtrl_Binding.h"

IMPLEMENT_CONOBJECT(GuiTextListCtrl);

static int sortColumn;
static bool sIncreasing;

static const char *getColumn(const char *text)
{
   int ct = sortColumn;
   while(ct--)
   {
      text = dStrchr(text, '\t');
      if(!text)
         return "";
      text++;
   }
   return text;
}

static S32 QSORT_CALLBACK textCompare( const void* a, const void* b )
{
   GuiTextListCtrl::Entry *ea = (GuiTextListCtrl::Entry *) (a);
   GuiTextListCtrl::Entry *eb = (GuiTextListCtrl::Entry *) (b);
   S32 result = dStricmp( getColumn( ea->text ), getColumn( eb->text ) );
   return ( sIncreasing ? result : -result );
}

static S32 QSORT_CALLBACK numCompare(const void *a,const void *b)
{
   GuiTextListCtrl::Entry *ea = (GuiTextListCtrl::Entry *) (a);
   GuiTextListCtrl::Entry *eb = (GuiTextListCtrl::Entry *) (b);
   const char* aCol = getColumn( ea->text );
   const char* bCol = getColumn( eb->text );
   F32 result = dAtof(aCol) - dAtof(bCol);
   S32 res = result < 0 ? -1 : (result > 0 ? 1 : 0);

   return ( sIncreasing ? res : -res );
}

GuiTextListCtrl::GuiTextListCtrl()
{
   VECTOR_SET_ASSOCIATION(mList);
   VECTOR_SET_ASSOCIATION(mColumnOffsets);

   mActive = true;
   mEnumerate = false;
   mResizeCell = true;
   mSize.set(1, 0);
   mColumnOffsets.push_back(0);
   mFitParentWidth = true;
   mClipColumnText = false;
}

void GuiTextListCtrl::initPersistFields()
{
   Parent::initPersistFields();
   addField("enumerate",               TypeBool, Offset(mEnumerate, GuiTextListCtrl));
   addField("resizeCell",              TypeBool, Offset(mResizeCell, GuiTextListCtrl));
   addField("columns",                 TypeS32Vector, Offset(mColumnOffsets, GuiTextListCtrl));
   addField("fitParentWidth",          TypeBool, Offset(mFitParentWidth, GuiTextListCtrl));
   addField("clipColumnText",          TypeBool, Offset(mClipColumnText, GuiTextListCtrl));
}

bool GuiTextListCtrl::onWake()
{
   if(!Parent::onWake())
      return false;

   setSize(mSize);
   return true;
}

U32 GuiTextListCtrl::getSelectedId()
{
   if (mSelectedCell.y == -1)
      return InvalidId;

   return mList[mSelectedCell.y].id;
}

U32 GuiTextListCtrl::getSelectedRow()
{
   return mSelectedCell.y;
}
void GuiTextListCtrl::onCellSelected(Point2I cell)
{
   Con::executef(this, 3, "onSelect", Con::getIntArg(mList[cell.y].id), mList[cell.y].text);

   if (mConsoleCommand[0])
      Con::evaluate(mConsoleCommand, false);
}

void GuiTextListCtrl::onRenderCell(Point2I offset, Point2I cell, bool selected, bool mouseOver)
{
   if ( mList[cell.y].active )
   {
      if (selected || (mProfile->mMouseOverSelected && mouseOver))
      {
         RectI highlightRect = RectI(offset.x, offset.y, mCellSize.x, mCellSize.y);
         highlightRect.inset( 0, -1 );
         renderFilledBorder( highlightRect, mProfile->mBorderColorHL, mProfile->mFillColorHL);
         dglSetBitmapModulation(mProfile->mFontColorHL);
      }
      else
         dglSetBitmapModulation(mouseOver ? mProfile->mFontColorHL : mProfile->mFontColor);
   }
   else
      dglSetBitmapModulation( mProfile->mFontColorNA );

   const char *text = mList[cell.y].text;
   for(U32 index = 0; index < (U32)mColumnOffsets.size(); index++)
   {
      const char *nextCol = dStrchr(text, '\t');
      if(mColumnOffsets[index] >= 0)
      {
         U32 slen;
         if(nextCol)
            slen = (U32)(nextCol - text);
         else
            slen = dStrlen(text);

         Point2I pos(offset.x + 4 + mColumnOffsets[index], offset.y);

         RectI saveClipRect;
         bool clipped = false;

         if(mClipColumnText && (index != (mColumnOffsets.size() - 1)))
         {
            saveClipRect = dglGetClipRect();

            RectI clipRect(pos, Point2I(mColumnOffsets[index+1] - mColumnOffsets[index] - 4, mCellSize.y));
            if(clipRect.intersect(saveClipRect))
            {
               clipped = true;
               dglSetClipRect(clipRect);
            }
         }

         dglDrawTextN(mFont, pos, text, slen, mProfile->mFontColors);

         if(clipped)
            dglSetClipRect(saveClipRect);
      }
      if(!nextCol)
         break;
      text = nextCol+1;
   }
}

U32 GuiTextListCtrl::getRowWidth(Entry *row)
{
   U32 width = 1;
   const char *text = row->text;
   for(U32 index = 0; index < (U32)mColumnOffsets.size(); index++)
   {
      const char *nextCol = dStrchr(text, '\t');
      U32 textWidth;
      if(nextCol)
         textWidth = mFont->getStrNWidth((const UTF8*)text, (U32)(nextCol - text));
      else
         textWidth = mFont->getStrWidth((const UTF8*)text);
      if(mColumnOffsets[index] >= 0)
         width = getMax(width, mColumnOffsets[index] + textWidth);
      if(!nextCol)
         break;
      text = nextCol+1;
   }
   return width;
}

void GuiTextListCtrl::insertEntry(U32 id, const char *text, S32 index)
{
   Entry e;
   e.text = dStrdup(text);
   e.id = id;
   e.active = true;
   if(!mList.size())
      mList.push_back(e);
   else
   {
      if(index > mList.size())
         index = mList.size();

      mList.insert(index);
      mList[index] = e;
   }
   setSize(Point2I(1, mList.size()));
}

void GuiTextListCtrl::addEntry(U32 id, const char *text)
{
   Entry e;
   e.text = dStrdup(text);
   e.id = id;
   e.active = true;
   mList.push_back(e);
   setSize(Point2I(1, mList.size()));
}

void GuiTextListCtrl::setEntry(U32 id, const char *text)
{
   S32 e = findEntryById(id);
   if(e == -1)
      addEntry(id, text);
   else
   {
      dFree(mList[e].text);
      mList[e].text = dStrdup(text);

      // Still have to call this to make sure cells are wide enough for new values:
      setSize( Point2I( 1, mList.size() ) );
   }
   setUpdate();
}

void GuiTextListCtrl::setEntryActive(U32 id, bool active)
{
   S32 index = findEntryById( id );
   if ( index == -1 )
      return;

   if ( mList[index].active != active )
   {
      mList[index].active = active;

      // You can't have an inactive entry selected...
      if ( !active && mSelectedCell.y >= 0 && mSelectedCell.y < mList.size()
           && mList[mSelectedCell.y].id == id )
         setSelectedCell( Point2I( -1, -1 ) );

      setUpdate();
   }
}

S32 GuiTextListCtrl::findEntryById(U32 id)
{
   for(U32 i = 0; i < (U32)mList.size(); i++)
      if(mList[i].id == id)
         return i;
   return -1;
}

S32 GuiTextListCtrl::findEntryByText(const char *text)
{
   for(U32 i = 0; i < (U32)mList.size(); i++)
      if(!dStricmp(mList[i].text, text))
         return i;
   return -1;
}

bool GuiTextListCtrl::isEntryActive(U32 id)
{
   S32 index = findEntryById( id );
   if ( index == -1 )
      return( false );

   return( mList[index].active );
}

void GuiTextListCtrl::setSize(Point2I newSize)
{
   mSize = newSize;

   if ( bool( mFont ) )
   {
      if ( mSize.x == 1 && mFitParentWidth )
      {
         GuiScrollCtrl* parent = dynamic_cast<GuiScrollCtrl *>(getParent());
         if ( parent )
            mCellSize.x = parent->getContentExtent().x;
      }
      else
      {
         // Find the maximum width cell:
         S32 maxWidth = 1;
         for ( U32 i = 0; i < (U32)mList.size(); i++ )
         {
            U32 rWidth = getRowWidth( &mList[i] );
            if ( rWidth > (U32)maxWidth )
               maxWidth = rWidth;
         }

         mCellSize.x = maxWidth + 8;
      }

      mCellSize.y = mFont->getHeight() + 2;
   }

   Point2I newExtent( newSize.x * mCellSize.x + mHeaderDim.x, newSize.y * mCellSize.y + mHeaderDim.y );
   resize( mBounds.point, newExtent );
}

void GuiTextListCtrl::clear()
{
   while (mList.size())
      removeEntry(mList[0].id);

   mMouseOverCell.set( -1, -1 );
   setSelectedCell(Point2I(-1, -1));
}

void GuiTextListCtrl::sort(U32 column, bool increasing)
{
   if (getNumEntries() < 2)
      return;
   sortColumn = column;
   sIncreasing = increasing;
   dQsort((void *)mList.address(), mList.size(), sizeof(Entry), textCompare);
}

void GuiTextListCtrl::sortNumerical( U32 column, bool increasing )
{
   if ( getNumEntries() < 2 )
      return;

   sortColumn = column;
   sIncreasing = increasing;
   dQsort( (void*)mList.address(), mList.size(), sizeof( Entry ), numCompare );
}

void GuiTextListCtrl::onRemove()
{
   clear();
   Parent::onRemove();
}

U32 GuiTextListCtrl::getNumEntries()
{
   return mList.size();
}

void GuiTextListCtrl::removeEntryByIndex(S32 index)
{
   if(index < 0 || index >= mList.size())
      return;
   dFree(mList[index].text);
   mList.erase(index);

   setSize(Point2I( 1, mList.size()));
   setSelectedCell(Point2I(-1, -1));
}

void GuiTextListCtrl::removeEntry(U32 id)
{
   S32 index = findEntryById(id);
   removeEntryByIndex(index);
}

const char *GuiTextListCtrl::getSelectedText()
{
   if (mSelectedCell.y == -1)
      return NULL;

   return mList[mSelectedCell.y].text;
}

const char *GuiTextListCtrl::getScriptValue()
{
   return getSelectedText();
}

void GuiTextListCtrl::setScriptValue(const char *val)
{
   S32 e = findEntryByText(val);
   if(e == -1)
      setSelectedCell(Point2I(-1, -1));
   else
      setSelectedCell(Point2I(0, e));
}

bool GuiTextListCtrl::onKeyDown( const GuiEvent &event )
{
   //if this control is a dead end, make sure the event stops here
   if ( !mVisible || !mActive || !mAwake )
      return true;

   S32 yDelta = 0;
   switch( event.keyCode )
   {
   case KEY_RETURN:
      if ( mAltConsoleCommand[0] )
         Con::evaluate( mAltConsoleCommand, false );
      break;
   case KEY_LEFT:
   case KEY_UP:
      if ( mSelectedCell.y > 0 )
      {
         mSelectedCell.y--;
         yDelta = -mCellSize.y;
   }
      break;
   case KEY_DOWN:
   case KEY_RIGHT:
      if ( mSelectedCell.y < ( mList.size() - 1 ) )
   {
         mSelectedCell.y++;
         yDelta = mCellSize.y;
   }
      break;
   case KEY_HOME:
      if ( mList.size() )
      {
         mSelectedCell.y = 0;
         yDelta = -(mCellSize.y * mList.size() + 1 );
      }
      break;
   case KEY_END:
      if ( mList.size() )
   {
         mSelectedCell.y = mList.size() - 1;
         yDelta = (mCellSize.y * mList.size() + 1 );
      }
      break;
   case KEY_DELETE:
      if ( mSelectedCell.y >= 0 && mSelectedCell.y < mList.size() )
      Con::executef( this, 2, "onDeleteKey", Con::getIntArg( mList[mSelectedCell.y].id ) );
      break;
   default:
   return( Parent::onKeyDown( event ) );
      break;
   };

   GuiScrollCtrl* parent = dynamic_cast<GuiScrollCtrl *>(getParent());
   if ( parent )
      parent->scrollDelta( 0, yDelta );

   return ( true );



}

