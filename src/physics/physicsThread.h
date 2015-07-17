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

#ifndef _PHYSICS_THREAD_H_
#define _PHYSICS_THREAD_H_

#ifndef _PLATFORM_THREADS_THREAD_H_
#include "platform/threads/thread.h"
#endif

#ifndef _SIM_EVENT_H_
#include "sim/simEvent.h"
#endif

#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

#ifndef _UTIL_DELEGATE_H_
#include "delegates/delegate.h"
#endif

#ifndef _SCENEENTITY_H_
#include "3d/entity/entity.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

#ifndef _PHYSICS_COMPONENT_H_
#include "3d/entity/components/physicsComponent.h"
#endif

#ifndef _PHYSICS_ENGINE_H_
#include "physicsEngine.h"
#endif

// ------------------------------------------------------------------------------
//  How the Physics Thread works:
// ------------------------------------------------------------------------------
//
//   1) Attempt to lock Execute mutex. When this locks we're free to execute.
//   2) Step Physics.
//   3) Unlock Execute mutex since we're finished.
//   4) Attempt to lock Finished mutex. We'll block here until the main thread
//        unlocks the mutex.
//   5) Unlock Finished mutex as soon as we get it. We have no reason to hold it
//        we just wanted to block for the main thread.
//
// ------------------------------------------------------------------------------

namespace Physics 
{
   // Threaded Physics
   class PhysicsThread : public Thread
   {
      typedef void (*simulateFunc)(F32 dt);

      protected:
         F32          stepSize;
         simulateFunc simulate;

      public:
         PhysicsThread(F32 _sizeSize, simulateFunc _simulateFunction);

         virtual void run(void *arg = 0);
   };
}

#endif // _PHYSICS_THREAD_H_
