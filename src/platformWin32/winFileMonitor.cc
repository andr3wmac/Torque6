//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
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

#include <windows.h>

#include "platform/platform.h"
#include "platform/platformFileMonitor.h"
#include "console/console.h"

//-----------------------------------------------------------------------------

Vector<StringTableEntry>   mDirectoryList;
Vector<HANDLE>             mHandleList;

//-----------------------------------------------------------------------------

bool addDirectoryMonitor( StringTableEntry path )
{
   for ( U32 i = 0; i < mDirectoryList.size(); ++i )
   {
      if (dStrcmp(mDirectoryList[i], path) == 0)
         return false;
   }

   HANDLE changeHandle = ::FindFirstChangeNotificationA(path, FALSE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_ATTRIBUTES);
   if (changeHandle == INVALID_HANDLE_VALUE || changeHandle == NULL) 
   {
      Con::errorf("[winFileMonitor::addDirectoryMonitor] : failed on [%s] [%d]", path, GetLastError());
      return false; 
   }

   mDirectoryList.push_back( path );
   mHandleList.push_back( changeHandle );

   return true;
}

bool removeDirectoryMonitor( StringTableEntry path )
{
   for ( U32 i = 0; i < mDirectoryList.size(); ++i )
   {
      if ( dStrcmp(mDirectoryList[i], path) != 0 )
         continue;

      ::FindCloseChangeNotification( mHandleList[i] );
      mDirectoryList.erase( i );
      mHandleList.erase( i );

      return true;
   }

   return false;
}

void processDirectoryMonitors()
{
   // WaitForMultipleObjects has a limit of MAXIMUM_WAIT_OBJECTS (64 at 
   // the moment), so we have to loop till we've handled the entire set.

   for ( U32 i=0; i < mHandleList.size(); i += MAXIMUM_WAIT_OBJECTS )
   {
      U32 numHandles = getMin( (U32)MAXIMUM_WAIT_OBJECTS, (U32)mHandleList.size() - i );

      DWORD dwWaitStatus = WaitForMultipleObjects( numHandles, mHandleList.address()+i, FALSE, 0);
      if ( dwWaitStatus == WAIT_FAILED || dwWaitStatus == WAIT_TIMEOUT )
         continue;

      if ( dwWaitStatus >= WAIT_OBJECT_0 && dwWaitStatus <= (WAIT_OBJECT_0 + numHandles - 1))
      {
         U32 index = i + dwWaitStatus;

         // reset our notification
         // NOTE: we do this before letting the volume system check mod times so we don't miss any.
         //    It is going to loop over the files and check their mod time vs. the saved time.
         //    This may result in extra calls to internalNotifyDirChanged(), but it will simpley check mod times again.
         ::FindNextChangeNotification( mHandleList[index] );

         onDirectoryChanged(mDirectoryList[index]);
      }
   }
}