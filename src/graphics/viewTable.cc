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
#include <bgfx/bgfx.h>

namespace Graphics
{
   static S32            s_viewTableLastID = -1;
   static U32            s_viewTableCount = 0;
   static ViewTableEntry s_viewTable[256];
   static S16            s_lastPriority = 0;
   static bool           s_viewTableDirty = false;

   void dumpViewTable()
   {
      Con::printf("View Table:");
      for(U32 i = 0; i < s_viewTableCount; ++i)
      {
         for(U32 n = 0; n < s_viewTableCount; ++n)
         {
            ViewTableEntry* view = &s_viewTable[n];
            if ( view->deleted )
               continue;

            if ( view->id == i )
               Con::printf("  [%d] %s (Priority: %d)", i, view->name, view->priority);
         }
      }
   }

   ViewTableEntry* insertView(const char* name, S16 priority, U8 viewID)
   {
      // Move up any views >= the desired ID.
      for(U32 n = 0; n < s_viewTableCount; ++n)
      {
         ViewTableEntry* view = &s_viewTable[n];
         if ( view->id >= viewID )
            view->id++;
      }

      // Store the last id so getView() can keep getting called
      // and get the next views.
      s_viewTableLastID = viewID;

      // Create new entry: Empty spot?
      for(U32 n = 0; n < s_viewTableCount; ++n)
      {
         ViewTableEntry* view = &s_viewTable[n];
         if ( view->deleted )
         {
            dStrcpy(view->name, name);
            view->temporary   = false;
            view->deleted     = false;
            view->priority    = priority;
            view->id          = viewID;

            //dumpViewTable();
            s_viewTableDirty = true;
            return view;
         }
      }

      // Create new entry
      dStrcpy(s_viewTable[s_viewTableCount].name, name);
      s_viewTable[s_viewTableCount].temporary   = false;
      s_viewTable[s_viewTableCount].deleted     = false;
      s_viewTable[s_viewTableCount].id          = viewID;
      s_viewTable[s_viewTableCount].priority    = priority;
      s_viewTableCount++;

      //dumpViewTable();
      s_viewTableDirty = true;
      return &s_viewTable[s_viewTableCount - 1];
   }

   void deleteView(U8 viewID)
   {
      for (U32 n = 0; n < s_viewTableCount; ++n)
      {
         ViewTableEntry* view = &s_viewTable[n];
         if (view->id == viewID)
            view->deleted = true;
         if (view->id > viewID)
            view->id--;
      }

      s_viewTableDirty = true;
      //dumpViewTable();
   }

   void resetViews()
   {
      // Clear View Settings in bgfx if view table is dirty.
      if (s_viewTableDirty)
      {
         for (U32 n = 0; n < s_viewTableCount; ++n)
            bgfx::resetView(n);

         s_viewTableDirty = false;
      }

      // Delete temporary views
      for (U32 n = 0; n < s_viewTableCount; ++n)
      {
         ViewTableEntry* view = &s_viewTable[n];

         if (view->deleted)
            continue;

         if (view->temporary)
            deleteView(view->id);
      }
   }

   ViewTableEntry* getView(const char* name)
   {
      return getView(name, s_lastPriority);
   }

   ViewTableEntry* getView(const char* name, S16 priority)
   {
      s_lastPriority = priority;

      // Find existing view in table.
      for(U32 n = 0; n < s_viewTableCount; ++n)
      {
         ViewTableEntry* view = &s_viewTable[n];
         if ( !view->deleted && dStrcmp(view->name, name) == 0 )
            return view;
      }

      // Find position in table based on priority.
      ViewTableEntry* targetView = NULL;
      S16 targetPriority = S16_MAX;
      for(U32 n = 0; n < s_viewTableCount; ++n)
      {
         ViewTableEntry* view = &s_viewTable[n];
         if (!view->deleted && priority < view->priority && view->priority < targetPriority)
         {
            targetView     = view;
            targetPriority = view->priority;
         }
      }

      // Determine view ID to insert at.
      U8 viewID = 0;
      if ( targetView != NULL )
         viewID = targetView->id;
      else
      {
         s_viewTableLastID++;
         viewID = (U8)s_viewTableLastID;
      }

      // Insert new entry.
      return insertView(name, priority, viewID);
   }

   ViewTableEntry* getTemporaryView(const char* name)
   {
      ViewTableEntry* view = getView(name);
      view->temporary = true;
      return view;
   }

   ViewTableEntry* getTemporaryView(const char* name, S16 priority)
   {
      ViewTableEntry* view = getView(name, priority);
      view->temporary = true;
      return view;
   }
}