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

#include "console/consoleTypes.h"
#include "controllerComponent.h"
#include "graphics/core.h"
#include "3d/entity/entity.h"
#include "3d/entity/components/meshComponent.h"
#include "3d/scene/core.h"

// Script bindings.
#include "controllerComponent_Binding.h"

// Debug Profiling.
#include "debug/profiler.h"

// bgfx/bx
#include <bgfx/bgfx.h>
#include <bx/fpumath.h>

// Assimp - Asset Import Library
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

// Client prediction
static F32 sMinWarpTicks = 0.5f;       // Fraction of tick at which instant warp occurs
static S32 sMaxWarpTicks = 3;          // Max warp duration in ticks
static S32 sMaxPredictionTicks = 30;   // Number of ticks to predict

namespace Scene
{
   IMPLEMENT_CONOBJECT(ControllerComponent);

   ControllerComponent::ControllerComponent()
   {
      mScale.set(0.0f, 0.0f, 0.0f);
      mPosition.set(0.0f, 0.0f, 0.0f);
      mRotation.set(0.0f, 0.0f, 0.0f);

      mDelta.pos        = Point3F(0.0f, 0.0f, 0.0f);
      mDelta.posVec     = Point3F(0.0f, 0.0f, 0.0f);
      mDelta.warpTicks  = mDelta.warpCount = 0;
      mDelta.dt         = 1;
      mDelta.move       = NullMove;

      mDelta.cameraOffset.set(0.0f, 0.0f, 0.0f);
      mDelta.cameraVec.set(0.0f, 0.0f, 0.0f);
      mDelta.cameraRot.set(0.0f, 0.0f, 0.0f);
      mDelta.cameraRotVec.set(0.0f, 0.0f, 0.0f);

      mPredictionCount = 0;

      mVelocity.set(0.0f, 0.0f, 0.0f);
      mTempPosition.set(0.0f, 0.0f, 0.0f);
      mUpdatePosition = false;
      mForceUpdatePosition = false;
   }

   void ControllerComponent::initPersistFields()
   {
      Parent::initPersistFields();
   }

   void ControllerComponent::onAddToScene()
   {  
      mOwnerEntity->setProcessTick(true);
   }

   void ControllerComponent::onRemoveFromScene()
   {  
      //setProcessTicks(false);
   }

   void ControllerComponent::setOwnerPosition(const Point3F& pos)
   {
      if ( mOwnerEntity->mPosition == pos )
         return;

      mOwnerEntity->mPosition = pos;
      mOwnerEntity->refresh();

      if ( mOwnerEntity->isServerObject() )
         Con::printf("[SERVER] Owner Position Updated: %f %f %f", pos.x, pos.y, pos.z);
      else
         Con::printf("[CLIENT] Owner Position Updated: %f %f %f", pos.x, pos.y, pos.z);
   }

   void ControllerComponent::setPosition(const Point3F& pos)
   {
      //if ( mOwnerEntity->mPosition == pos )
      //   return;

      //mOwnerEntity->mPosition = pos;
      //mOwnerEntity->refresh();

      mTempPosition = pos;
      if ( mOwnerEntity->isServerObject() )
         Con::printf("[SERVER] Temp Position Updated: %f %f %f", pos.x, pos.y, pos.z);
      else
         Con::printf("[CLIENT] Temp Position Updated: %f %f %f", pos.x, pos.y, pos.z);
   }

   void ControllerComponent::interpolateMove( F32 dt )
   {  
      if ( mOwnerEntity->isClientObject() )
      {
         Point3F pos = mDelta.pos + mDelta.posVec * dt;
         setOwnerPosition(pos);
         mDelta.dt = dt;
      }
   }

   void ControllerComponent::processMove(const Move* move)
   {  
      if (mDelta.warpTicks > 0) {
         mDelta.warpTicks--;

         // Set new pos
         mDelta.pos = mTempPosition;
         mDelta.pos += mDelta.warpOffset;
         //mDelta.rot += mDelta.rotOffset;

         // Wrap yaw to +/-PI
         //if (mDelta.rot.z < - M_PI_F)
         //   mDelta.rot.z += M_2PI_F;
         //else if (mDelta.rot.z > M_PI_F)
         //   mDelta.rot.z -= M_2PI_F;

         setPosition(mDelta.pos); //,delta.rot);

         // Backstepping
         mDelta.posVec = -mDelta.warpOffset;
         //mDelta.rotVec = -mDelta.rotOffset;
      }
      else 
      {
         if (!move) 
         {
            if (mOwnerEntity->isGhost()) 
            {
               // If we haven't run out of prediction time,
               // predict using the last known move.
               if (mPredictionCount-- <= 0)
                  return;
               move = &mDelta.move;
            }
            else
               move = &NullMove;
         }

         if ( mOwnerEntity->isServerObject() )
         {
            // Process input move
            updateMove(move);
         }
      }
   }

   void ControllerComponent::updateMove(const Move* move)
   {
      mDelta.move = *move;
      mVelocity = Point3F(move->x, move->y, move->z);
      if ( mVelocity.len() > 0.0f )
      {
         mUpdatePosition = true;
         mOwnerEntity->setMaskBits(SceneEntity::ComponentMask);
         setPosition(mTempPosition + mVelocity);
      }

      mDelta.posVec = mTempPosition;
   }

   void ControllerComponent::advanceMove( F32 timeDelta )
   {  
      // Unused at the moment.
   }

   void ControllerComponent::writePacketData(GameConnection *conn, BitStream *stream) 
   { 
      Point3F pos = mOwnerEntity->mPosition;
      stream->write(pos.x);
      stream->write(pos.y);
      stream->write(pos.z);
      stream->write(mVelocity.x);
      stream->write(mVelocity.y);
      stream->write(mVelocity.z);
   }

   void ControllerComponent::readPacketData(GameConnection *conn, BitStream *stream)
   { 
      Point3F pos;
      stream->read(&pos.x);
      stream->read(&pos.y);
      stream->read(&pos.z);
      stream->read(&mVelocity.x);
      stream->read(&mVelocity.y);
      stream->read(&mVelocity.z);
      mDelta.pos = pos;
   }

   U32 ControllerComponent::packUpdate(NetConnection *con, U32 mask, BitStream *stream)
   {
      if (stream->writeFlag(mUpdatePosition))
      {
         // Write Position
         Point3F pos = mTempPosition;
         stream->writeCompressedPoint(pos);

         // Write Velocity
         F32 len = mVelocity.len();
         if(stream->writeFlag(len > 0.02f))
         {
            Point3F outVel = mVelocity;
            outVel *= 1.0f/len;
            stream->writeNormalVector(outVel, 10);
            len *= 32.0f;  // 5 bits of fraction
            if(len > 8191)
               len = 8191;
            stream->writeInt((S32)len, 13);
         }

         // Write Move Data
         mDelta.move.pack(stream);

         // Write Force Update
         stream->writeFlag(mForceUpdatePosition);

         mUpdatePosition = false;
         mForceUpdatePosition = false;
      }

      return 0;
   }   

   void ControllerComponent::unpackUpdate(NetConnection *con, BitStream *stream)
   {
      if (stream->readFlag()) 
      {
         // Read Position
         Point3F pos;
         stream->readCompressedPoint(&pos);

         // Read Velocity
         F32 speed = mVelocity.len();
         if(stream->readFlag())
         {
            stream->readNormalVector(&mVelocity, 10);
            mVelocity *= stream->readInt(13) / 32.0f;
         }
         else
         {
            mVelocity.set(0.0f, 0.0f, 0.0f);
         }

         // Read Move
         mDelta.move.unpack(stream);

         // Read Force Update
         if (!stream->readFlag())
         {
            // Determine number of ticks to warp based on the average
            // of the client and server velocities.
            mDelta.warpOffset = pos - mDelta.pos;
            F32 as = (speed + mVelocity.len()) * 0.5f * TickSec;
            F32 dt = (as > 0.00001f) ? mDelta.warpOffset.len() / as: sMaxWarpTicks;
            mDelta.warpTicks = (S32)((dt > sMinWarpTicks) ? getMax(mFloor(dt + 0.5f), 1.0f) : 0.0f);

            if (mDelta.warpTicks)
            {
               // Setup the warp to start on the next tick.
               if (mDelta.warpTicks > sMaxWarpTicks)
                  mDelta.warpTicks = sMaxWarpTicks;
               mDelta.warpOffset /= (F32)mDelta.warpTicks;
            }
            else
            {
               // Going to skip the warp, server and client are real close.
               // Adjust the frame interpolation to move smoothly to the
               // new position within the current tick.
               Point3F cp = mDelta.pos + mDelta.posVec * mDelta.dt;
               if (mDelta.dt == 0) 
               {
                  mDelta.posVec.set(0.0f, 0.0f, 0.0f);
                  //mDelta.rotVec.set(0.0f, 0.0f, 0.0f);
               }
               else
               {
                  F32 dti = 1.0f / mDelta.dt;
                  mDelta.posVec = (cp - pos) * dti;
                  //delta.rotVec.z = mRot.z - rot.z;

                  //if(delta.rotVec.z > M_PI_F)
                  //   delta.rotVec.z -= M_2PI_F;
                  //else if(delta.rotVec.z < -M_PI_F)
                  //   delta.rotVec.z += M_2PI_F;

                  //delta.rotVec.z *= dti;
               }
               mDelta.pos = pos;
               //mDelta.rot = rot;
               setPosition(pos); //,rot);
            }
         }
         else 
         {
            // Set the player to the server position
            mDelta.pos = pos;
            //delta.rot = rot;
            mDelta.posVec.set(0.0f, 0.0f, 0.0f);
            //mDelta.rotVec.set(0.0f, 0.0f, 0.0f);
            mDelta.warpTicks = 0;
            mDelta.dt = 0.0f;
            setPosition(pos); //, rot);
         }
      }
   }
}
