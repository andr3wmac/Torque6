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

#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _BULLET_H
#include "bullet.h"
#endif

namespace Physics
{
   // 
   extern bool paused;
   extern PhysicsEngine* engine;

   // Init/Destroy
   void init();
   void destroy();

   void pause();
   void resume();

   PhysicsBox*       getPhysicsBox(Point3F position, Point3F rotation, Point3F scale, void* _user = NULL);
   PhysicsSphere*    getPhysicsSphere(Point3F position, Point3F rotation, F32 radius, void* _user = NULL);
   PhysicsCharacter* getPhysicsCharacter(Point3F position, Point3F rotation, F32 radius, F32 height, void* _user = NULL);
   void deletePhysicsObject(PhysicsObject* _obj);
}

#endif