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

#ifndef _CONTROLLER_COMPONENT_H_
#define _CONTROLLER_COMPONENT_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _BASE_COMPONENT_H_
#include "baseComponent.h"
#endif

#ifndef _MESH_COMPONENT_H_
#include "meshComponent.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

#ifndef _MOVELIST_H_
#include "game/moveList.h"
#endif

namespace Scene 
{
   class DLL_PUBLIC ControllerComponent : public BaseComponent
   {
      private:
         typedef BaseComponent Parent;

      protected:
         // Client-side prediction/interpolation
         struct StateDelta 
         {
            Move     move;                ///< Last move from server
            F32      dt;                  ///< Last interpolation time
            // Interpolation data
            Point3F  pos;
            Point3F  posVec;
            QuatF    rot[2];
            // Warp data
            S32      warpTicks;           ///< Number of ticks to warp
            S32      warpCount;           ///< Current pos in warp
            Point3F  warpOffset;
            QuatF    warpRot[2];
            //
            Point3F  cameraOffset;
            Point3F  cameraVec;
            Point3F  cameraRot;
            Point3F  cameraRotVec;
         };

         StateDelta  mDelta;
         S32         mPredictionCount;    ///< Number of ticks to predict

         Point3F     mVelocity;
         Point3F     mTempPosition;
         bool        mUpdatePosition;
         bool        mForceUpdatePosition;

      public:
         ControllerComponent();

         void onAddToScene();
         void onRemoveFromScene();
         void refresh() { }

         void setPosition(const Point3F& pos);
         void setOwnerPosition(const Point3F& pos);
         void updateMove(const Move* move);

         virtual void processMove( const Move *move );
         virtual void interpolateMove( F32 dt );
         virtual void advanceMove( F32 dt );

         virtual void writePacketData(GameConnection *conn, BitStream *stream);
         virtual void readPacketData (GameConnection *conn, BitStream *stream);
         virtual U32  packUpdate(NetConnection *con, U32 mask, BitStream *stream);
         virtual void unpackUpdate(NetConnection *con, BitStream *stream);

         static void initPersistFields();

         DECLARE_CONOBJECT(ControllerComponent);
   };
}

#endif // _CONTROLLER_COMPONENT_H_