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
#include "platform/platformFileMonitor.h"
#include "collection/vector.h"
#include "console/console.h"
#include "string/stringTable.h"
#include "io/resource/resourceManager.h"

//-----------------------------------------------------------------------------

struct PlatformFileMonitor
{
   bool                       firstRun;
   StringTableEntry           path;
   StringTableEntry           fileName;
   PlatformFileChangeDelegate callback;
   FileTime                   createTime;
   FileTime                   modifyTime;
};

typedef Vector<PlatformFileMonitor> PlatformDirectoryMonitor;
typedef HashMap<StringTableEntry, PlatformDirectoryMonitor> PlatformDirectoryMonitorTable;
PlatformDirectoryMonitorTable dirMonitorMap;

bool addFileMonitor(StringTableEntry path, PlatformFileChangeDelegate callback)
{
   StringTableEntry dirName = Platform::stripFileName(path);
   StringTableEntry fileName = Platform::stripDirectory(path);

   PlatformFileMonitor fileMonitor;
   fileMonitor.firstRun = true;
   fileMonitor.fileName = fileName;
   fileMonitor.path = path;
   fileMonitor.callback = callback;

   PlatformDirectoryMonitorTable::iterator itr = dirMonitorMap.find(dirName);
   if (itr != dirMonitorMap.end())
   {
      PlatformDirectoryMonitor* dirMonitor = &dirMonitorMap[dirName];
      for (U32 n = 0; n < dirMonitor->size(); ++n)
      {
         PlatformFileMonitor* fileMonitor = &dirMonitor->at(n);
         if (dStrcmp(fileMonitor->fileName, fileName) == 0)
            return false;
      }
      dirMonitor->push_back(fileMonitor);
      onDirectoryChanged(dirName);
      return true;
   }
   else {
      PlatformDirectoryMonitor dirMonitor;
      dirMonitor.push_back(fileMonitor);
      dirMonitorMap.insert(dirName, dirMonitor);
      addDirectoryMonitor(dirName);
      onDirectoryChanged(dirName);
      return true;
   }

   return false;
}

bool removeFileMonitor(StringTableEntry path)
{
   StringTableEntry dirName = Platform::stripFileName(path);
   removeDirectoryMonitor(dirName);
   return true;
}

void onDirectoryChanged(StringTableEntry path)
{
   PlatformDirectoryMonitorTable::iterator itr = dirMonitorMap.find(path);
   if (itr == dirMonitorMap.end())
      return;

   PlatformDirectoryMonitor* dirMonitor = &dirMonitorMap[path];
   Vector < Platform::FileInfo > fileInfoVec;
   Platform::dumpPath(path, fileInfoVec);

   //Con::printf("Dir: %s, Size: %d", path, dirMonitor->size());

   for (U32 n = 0; n < dirMonitor->size(); ++n)
   {
      PlatformFileMonitor* fileMonitor = &dirMonitor->at(n);
      for (U32 i = 0; i < fileInfoVec.size(); ++i)
      {
         if (dStrcmp(fileMonitor->fileName, fileInfoVec[i].pFileName) == 0 && dStrlen(fileMonitor->fileName) == dStrlen(fileInfoVec[i].pFileName))
         {
#ifdef TORQUE_OS_WIN32
            FileTime createTime;
            FileTime modifyTime;
            //Con::printf("Getting file times: %s %s", fileInfoVec[i].pFullPath, fileInfoVec[i].pFileName);
            Platform::getFileTimes(fileMonitor->path, &createTime, &modifyTime);
            
            //Con::printf("COMPARING [n=%d, i=%d]: %s %s", n, i, fileMonitor->fileName, fileInfoVec[i].pFileName);
            if (fileMonitor->firstRun || 
               fileMonitor->createTime.v1 != createTime.v1 || fileMonitor->createTime.v2 != createTime.v2 ||
               fileMonitor->modifyTime.v1 != modifyTime.v1 || fileMonitor->modifyTime.v2 != modifyTime.v2)
            {
               fileMonitor->createTime.v1 = createTime.v1;
               fileMonitor->createTime.v2 = createTime.v2;
               fileMonitor->modifyTime.v1 = modifyTime.v1;
               fileMonitor->modifyTime.v2 = modifyTime.v2;

               if (fileMonitor->firstRun)
                  fileMonitor->firstRun = false;
               else
                  fileMonitor->callback(fileMonitor->path);
            }
#endif
         }
      }
   }
}