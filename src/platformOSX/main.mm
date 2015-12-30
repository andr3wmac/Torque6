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

extern "C"
{
    #import <Foundation/Foundation.h>
    #import <AppKit/AppKit.h>

    __attribute__ ((visibility ("default"))) int osxmain(int argc, const char **argv);
};

#include "AppDelegate.h"
#include "game/gameInterface.h"
#include <bgfx/bgfxplatform.h>

NSEvent* pollEvent()
{
    return [NSApp
            nextEventMatchingMask:NSAnyEventMask
            untilDate:[NSDate distantPast] // do not wait for event
            inMode:NSDefaultRunLoopMode
            dequeue:YES
            ];
}

void processEvents()
{
   NSEvent* event = pollEvent();
   while (event)
   {
      [NSApp sendEvent:event];
      [NSApp updateWindows];
      
      event = pollEvent();
   }
}

int osxmain(int argc, const char **argv)
{
   [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
   [NSApp activateIgnoringOtherApps:YES];
   [NSApp finishLaunching];
   
   id quitMenuItem = [NSMenuItem new];
   [quitMenuItem
    initWithTitle:@"Quit"
    action:@selector(terminate:)
    keyEquivalent:@"q"];

   id appMenu = [NSMenu new];
   [appMenu addItem:quitMenuItem];

   id appMenuItem = [NSMenuItem new];
   [appMenuItem setSubmenu:appMenu];

   id menubar = [[NSMenu new] autorelease];
   [menubar addItem:appMenuItem];
   [NSApp setMainMenu:menubar];
   
   Game->mainInitialize(argc, argv);

   // run the game
   while ( Game->isRunning() )
   {
      // Keeps the OSX window happy
      processEvents();

      Game->mainLoop();
   }

   Game->mainShutdown();
   
   return 0;
}
