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

#include "inputListener.h"

#include "platform/event.h"
#include "console/console.h"
#include "platform/platform.h"
#include "platform/platformInput.h"
#include "platform/platformAssert.h"
#include "io/fileStream.h"
#include "io/resource/resourceManager.h"

// Script bindings.
#include "inputListener_Binding.h"

InputListener::InputListener() :
   mListen(false)
{
   // We should always start with processing of ticks off!
}

//------------------------------------------------------------------------------

InputListener::~InputListener()
{
   setListening(false);
}

//------------------------------------------------------------------------------

Vector<InputListener *>& InputListener::getInputListenerList()
{
   // This helps to avoid the static initialization order fiasco
   static Vector<InputListener *> smInputListenerList; ///< List of tick controls
   return smInputListenerList;
}

//------------------------------------------------------------------------------

void InputListener::setListening(bool listen /* = true  */)
{
   // Ignore no change.
   if (listen == mListen)
      return;

   // Update tick flag.
   mListen = listen;

   // Are we processing ticks?
   if (mListen)
   {
      // Yes, so add to process list.
      getInputListenerList().push_back(this);
      return;
   }

   // No, so remove from process list.
   for (InputListenerListIterator i = getInputListenerList().begin(); i != getInputListenerList().end(); i++)
   {
      if ((*i) == this)
      {
         getInputListenerList().erase(i);
         return;
      }
   }
}

bool InputListener::handleInputEvent(const InputEvent *event)
{
   // Fetch a copy of the render list.
   Vector<InputListener*> listenerList = getInputListenerList();

   // Process Input Events
   for (InputListenerListIterator i = listenerList.begin(); i != listenerList.end(); i++)
   {
      bool result = (*i)->processInputEvent(event);
      if (result)
         return true;
   }

   // Return false if nothing caught the event.
   return false;
}

bool InputListener::handleMouseMoveEvent(const MouseMoveEvent *event)
{
   // Fetch a copy of the render list.
   Vector<InputListener*> listenerList = getInputListenerList();

   // Process Input Events
   for (InputListenerListIterator i = listenerList.begin(); i != listenerList.end(); i++)
   {
      bool result = (*i)->processMouseMoveEvent(event);
      if (result)
         return true;
   }

   // Return false if nothing caught the event.
   return false;
}

bool InputListener::handleScreenTouchEvent(const ScreenTouchEvent *event)
{
   // Fetch a copy of the render list.
   Vector<InputListener*> listenerList = getInputListenerList();

   // Process Input Events
   for (InputListenerListIterator i = listenerList.begin(); i != listenerList.end(); i++)
   {
      bool result = (*i)->processScreenTouchEvent(event);
      if (result)
         return true;
   }

   // Return false if nothing caught the event.
   return false;
}