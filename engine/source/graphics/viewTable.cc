//-----------------------------------------------------------------------------
// Copyright (c) 2015 Andrew Mac
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

#include "graphics/core.h"
#include <bgfx.h>

namespace Graphics
{
   S32            _viewTableLastID = -1;
   U32            _viewTableCount = 0;
   ViewTableEntry _viewTable[256];

   void dumpViewTable()
   {
      Con::printf("View Table:");
      for(U32 i = 0; i < _viewTableCount; ++i)
      {
         for(U32 n = 0; n < _viewTableCount; ++n)
         {
            ViewTableEntry* view = &_viewTable[n];
            if ( view->deleted )
               continue;

            if ( view->id == i )
               Con::printf("  [%d] %s", i, view->name);
         }
      }
   }

   ViewTableEntry* insertView(const char* name, U8 viewID)
   {
      // Move up any views >= the desired ID.
      for(U32 n = 0; n < _viewTableCount; ++n)
      {
         ViewTableEntry* view = &_viewTable[n];
         if ( view->id >= viewID )
            view->id++;
      }

      // Store the last id so getView() can keep getting called
      // and get the next views.
      _viewTableLastID = viewID;

      // Create new entry: Empty spot?
      for(U32 n = 0; n < _viewTableCount; ++n)
      {
         ViewTableEntry* view = &_viewTable[n];
         if ( view->deleted )
         {
            dStrcpy(view->name, name);
            view->deleted = false;
            view->id = viewID;
            dumpViewTable();
            return view;
         }
      }

      // Create new entry
      _viewTable[_viewTableCount].deleted = false;
      _viewTable[_viewTableCount].id = viewID;
      dStrcpy(_viewTable[_viewTableCount].name, name);
      _viewTableCount++;
      //dumpViewTable();
      return &_viewTable[_viewTableCount - 1];
   }

   ViewTableEntry* getView(const char* name, const char* target, bool beforeTarget)
   {
      // Find existing view in table.
      for(U32 n = 0; n < _viewTableCount; ++n)
      {
         ViewTableEntry* view = &_viewTable[n];
         if ( !view->deleted && dStrcmp(view->name, name) == 0 )
            return view;
      }

      // Find target view in table.
      ViewTableEntry* targetView = NULL;
      if ( dStrlen(target) > 0 )
      {
         for(U32 n = 0; n < _viewTableCount; ++n)
         {
            ViewTableEntry* view = &_viewTable[n];
            if ( !view->deleted && dStrcmp(view->name, target) == 0 )
            {
               targetView = view;
               break;
            }
         }
      }

      // Determine view ID to insert at.
      U8 viewID = 0;
      if ( targetView != NULL )
         viewID = beforeTarget ? targetView->id : (targetView->id + 1);
      else
      {
         _viewTableLastID++;
         viewID = (U8)_viewTableLastID;
      }

      // Insert new entry.
      return insertView(name, viewID);
   }
}