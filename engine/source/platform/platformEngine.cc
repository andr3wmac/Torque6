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
#include "console/console.h"
#include "game/gameInterface.h"

void Platform::mainLoop()
{
   if( Game->isRunning() )
      Game->mainLoop();
}

void Platform::resizeWindow(int width, int height)
{
   Platform::setWindowSize( width, height );
         
   Game->gameReactivate();

   char tempBuf[32];
   dSprintf( tempBuf, sizeof(char) * 32, "%d %d %d", width, height, 16 );
   Con::setVariable( "$pref::Video::windowedRes", tempBuf );
}

void Platform::mouseMove(int x, int y)
{
   MouseMoveEvent event;

   event.xPos = x;
   event.yPos = y;
   event.modifier = 0;

   Game->postEvent(event);
}

void Platform::mouseButton(bool down, bool left)
{
   InputEvent event;

   event.deviceInst = 0;
   event.deviceType = MouseDeviceType;
   event.objType = SI_BUTTON;
   event.objInst = left ? KEY_BUTTON0 : KEY_BUTTON1;
   event.action = down ? SI_MAKE : SI_BREAK;
   event.modifier = 0;
   event.ascii = 0;
   event.fValues[0] = down ? 1.0f : 0.0f;

   Game->postEvent(event);
}