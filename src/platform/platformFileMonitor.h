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
#ifndef _PLATFORM_FILEMONITOR_H_
#define _PLATFORM_FILEMONITOR_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _SIGNAL_H_
#include "delegates/delegateSignal.h"
#endif

#ifndef _HASHTABLE_H
#include "collection/hashTable.h"
#endif

typedef Delegate<void(StringTableEntry path)> PlatformFileChangeDelegate;
typedef   Signal<void(StringTableEntry path)> PlatformFileChangeSignal;

bool addFileMonitor(StringTableEntry path, PlatformFileChangeDelegate callback);
bool removeFileMonitor(StringTableEntry path);

bool addDirectoryMonitor(StringTableEntry path);
bool removeDirectoryMonitor(StringTableEntry path);
void onDirectoryChanged(StringTableEntry path);
void processDirectoryMonitors();

#endif // _FILE_IO_H_
