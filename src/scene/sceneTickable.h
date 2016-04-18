//-----------------------------------------------------------------------------
// Copyright (c) 2016 Andrew Mac
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

#ifndef _SCENE_TICKABLE_H_
#define _SCENE_TICKABLE_H_

#include "collection/vector.h"

// This class is a duplicate of Tickable except it only ticks 
// when the scene is active.

namespace Scene
{
   class DLL_PUBLIC SceneTickable
   {
   private:
      static U32 smLastTick;  ///< Time of the last tick that occurred
      static U32 smLastTime;  ///< Last time value at which advanceTime was called
      static U32 smLastDelta; ///< Last delta value for advanceTime

      // This just makes life easy
      typedef Vector<SceneTickable *>::iterator ProcessListIterator;

      /// Returns a reference to the list of all Tickable objects.
      static Vector<SceneTickable *>& getProcessList();

      /// Indicates whether the object is currently processing ticks or not.
      bool mProcessTick;

   protected:
      /// This method is called every frame and lets the control interpolate between
      /// ticks so you can smooth things as long as isProcessingTicks returns true
      /// when it is called on the object
      virtual void interpolateTick(F32 delta) = 0;

      /// This method is called once every 32ms if isProcessingTicks returns true
      /// when called on the object
      virtual void processTick() = 0;

      /// This method is called once every frame regardless of the return value of
      /// isProcessingTicks and informs the object of the passage of time
      virtual void advanceTime(F32 timeDelta) = 0;

   public:
      // Can let everyone look at these if they want to
      static const U32 smTickShift;    ///< Shift value to control how often Ticks occur
      static const U32 smTickMs;       ///< Number of milliseconds per tick, 32 in this case
      static const F32 smTickSec;      ///< Fraction of a second per tick
      static const U32 smTickMask;

      /// Constructor
      /// This will add the object to the process list
      SceneTickable();

      /// Destructor
      /// Remove this object from the process list
      virtual ~SceneTickable();

      /// Is this object wanting to receive tick notifications
      /// @returns True if object wants tick notifications
      virtual bool isProcessingTicks() const { return mProcessTick; };

      /// Sets this object as either tick processing or not
      /// @param   tick     True if this object should process ticks
      virtual void setProcessTicks(bool tick = true);

      //------------------------------------------------------------------------------

         /// This is called in clientProcess to advance the time for all Tickable
         /// objects
         /// @returns True if any ticks were sent
         /// @see clientProcess
      static bool advanceTime(U32 timeDelta);
   };
}

#endif
