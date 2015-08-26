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
#include "graphics/dgl.h"
#include "gui/guiCanvas.h"
#include "gui/containers/guiFrameCtrl.h"
#include "gui/containers/guiFrameCtrl_Binding.h"

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(GuiFrameSetCtrl);

//-----------------------------------------------------------------------------

static EnumTable::Enums borderStateEnums[] =
{
   { GuiFrameSetCtrl::FRAME_STATE_ON,    "alwaysOn"  },
   { GuiFrameSetCtrl::FRAME_STATE_OFF,   "alwaysOff" },
   { GuiFrameSetCtrl::FRAME_STATE_AUTO,  "dynamic"   }
};
static EnumTable gBorderStateTable(3, &borderStateEnums[0]);

//-----------------------------------------------------------------------------
void GuiFrameSetCtrl::initPersistFields()
{
   Parent::initPersistFields();
   addField("columns",        TypeS32Vector, Offset(mColumnOffsets, GuiFrameSetCtrl));
   addField("rows",           TypeS32Vector, Offset(mRowOffsets, GuiFrameSetCtrl));
   addField("borderWidth",    TypeS32,       Offset(mFramesetDetails.mBorderWidth, GuiFrameSetCtrl));
   addField("borderColor",    TypeColorI,    Offset(mFramesetDetails.mBorderColor, GuiFrameSetCtrl));
   addField("borderEnable",   TypeEnum,      Offset(mFramesetDetails.mBorderEnable, GuiFrameSetCtrl),   1, &gBorderStateTable);
   addField("borderMovable",  TypeEnum,      Offset(mFramesetDetails.mBorderMovable, GuiFrameSetCtrl),  1, &gBorderStateTable);
   addField("autoBalance",    TypeBool,      Offset(mAutoBalance, GuiFrameSetCtrl));
   addField("fudgeFactor",    TypeS32,       Offset(mFudgeFactor, GuiFrameSetCtrl));
}

//-----------------------------------------------------------------------------
GuiFrameSetCtrl::GuiFrameSetCtrl()
{
   VECTOR_SET_ASSOCIATION(mColumnOffsets);
   VECTOR_SET_ASSOCIATION(mRowOffsets);
   VECTOR_SET_ASSOCIATION(mFrameDetails);

   mAutoBalance = true;
   mIsContainer = true;

   init(1, 1, NULL, NULL);
}

//-----------------------------------------------------------------------------
GuiFrameSetCtrl::GuiFrameSetCtrl(U32 columns, U32 rows, const U32 columnOffsets[], const U32 rowOffsets[])
{
   init(columns, rows, columnOffsets, rowOffsets);
}

//-----------------------------------------------------------------------------
GuiFrameSetCtrl::~GuiFrameSetCtrl()
{
   while (mFrameDetails.size() > 0)
   {
      delete mFrameDetails.last();
      mFrameDetails.pop_back();
   }
}

//-----------------------------------------------------------------------------

void GuiFrameSetCtrl::addObject(SimObject *object)
{
   AssertFatal(object != NULL, "GuiFrameSetCtrl::addObject: NULL object");

   // assign the object to a frame - give it default frame details
   Parent::addObject(object);
   GuiControl *gc = dynamic_cast<GuiControl *>(object);
   if (gc != NULL)
   {
      FrameDetail *detail = new FrameDetail;
      detail->mMinExtent = gc->mMinExtent;
      mFrameDetails.push_back(detail);
   }
   else
      mFrameDetails.push_back(NULL);
   // resize it to fit into the frame to which it is assigned (if no frame for it, don't bother resizing)
   if(isAwake())
      computeSizes();
}

//-----------------------------------------------------------------------------
void GuiFrameSetCtrl::removeObject(SimObject *object)
{
   if (object != NULL)
   {
      VectorPtr<SimObject *>::iterator soitr;
      VectorPtr<FrameDetail *>::iterator fditr = mFrameDetails.begin();
      for (soitr = begin(); soitr != end(); soitr++, fditr++)
      {
         if (*soitr == object)
         {
            delete *fditr;
            mFrameDetails.erase(fditr);
            break;
         }
      }
   }
   Parent::removeObject(object);
}

//-----------------------------------------------------------------------------
void GuiFrameSetCtrl::resize(const Point2I &newPos, const Point2I &newExtent)
{
   // rebalance before losing the old extent (if required)
   if (mAutoBalance == true)
      rebalance(newExtent);

   Parent::resize(newPos, newExtent);

   // compute new sizing info for the frames - takes care of resizing the children
   computeSizes( !mAutoBalance );
}

//-----------------------------------------------------------------------------

void GuiFrameSetCtrl::getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent)
{
   Region curRegion = NONE;
         //*** Determine the region by mouse position.
         Point2I curMousePos = globalToLocalCoord(lastGuiEvent.mousePoint);
         curRegion = pointInAnyRegion(curMousePos);

   switch (curRegion)
   {
   case VERTICAL_DIVIDER:
      // change to left-right cursor
      if(GuiControl::smCursorChanged != CursorManager::curResizeVert)
      {
         //*** We've already changed the cursor, so set it back before we change it again.
         if(GuiControl::smCursorChanged != -1)
            Input::popCursor();

         //*** Now change the cursor shape
         Input::pushCursor(CursorManager::curResizeVert);
         GuiControl::smCursorChanged = CursorManager::curResizeVert;

      }
      break;

   case HORIZONTAL_DIVIDER:
      // change to up-down cursor
      if(GuiControl::smCursorChanged != CursorManager::curResizeHorz)
      {
         //*** We've already changed the cursor, so set it back before we change it again.
         if(GuiControl::smCursorChanged != -1)
            Input::popCursor();

         //*** Now change the cursor shape
         Input::pushCursor(CursorManager::curResizeHorz);
         GuiControl::smCursorChanged = CursorManager::curResizeHorz;
      }
      break;

   case DIVIDER_INTERSECTION:
      // change to move cursor
      if(GuiControl::smCursorChanged != CursorManager::curResizeAll)
      {
         //*** We've already changed the cursor, so set it back before we change it again.
         if(GuiControl::smCursorChanged != -1)
            Input::popCursor();

         //*** Now change the cursor shape
         Input::pushCursor(CursorManager::curResizeAll);
         GuiControl::smCursorChanged = CursorManager::curResizeAll;

      }
      break;

   case NONE:
   default:
      if(GuiControl::smCursorChanged != -1)
      {
         //*** We've already changed the cursor, so set it back before we change it again.
         Input::popCursor();

         GuiControl::smCursorChanged = -1;
      }
      break;
   }

}

//-----------------------------------------------------------------------------
void GuiFrameSetCtrl::onMouseDown(const GuiEvent &event)
{
   if (mFramesetDetails.mBorderEnable != FRAME_STATE_OFF && mFramesetDetails.mBorderMovable != FRAME_STATE_OFF)
   {
      // determine if a divider was hit
      Point2I curMousePos = globalToLocalCoord(event.mousePoint);
      findHitRegion(curMousePos);                        // sets mCurVerticalHit, mCurHorizontalHit, & mCurHitRegion

      if (mCurHitRegion != NONE)
      {
         mouseLock();
         setFirstResponder();
         setUpdate();
      }
   }
}

//-----------------------------------------------------------------------------
void GuiFrameSetCtrl::onMouseUp(const GuiEvent &event)
{
   if (mCurHitRegion != NONE)
   {
      mCurHitRegion = NONE;
      mCurVerticalHit = NO_HIT;
      mCurHorizontalHit = NO_HIT;
      mouseUnlock();
      setUpdate();
   }
}

//-----------------------------------------------------------------------------
void GuiFrameSetCtrl::onMouseDragged(const GuiEvent &event)
{
   if (mCurHitRegion != NONE)
   {
      // identify the frames involved in the resizing, checking if they are resizable
      S32 indexes[4];
      S32 activeFrames = findResizableFrames(indexes);
      if (activeFrames > 0)
      {
         S32 range[4];
         // determine the range of movement, limiting as specified by individual frames
         computeMovableRange(mCurHitRegion, mCurVerticalHit, mCurHorizontalHit, activeFrames, indexes, range);
         Point2I curMousePos = globalToLocalCoord(event.mousePoint);
         switch (mCurHitRegion)
         {
            case VERTICAL_DIVIDER:
               mColumnOffsets[mCurVerticalHit] = getMin(getMax(range[0], curMousePos.x - mLocOnDivider.x), range[1]);
               break;
            case HORIZONTAL_DIVIDER:
               mRowOffsets[mCurHorizontalHit] = getMin(getMax(range[0], curMousePos.y - mLocOnDivider.y), range[1]);
               break;
            case DIVIDER_INTERSECTION:
               mColumnOffsets[mCurVerticalHit] = getMin(getMax(range[0], curMousePos.x - mLocOnDivider.x), range[1]);
               mRowOffsets[mCurHorizontalHit] = getMin(getMax(range[2], curMousePos.y - mLocOnDivider.y), range[3]);
               break;
            default:
               return;
         }
         computeSizes();
      }
   }
}

//-----------------------------------------------------------------------------
bool GuiFrameSetCtrl::onAdd()
{
   if (Parent::onAdd() == false)
      return(false);

   return(true);
}

bool GuiFrameSetCtrl::onWake()
{
   if (Parent::onWake() == false)
      return(false);

   computeSizes();
   return(true);
}

//-----------------------------------------------------------------------------
void GuiFrameSetCtrl::onRender(Point2I offset, const RectI &updateRect )
{
   RectI r(offset.x, offset.y, mBounds.extent.x, mBounds.extent.y);

   // draw the border of the frameset if specified
   if (mProfile->mOpaque)
      dglDrawRectFill(r, mProfile->mFillColor);

   drawDividers(offset);

   if (mProfile->mBorder)
      dglDrawRect(r, mProfile->mBorderColor);

   // draw the frame contents
   renderChildControls(offset, updateRect);
}

//-----------------------------------------------------------------------------
bool GuiFrameSetCtrl::init(U32 columns, U32 rows, const U32 columnOffsets[], const U32 rowOffsets[])
{
   if (columns != 0 && rows != 0)
   {
      mColumnOffsets.clear();
      mRowOffsets.clear();
      U32 i;
      for (i = 0; i < columns; i++)
      {
         if (columnOffsets == NULL)
            mColumnOffsets.push_back(0);
         else
         {
            AssertFatal(columnOffsets != NULL, "GuiFrameSetCtrl::init: NULL column offsets");
            mColumnOffsets.push_back((U32)columnOffsets[i]);
            if (i > 0)
            {
               AssertFatal(mColumnOffsets[i - 1] < mColumnOffsets[i], "GuiFrameSetCtrl::init: column offsets must be monotonically increasing");
               mColumnOffsets.clear();
               return(false);
            }
         }
      }
      for (i = 0; i < rows; i++)
      {
         if (rowOffsets == NULL)
            mRowOffsets.push_back(0);
         else
         {
            AssertFatal(rowOffsets != NULL, "GuiFrameSetCtrl::init: NULL row offsets");
            mRowOffsets.push_back((U32)rowOffsets[i]);
            if (i > 0)
            {
               AssertFatal(mRowOffsets[i - 1] < mRowOffsets[i], "GuiFrameSetCtrl::init: row offsets must be monotonically increasing");
               mRowOffsets.clear();
               return(false);
            }
         }
      }
   }
   mFramesetDetails.mBorderWidth = DEFAULT_BORDER_WIDTH;
   mFramesetDetails.mBorderEnable = FRAME_STATE_AUTO;
   mFramesetDetails.mBorderMovable = FRAME_STATE_AUTO;
   mAutoBalance = false;
   mFudgeFactor = 0;
   mCurHitRegion = NONE;
   mCurVerticalHit = NO_HIT;
   mCurHorizontalHit = NO_HIT;
   return(true);
}

//-----------------------------------------------------------------------------
// point is assumed to already be in local coordinates.
GuiFrameSetCtrl::Region GuiFrameSetCtrl::findHitRegion(const Point2I &point)
{
   Vector<S32>::iterator itr;
   S32 i = 1;
   // step through vertical dividers
   for (itr = mColumnOffsets.begin() + 1; itr < mColumnOffsets.end(); itr++, i++)
   {
      if (hitVerticalDivider(*itr, point) == true)
      {
         mCurVerticalHit = i;
         mLocOnDivider.x = point.x - (*itr);
         break;
      }
   }
   i = 1;
   // step through horizontal dividers
   for (itr = mRowOffsets.begin() + 1; itr < mRowOffsets.end(); itr++, i++)
   {
      if (hitHorizontalDivider(*itr, point) == true)
      {
         mCurHorizontalHit = i;
         mLocOnDivider.y = point.y - (*itr);
         break;
      }
   }
   // now set type of hit...
   if (mCurVerticalHit != NO_HIT)
   {
      if (mCurHorizontalHit != NO_HIT)
         return(mCurHitRegion = DIVIDER_INTERSECTION);
      else
         return(mCurHitRegion = VERTICAL_DIVIDER);
   }
   else if (mCurHorizontalHit != NO_HIT)
      return(mCurHitRegion = HORIZONTAL_DIVIDER);
   else
      return(mCurHitRegion = NONE);
}

GuiFrameSetCtrl::Region GuiFrameSetCtrl::pointInAnyRegion(const Point2I &point)
{
   Vector<S32>::iterator itr;
   S32 i = 1;
   S32 curVertHit = NO_HIT, curHorzHit = NO_HIT;
   Region result = NONE;
   // step through vertical dividers
   for (itr = mColumnOffsets.begin() + 1; itr < mColumnOffsets.end(); itr++, i++)
   {
      if (hitVerticalDivider(*itr, point) == true)
      {
         curVertHit = i;
         break;
      }
   }
   i = 1;
   // step through horizontal dividers
   for (itr = mRowOffsets.begin() + 1; itr < mRowOffsets.end(); itr++, i++)
   {
      if (hitHorizontalDivider(*itr, point) == true)
      {
         curHorzHit = i;
         break;
      }
   }
   // now select the type of region in which the point lies
   if (curVertHit != NO_HIT)
   {
      if (curHorzHit != NO_HIT)
         result = DIVIDER_INTERSECTION;
      else
         result = VERTICAL_DIVIDER;
   }
   else if (curHorzHit != NO_HIT)
      result = HORIZONTAL_DIVIDER;
   return(result);
}

//-----------------------------------------------------------------------------
// indexes must have at least 4 entries.
// This *may* modify mCurVerticalHit, mCurHorizontalHit, and mCurHitRegion if it
// determines that movement is disabled by frame content.
// If it does make such a change, it also needs to do the reset performed by
// onMouseUp if it sets mCurHitRegion to NONE.
S32 GuiFrameSetCtrl::findResizableFrames(S32 indexes[])
{
   AssertFatal(indexes != NULL, "GuiFrameSetCtrl::findResizableFrames: NULL indexes");

   // first, find the column and row indexes of the affected columns/rows
   S32 validIndexes = 0;
   switch (mCurHitRegion)
   {
      case VERTICAL_DIVIDER:                             // columns
         indexes[0] = mCurVerticalHit - 1;
         indexes[1] = mCurVerticalHit;
         validIndexes = 2;
         break;
      case HORIZONTAL_DIVIDER:                           // rows
         indexes[0] = mCurHorizontalHit - 1;
         indexes[1] = mCurHorizontalHit;
         validIndexes = 2;
         break;
      case DIVIDER_INTERSECTION:                         // columns & rows
         indexes[0] = mCurVerticalHit - 1;               // columns
         indexes[1] = mCurVerticalHit;
         indexes[2] = mCurHorizontalHit - 1;             // rows
         indexes[3] = mCurHorizontalHit;
         validIndexes = 4;
         break;
      default:
         break;
   }
   // now, make sure these indexes are for movable frames
   VectorPtr<SimObject *>::iterator soitr;
   VectorPtr<FrameDetail *>::iterator fditr = mFrameDetails.begin();
   GuiControl *gc;
   S32 column = 0;
   S32 row = 0;
   S32 columns = mColumnOffsets.size();
   S32 rows = mRowOffsets.size();
   for (soitr = begin(); soitr != end() && validIndexes > 0; soitr++, fditr++)
   {
      // don't continue if some of the frames are empty
      if (fditr == mFrameDetails.end())
         break;
      // otherwise, check the gui elements for move-restrictions
      gc = dynamic_cast<GuiControl *>(*soitr);
      if (gc != NULL)
      {
         if (column == columns)
         {
            column = 0;
            row++;
         }
         if (row == rows)
            break;
         switch (mCurHitRegion)
         {
            case VERTICAL_DIVIDER:
               if ((column == indexes[0] || column == indexes[1]) && (*fditr) && (*fditr)->mBorderMovable == FRAME_STATE_OFF)
                  validIndexes = 0;
               break;
            case HORIZONTAL_DIVIDER:
               if ((row == indexes[0] || row == indexes[1]) && (*fditr) && (*fditr)->mBorderMovable == FRAME_STATE_OFF)
                  validIndexes = 0;
               break;
            case DIVIDER_INTERSECTION:
               if ((column == indexes[0] || column == indexes[1]) && (*fditr) && (*fditr)->mBorderMovable == FRAME_STATE_OFF)
               {
                  if ((row == indexes[2] || row == indexes[3]) && (*fditr) && (*fditr)->mBorderMovable == FRAME_STATE_OFF)
                     validIndexes = 0;
                  else
                  {
                     mCurHitRegion = HORIZONTAL_DIVIDER;
                     mCurVerticalHit = NO_HIT;
                     indexes[0] = indexes[2];
                     indexes[1] = indexes[3];
                     validIndexes = 2;
                  }
               }
               else if ((row == indexes[2] || row == indexes[3]) && (*fditr) && (*fditr)->mBorderMovable == FRAME_STATE_OFF)
               {
                  mCurHitRegion = VERTICAL_DIVIDER;
                  mCurHorizontalHit = NO_HIT;
                  validIndexes = 2;
               }
               break;
            default:
               return(0);
         }
         column++;
      }
   }
   if (validIndexes == 0)
   {
      mCurHitRegion = NONE;
      mCurVerticalHit = NO_HIT;
      mCurHorizontalHit = NO_HIT;
      mouseUnlock();
      setUpdate();
   }
   return(validIndexes);
}

//-----------------------------------------------------------------------------
// This method locates the gui control and frame detail associated with a
// particular frame index.
bool GuiFrameSetCtrl::findFrameContents(S32 index, GuiControl **gc, FrameDetail **fd)
{
   AssertFatal(gc != NULL, "GuiFrameSetCtrl::findFrameContents: NULL gui control pointer");
   AssertFatal(fd != NULL, "GuiFrameSetCtrl::findFrameContents: NULL frame detail pointer");
   AssertFatal(*gc == NULL, "GuiFrameSetCtrl::findFrameContents: contents of gui control must be NULL");
   AssertFatal(*fd == NULL, "GuiFrameSetCtrl::findFrameContents: contents of frame detail must be NULL");

   if (index >= 0 && index < size())
   {
      VectorPtr<SimObject *>::iterator soitr;
      VectorPtr<FrameDetail *>::iterator fditr = mFrameDetails.begin();
      for (soitr = begin(); soitr != end(); soitr++, fditr++, index--)
      {
         if (index == 0)
         {
            GuiControl *guiCtrl = dynamic_cast<GuiControl *>(*soitr);
            if (guiCtrl != NULL)
            {
               *gc = guiCtrl;
               *fd = *fditr;
               return(true);
            }
            else
               break;
         }
      }
   }
   return(false);
}

//-----------------------------------------------------------------------------
void GuiFrameSetCtrl::computeSizes(bool balanceFrames)
{
   S32 columns = mColumnOffsets.size();
   S32 rows = mRowOffsets.size();
   S32 vDividers = columns - 1;
   S32 hDividers = rows - 1;

   if ( !balanceFrames && mFrameDetails.size() == ( columns * rows ) )
   {
      // This will do some goofy things if you allow this control to resize smaller than
      // the additive minimum extents of its frames--so don't.
      S32 index, delta;

      if ( columns > 1 )
      {
         index = columns - 1;
         delta = mFrameDetails[index]->mMinExtent.x - ( mBounds.extent.x - mColumnOffsets[index] );
         while ( delta > 0 )
         {
            mColumnOffsets[index--] -= delta;
            if ( index >= 0 )
               delta = mFrameDetails[index]->mMinExtent.x - ( mColumnOffsets[index + 1] - mColumnOffsets[index] );
            else
               break;
         }
      }

      if ( rows > 1 )
      {
         index = rows - 1;
         delta = mFrameDetails[columns * index]->mMinExtent.y - ( mBounds.extent.y - mRowOffsets[index] );
         while ( delta > 0 )
         {
            mRowOffsets[index--] -= delta;
            if ( index >= 0 )
               delta = mFrameDetails[columns * index]->mMinExtent.y - ( mRowOffsets[index + 1] - mRowOffsets[index] );
            else
               break;
         }
      }
   }

   // first, update the divider placement if necessary
   if (balanceFrames == true && mColumnOffsets.size() > 0 && mRowOffsets.size() > 0)
   {
      Vector<S32>::iterator itr;
      F32 totWidth = F32(mBounds.extent.x - vDividers * mFramesetDetails.mBorderWidth);
      F32 totHeight = F32(mBounds.extent.y - hDividers * mFramesetDetails.mBorderWidth);
      F32 frameWidth = totWidth/(F32)columns;
      F32 frameHeight = totHeight/(F32)rows;
      F32 i = 0.;
      for (itr = mColumnOffsets.begin(); itr != mColumnOffsets.end(); itr++, i++)
         *itr = (S32)(i * (frameWidth + (F32)mFramesetDetails.mBorderWidth));
      i = 0.;
      for (itr = mRowOffsets.begin(); itr != mRowOffsets.end(); itr++, i++)
         *itr = (S32)(i * (frameHeight + (F32)mFramesetDetails.mBorderWidth));
   }

   // now, resize the contents of each frame (and move content w/o a frame beyond visible range)
   VectorPtr<SimObject *>::iterator soitr;
   GuiControl *gc;
   S32 column = 0;
   S32 row = 0;
   Point2I newPos;
   Point2I newExtent;
   // step through all the children
   for (soitr = begin(); soitr != end(); soitr++)
   {
      // column and row track the current frame being resized
      if (column == columns)
      {
         column = 0;
         row++;
      }
      // resize the contents if its a gui control...
      gc = dynamic_cast<GuiControl *>(*soitr);
      if (gc != NULL)
      {
         if (row >= rows)
         {
            // no more visible frames
            newPos = mBounds.extent;
            newExtent.set(DEFAULT_MIN_FRAME_EXTENT, DEFAULT_MIN_FRAME_EXTENT);
            gc->resize(newPos, newExtent);
            continue;
         }
         else
         {
            // determine x components of new position & extent
            newPos.x = mColumnOffsets[column];
            if (column == vDividers)
               newExtent.x = mBounds.extent.x - mColumnOffsets[column];             // last column
            else
               newExtent.x = mColumnOffsets[column + 1] - mColumnOffsets[column] - mFramesetDetails.mBorderWidth;   // any other column
            // determine y components of new position & extent
            newPos.y = mRowOffsets[row];
            if (row == hDividers)
               newExtent.y = mBounds.extent.y - mRowOffsets[row];                   // last row
            else
               newExtent.y = mRowOffsets[row + 1] - mRowOffsets[row] - mFramesetDetails.mBorderWidth;            // any other row
            // apply the new position & extent
            gc->resize(newPos, newExtent);
            column++;
         }
      }
   }
}

//-----------------------------------------------------------------------------
// this method looks at the previous offsets, and uses them to redistribute
// the available height & width proportionally.
void GuiFrameSetCtrl::rebalance(const Point2I &newExtent)
{
   // look at old_width and old_height - current extent
   F32 widthScale = (F32)newExtent.x/(F32)mBounds.extent.x;
   F32 heightScale = (F32)newExtent.y/(F32)mBounds.extent.y;
   Vector<S32>::iterator itr;
   // look at old width offsets
   for (itr = mColumnOffsets.begin() + 1; itr < mColumnOffsets.end(); itr++)
      // multiply each by new_width/old_width
      *itr = S32(F32(*itr) * widthScale);
   // look at old height offsets
   for (itr = mRowOffsets.begin() + 1; itr < mRowOffsets.end(); itr++)
      // multiply each by new_height/new_width
      *itr = S32(F32(*itr) * heightScale);

}

//-----------------------------------------------------------------------------
void GuiFrameSetCtrl::computeMovableRange(Region hitRegion, S32 vertHit, S32 horzHit, S32 numIndexes, const S32 indexes[], S32 ranges[])
{
   S32 hardRanges[4];
   switch (numIndexes)
   {
      case 2:
         switch (hitRegion)
         {
            case VERTICAL_DIVIDER:
               ranges[0] = hardRanges[0] = (vertHit <= 1) ? mFramesetDetails.mBorderWidth : mColumnOffsets[vertHit - 1] + mFramesetDetails.mBorderWidth;
               ranges[1] = hardRanges[1] = (vertHit >= (mColumnOffsets.size() - 1)) ? mBounds.extent.x : mColumnOffsets[vertHit + 1] - mFramesetDetails.mBorderWidth;
               break;
            case HORIZONTAL_DIVIDER:
               ranges[0] = hardRanges[0] = (horzHit <= 1) ? mFramesetDetails.mBorderWidth : mRowOffsets[horzHit - 1] + mFramesetDetails.mBorderWidth;
               ranges[1] = hardRanges[1] = (horzHit >= (mRowOffsets.size() - 1)) ? mBounds.extent.y : mRowOffsets[horzHit + 1] - mFramesetDetails.mBorderWidth;
               break;
            default:
               return;
         }
         break;
      case 4:
         if (hitRegion == DIVIDER_INTERSECTION)
         {
            ranges[0] = hardRanges[0] = (vertHit <= 1) ? mFramesetDetails.mBorderWidth : mColumnOffsets[vertHit - 1] + mFramesetDetails.mBorderWidth;
            ranges[1] = hardRanges[1] = (vertHit >= (mColumnOffsets.size() - 1)) ? mBounds.extent.x : mColumnOffsets[vertHit + 1] - mFramesetDetails.mBorderWidth;
            ranges[2] = hardRanges[2] = (horzHit <= 1) ? mFramesetDetails.mBorderWidth : mRowOffsets[horzHit - 1] + mFramesetDetails.mBorderWidth;
            ranges[3] = hardRanges[3] = (horzHit >= (mRowOffsets.size() - 1)) ? mBounds.extent.y : mRowOffsets[horzHit + 1] - mFramesetDetails.mBorderWidth;
         }
         else
            return;
         break;
      default:
         return;
   }
   // now that we have the hard ranges, reduce ranges based on minimum frame extents
   VectorPtr<SimObject *>::iterator soitr;
   VectorPtr<FrameDetail *>::iterator fditr = mFrameDetails.begin();
   GuiControl *gc;
   S32 column = 0;
   S32 row = 0;
   S32 columns = mColumnOffsets.size();
   S32 rows = mRowOffsets.size();
   for (soitr = begin(); soitr != end(); soitr++, fditr++)
   {
      // only worry about visible frames
      if (column == columns)
      {
         column = 0;
         row++;
      }
      if (row == rows)
         return;
      gc = dynamic_cast<GuiControl *>(*soitr);
      if (gc != NULL)
      {
         // the gui control is in a visible frame, so look at its frame details
         if ((*fditr) != NULL)
         {
            switch (hitRegion)
            {
               case VERTICAL_DIVIDER:
                  if (column == indexes[0])
                     ranges[0] = getMax(ranges[0], hardRanges[0] + (*fditr)->mMinExtent.x);
                  if (column == indexes[1])
                     ranges[1] = getMin(ranges[1], hardRanges[1] - (*fditr)->mMinExtent.x);
                  break;
               case HORIZONTAL_DIVIDER:
                  if (row == indexes[0])
                     ranges[0] = getMax(ranges[0], hardRanges[0] + (*fditr)->mMinExtent.y);
                  if (row == indexes[1])
                     ranges[1] = getMin(ranges[1], hardRanges[1] - (*fditr)->mMinExtent.y);
                  break;
               case DIVIDER_INTERSECTION:
                  if (column == indexes[0])
                     ranges[0] = getMax(ranges[0], hardRanges[0] + (*fditr)->mMinExtent.x);
                  if (column == indexes[1])
                     ranges[1] = getMin(ranges[1], hardRanges[1] - (*fditr)->mMinExtent.x);
                  if (row == indexes[2])
                     ranges[2] = getMax(ranges[2], hardRanges[2] + (*fditr)->mMinExtent.y);
                  if (row == indexes[3])
                     ranges[3] = getMin(ranges[3], hardRanges[3] - (*fditr)->mMinExtent.y);
                  break;
               default:
                  return;
            }
         }
         column++;
      }
   }
}

//-----------------------------------------------------------------------------
void GuiFrameSetCtrl::drawDividers(const Point2I &offset)
{
   // draw the frame dividers, if they are enabled
   if (mFramesetDetails.mBorderEnable != FRAME_STATE_OFF)
   {
      RectI r;
      Vector<S32>::iterator itr;
      for (itr = mColumnOffsets.begin() + 1; itr < mColumnOffsets.end(); itr++)
      {
         r.point = Point2I(*itr - mFramesetDetails.mBorderWidth, mFudgeFactor) + offset;
         r.extent.set(mFramesetDetails.mBorderWidth, mBounds.extent.y - ( 2 * mFudgeFactor ) );
         dglDrawRectFill(r, mFramesetDetails.mBorderColor);
      }
      for (itr = mRowOffsets.begin() + 1; itr < mRowOffsets.end(); itr++)
      {
         r.point = Point2I(mFudgeFactor, *itr - mFramesetDetails.mBorderWidth) + offset;
         r.extent.set(mBounds.extent.x - ( 2 * mFudgeFactor ), mFramesetDetails.mBorderWidth);
         dglDrawRectFill(r, mFramesetDetails.mBorderColor);
      }
   }
}

//-----------------------------------------------------------------------------
void GuiFrameSetCtrl::frameBorderEnable(S32 index, const char *state)
{
   GuiControl *gc = NULL;
   FrameDetail *fd = NULL;
   if (findFrameContents(index, &gc, &fd) == true && fd != NULL)
   {
      if (state != NULL)
      {
         // find the value for the detail member
         for (S32 i = 0; i < gBorderStateTable.size; i++)
         {
            if (dStrcmp(state, gBorderStateTable.table[i].label) == 0)
               fd->mBorderEnable = gBorderStateTable.table[i].index;
         }
      }
      else
         // defaults to AUTO if NULL passed in state
         fd->mBorderEnable = FRAME_STATE_AUTO;
   }
}

//-----------------------------------------------------------------------------
void GuiFrameSetCtrl::frameBorderMovable(S32 index, const char *state)
{
   GuiControl *gc = NULL;
   FrameDetail *fd = NULL;
   if (findFrameContents(index, &gc, &fd) == true && fd != NULL)
   {
      if (state != NULL)
      {
         // find the value for the detail member
         for (S32 i = 0; i < gBorderStateTable.size; i++)
         {
            if (dStrcmp(state, gBorderStateTable.table[i].label) == 0)
               fd->mBorderMovable = gBorderStateTable.table[i].index;
         }
      }
      else
         // defaults to AUTO if NULL passed in state
         fd->mBorderMovable = FRAME_STATE_AUTO;
   }
}

//-----------------------------------------------------------------------------
void GuiFrameSetCtrl::frameMinExtent(S32 index, const Point2I &extent)
{
   GuiControl *gc = NULL;
   FrameDetail *fd = NULL;
   if (findFrameContents(index, &gc, &fd) == true && fd != NULL)
      fd->mMinExtent = extent;
}
