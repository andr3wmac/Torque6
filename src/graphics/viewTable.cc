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
#include "rendering/renderCamera.h"
#include <bgfx/bgfx.h>

namespace Graphics
{
   static S32                       s_viewTableLastID = -1;
   static U32                       s_viewTableCount = 0;
   static ViewTableEntry            s_viewTable[256];
   static S32                       s_lastPriority = 0;
   static Rendering::RenderCamera*  s_lastCamera = NULL;
   static bool                      s_viewTableDirty = false;

   void _dumpViewTable()
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

   ViewTableEntry* _insertView(const char* name, S32 priority, U8 viewID, Rendering::RenderCamera* camera = NULL)
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
            view->camera      = camera;

            //_dumpViewTable();
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
      s_viewTable[s_viewTableCount].camera      = camera;
      s_viewTableCount++;

      //_dumpViewTable();
      s_viewTableDirty = true;
      return &s_viewTable[s_viewTableCount - 1];
   }

   void _deleteViewByID(U8 viewID)
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
      //_dumpViewTable();
   }

   ViewTableEntry* _getView(const char* name, S32 priority)
   {
      s_lastPriority = priority;

      // Find existing view in table.
      for (U32 n = 0; n < s_viewTableCount; ++n)
      {
         ViewTableEntry* view = &s_viewTable[n];
         if (!view->deleted && dStrcmp(view->name, name) == 0)
            return view;
      }

      // Find position in table based on priority.
      ViewTableEntry* targetView = NULL;
      S32 targetPriorityDiff = S32_MAX;
      ViewTableEntry* highestView = NULL;
      S32 highestPriority = S32_MIN;
      for (U32 n = 0; n < s_viewTableCount; ++n)
      {
         ViewTableEntry* view = &s_viewTable[n];
         if (view->deleted)
            continue;

         if (view->priority > highestPriority)
         {
            highestPriority = view->priority;
            highestView = view;
         }

         if (priority >= view->priority)
            continue;

         if (view->priority - priority < targetPriorityDiff)
         {
            targetPriorityDiff = view->priority - priority;
            targetView = view;
         }
      }

      // Determine view ID to insert at.
      U8 viewID = 0;
      if (priority > highestPriority && highestView != NULL)
      {
         viewID = highestView->id + 1;
      }
      else if (targetView != NULL)
      {
         viewID = targetView->id;
      }
      else
      {
         s_viewTableLastID++;
         viewID = (U8)s_viewTableLastID;
      }

      // Insert new entry.
      return _insertView(name, priority, viewID, s_lastCamera);
   }

   ViewTableEntry* getCameraStart(Rendering::RenderCamera* camera)
   {
      ViewTableEntry* result = NULL;
      S32 minPriority = (S32)camera->getRenderPriority() * 100000;
      S32 maxPriority = ((S32)camera->getRenderPriority() + 1) * 100000;
      for (U32 n = 0; n < s_viewTableCount; ++n)
      {
         ViewTableEntry* view = &s_viewTable[n];
         if (view->deleted)
            continue;

         if (view->priority >= minPriority && view->priority < maxPriority)
         {
            if (result != NULL && result->id < view->id)
               continue;

            result = view;
         }
      }

      return result;
   }

   void resetViews()
   {
      // Delete temporary views
      for (U32 n = 0; n < s_viewTableCount; ++n)
      {
         ViewTableEntry* view = &s_viewTable[n];

         if (view->deleted)
            continue;

         if (view->temporary)
            _deleteViewByID(view->id);
      }

      // Clear View Settings in bgfx if view table is dirty.
      if (s_viewTableDirty)
      {
         //_dumpViewTable();

         for (U32 n = 0; n < s_viewTableCount; ++n)
         {
            bgfx::resetView(n);
            if (!s_viewTable[n].deleted)
               bgfx::setViewName(s_viewTable[n].id, s_viewTable[n].name);
         }

         s_viewTableDirty = false;
      }
   }

   ViewTableEntry* getView(const char* name)
   {
      char viewName[256];
      dStrcpy(viewName, name);
      if (s_lastCamera != NULL)
      {
         dSprintf(viewName, 256, "%s:%s", s_lastCamera->getName(), name);
      }

      s_lastPriority += 1;
      return _getView(viewName, s_lastPriority);
   }

   ViewTableEntry* getView(const char* name, S32 priority, Rendering::RenderCamera* camera)
   {
      char viewName[256];
      dStrcpy(viewName, name);
      if (camera != NULL)
      {
         dSprintf(viewName, 256, "%s:%s", camera->getName(), name);
         priority += (S32)camera->getRenderPriority() * 100000;
      }

      s_lastCamera = camera;
      return _getView(viewName, priority);
   }

   ViewTableEntry* getTemporaryView(const char* name)
   {
      ViewTableEntry* view = getView(name);
      view->temporary = true;
      return view;
   }

   ViewTableEntry* getTemporaryView(const char* name, S32 priority, Rendering::RenderCamera* camera)
   {
      ViewTableEntry* view = getView(name, priority, camera);
      view->temporary = true;
      return view;
   }

   void deleteView(ViewTableEntry* entry)
   {
      // Find view in table.
      for (U32 n = 0; n < s_viewTableCount; ++n)
      {
         ViewTableEntry* view = &s_viewTable[n];
         if (view == entry && !view->deleted)
         {
            _deleteViewByID(view->id);
            //_dumpViewTable();
            return;
         }
      }
   }
}