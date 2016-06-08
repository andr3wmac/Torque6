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

#ifndef _PHYSICS_ENGINE_H_
#define _PHYSICS_ENGINE_H_

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

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

#ifndef _DEBUG_MODE_H_
#include "debug/debugMode.h"
#endif

// ------------------------------------------------------------------------------
//  How the Physics Engine works:
// ------------------------------------------------------------------------------
//
//   1) PhysicsEngine::InterpolateTick is called
//   2) If time passed is >= 1.0f/32.0f PhysicsEngine::processPhysics is called
//   3) If single-threaded, step is processed, physics is updated.
//   4) If multi-threaded:
//      a) Attempt to lock Execute mutex. The Physics Thread should be done by
//           now so this should be immediately available.
//      b) Unlock Finished mutex which lets the Physics Thread lock it.
//           The physics thread is now stuck trying to lock Execute again, but we 
//           hold that. This means we're synced and free to operate on the data.
//      c) Sync data between physics thread and main thread.
//      d) Unlock Execute mutex, letting the physics thread start it's next step.
//      e) Lock Finished mutex until the next time Physics Engine ticks.
//
// ------------------------------------------------------------------------------

namespace Physics 
{
   class PhysicsThread;

   // Shared data between main thread and physics thread.
   struct PhysicsAction
   {
      enum Enum
      {
         setPosition,
         setRotationEuler,
         setRotationQuat,
         setScale,
         setLinearVelocity,
         applyForce,
         COUNT
      };

      Enum actionType;
      Point3F vector3Value;
      QuatF quatValue;
   };

   class PhysicsObject
   {
      public:
         Point3F                             mPosition;
         QuatF                               mRotation;
         Vector<PhysicsAction>               mPhysicsActions;
         bool                                mStatic;
         bool                                mBlocking;
         bool                                mInitialized;
         bool                                mDeleted;
         bool                                mShouldBeDeleted;
         void*                               mUser;
         Delegate<void(void* _hitUser)>      mOnCollideDelegate;

         PhysicsObject()
         {
            mPosition         = Point3F(0.0f, 0.0f, 0.0f);
            mRotation         = QuatF(0.0f, 0.0f, 0.0f, 1.0f);
            mStatic           = false;
            mBlocking         = true;
            mInitialized      = false;
            mDeleted          = true;
            mShouldBeDeleted  = false;
            mUser             = NULL;
            mOnCollideDelegate.clear();
         }
         ~PhysicsObject() { }

         void addAction(PhysicsAction::Enum _actionType, Point3F _vector3Value)
         {
            PhysicsAction action;
            action.actionType = _actionType;
            action.vector3Value = _vector3Value;
            mPhysicsActions.push_back(action);
         }

         void addAction(PhysicsAction::Enum _actionType, QuatF _quatValue)
         {
            PhysicsAction action;
            action.actionType = _actionType;
            action.quatValue = _quatValue;
            mPhysicsActions.push_back(action);
         }

         virtual void initialize()                    { mInitialized = true; }
         virtual void destroy()                       { mInitialized = false; }
         virtual void setStatic(bool _val)            { mStatic = _val; }
         virtual void setBlocking(bool _val)          { mBlocking = _val; }

         virtual Point3F getPosition()                { return mPosition; }
         virtual void setPosition(Point3F _position)  { addAction(PhysicsAction::setPosition, _position); }
         virtual QuatF getRotation()                  { return mRotation; }
         virtual void setRotation(Point3F _rot)       { addAction(PhysicsAction::setRotationEuler, _rot); }
         virtual void setRotation(QuatF _rot)         { addAction(PhysicsAction::setRotationQuat, _rot); }
         virtual void applyForce(Point3F _force)      { addAction(PhysicsAction::applyForce, _force); }
         virtual void setLinearVelocity(Point3F _vel) { addAction(PhysicsAction::setLinearVelocity, _vel); }
   };

   class PhysicsBox : public PhysicsObject
   {
      public:
         Point3F mScale;

         PhysicsBox()
         {
            mScale = Point3F(1.0f, 1.0f, 1.0f);
         }

         virtual Point3F getScale() { return mScale; }
         virtual void setScale(Point3F _scale) { mScale = _scale; }
   };

   class PhysicsSphere : public PhysicsObject
   {
      public:
         F32 mRadius;

         PhysicsSphere()
         {
            mRadius = 1.0f;
         }

         virtual F32 getRadius() { return mRadius; }
         virtual void setRadius(F32 _radius) { mRadius = _radius; }
   };

   class PhysicsMesh : public PhysicsObject
   {
   public:
      
      Point3F              mScale;
      Graphics::MeshData   mMeshData;

      PhysicsMesh()
      {
         mScale = Point3F(1.0f, 1.0f, 1.0f);
      }

      virtual Point3F getScale() { return mScale; }
      virtual void setScale(Point3F _scale) { mScale = _scale; }
   };

   class PhysicsCharacter : public PhysicsObject
   {
   public:
      F32      mRadius;
      F32      mHeight;

      PhysicsCharacter()
      {
         mRadius = 0.6f;
         mHeight = 1.82f;
      }

      virtual F32 getRadius() { return mRadius; }
      virtual void setRadius(F32 _radius) { mRadius = _radius; }
      virtual F32 getHeight() { return mHeight; }
      virtual void setHeight(F32 _height) { mHeight = _height; }
   };

   // Thread safe physics event.
   class PhysicsEvent : public SimEvent
   {
      protected:
         PhysicsObject* mObjA;
         PhysicsObject* mObjB;

      public:
         PhysicsEvent(PhysicsObject* objA, PhysicsObject* objB)
         {
            mObjA = objA;
            mObjB = objB;
         }

         virtual void process(SimObject *object);
   };

   // Physics Engine Core
   class PhysicsEngine : public virtual Tickable
   {
      protected:
         PhysicsThread* mPhysicsThread;
         F64            mPreviousTime;
         F64            mAccumulatorTime;
         F32            mStepSize;
         bool           mRunning;

      public:
         PhysicsEngine();
         ~PhysicsEngine();

         void setRunning(bool value);
         void processPhysics();

         // These must be implemented for a functioning physics engine:
         virtual Vector<PhysicsObject*>   getPhysicsObjects() { Vector<PhysicsObject*> results; return results; }
         virtual PhysicsBox*              createPhysicsBox(Point3F position, Point3F rotation, Point3F scale, void* _user = NULL) { return NULL; }
         virtual PhysicsSphere*           createPhysicsSphere(Point3F position, Point3F rotation, F32 radius, void* _user = NULL) { return NULL; }
         virtual PhysicsMesh*             createPhysicsMesh(Point3F position, Point3F rotation, Point3F scale, const Graphics::MeshData& meshData, void* _user = NULL) { return NULL; }
         virtual PhysicsCharacter*        createPhysicsCharacter(Point3F position, Point3F rotation, F32 radius, F32 height, void* _user = NULL) { return NULL; }
         virtual void                     deletePhysicsObject(PhysicsObject* _obj);
         virtual void                     simulate(F32 dt);
         virtual void                     update();

         // Tickable
         virtual void interpolateTick( F32 delta );
         virtual void processTick();
         virtual void advanceTime( F32 timeDelta );

         // Static Variables
         static void* smPhysicsExecuteMutex;
         static void* smPhysicsFinishedMutex;
   };

   // PhysicsDebug Debug Mode visually displays bounds of physics objects.
   class PhysicsDebug : public Debug::DebugMode
   {
      protected:
         U32 mObjectColors[2048];

      public:
         PhysicsDebug();
         void render(Rendering::RenderCamera* camera);

         DECLARE_DEBUG_MODE("Physics", PhysicsDebug);
   };
}

#endif // _PHYSICS_ENGINE_H_
