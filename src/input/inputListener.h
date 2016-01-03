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

#ifndef _INPUTLISTENER_H_
#define _INPUTLISTENER_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _EVENT_H_
#include "platform/event.h"
#endif

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

class DLL_PUBLIC InputListener
{
   private:
      // This just makes life easy
      typedef Vector< InputListener *>::iterator  InputListenerListIterator;

      /// Returns a reference to the list of all Renderable objects.
      static Vector<InputListener *>& getInputListenerList();

      /// Indicates whether the object is currently processing ticks or not.
      bool mListen;

   protected:
      /// This method is called once every 32ms if isProcessingTicks returns true
      /// when called on the object
      virtual bool processInputEvent(const InputEvent *event) = 0;
      virtual bool processMouseMoveEvent(const MouseMoveEvent *event) = 0;
      virtual bool processScreenTouchEvent(const ScreenTouchEvent *event) = 0;

   public:
      /// Constructor
      /// This will add the object to the process list
      InputListener();

      /// Destructor
      /// Remove this object from the process list
      ~InputListener();

      /// Is this object wanting to receive tick notifications
      /// @returns True if object wants tick notifications
      bool isListening() const { return mListen; };

      /// Sets this object as either tick processing or not
      /// @param   tick     True if this object should process ticks
      void setListening(bool listen = true);

      static bool handleInputEvent(const InputEvent *event);
      static bool handleMouseMoveEvent(const MouseMoveEvent *event);
      static bool handleScreenTouchEvent(const ScreenTouchEvent *event);
};

#endif // _INPUTLISTENER_H_
