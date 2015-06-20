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

#include "physics.h"

#include "console/consoleInternal.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "3d/rendering/common.h"
#include "3d/rendering/renderable.h"

#include <bgfx.h>
#include <bx/fpumath.h>

#include "physics_ScriptBinding.h"

namespace Physics
{
   bool paused = false;
   PhysicsEngine* engine = NULL;

   // Init/Destroy
   void init()
   {
      engine = new BulletPhysicsEngine();
      engine->setRunning(true);
   }

   void destroy()
   {
      SAFE_DELETE(engine);
   }

   void pause()
   {
      paused = true;
      engine->setRunning(false);
   }

   void resume()
   {
      paused = false;
      engine->setRunning(true);
   }

   PhysicsObject* getPhysicsObject(void* _user)
   {
      return engine->getPhysicsObject(_user);
   }

   void deletePhysicsObject(PhysicsObject* _obj)
   {
      engine->deletePhysicsObject(_obj);
   }
}