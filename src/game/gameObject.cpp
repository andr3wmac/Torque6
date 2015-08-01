//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
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
#include "gameObject.h"
#include "gameConnection.h"
#include "console/consoleTypes.h"
#include "console/consoleInternal.h"
#include "io/bitStream.h"
#include "network/netConnection.h"
#include "math/mathIO.h"
#include "moveManager.h"
#include "gameProcess.h"

//----------------------------------------------------------------------------
IMPLEMENT_CO_NETOBJECT_V1(GameObject);

GameObject::GameObject()
: mControllingClient( NULL )
{
   mNetFlags.set(Ghostable);
   //mTypeMask |= GameObjectObjectType;
   mProcessTag = 0;

   // From ProcessObject   
   mIsGameObject = true;
   
#ifdef TORQUE_DEBUG_NET_MOVES
   mLastMoveId = 0;
   mTicksSinceLastMove = 0;
   mIsAiControlled = false;
#endif
}

GameObject::~GameObject()
{
}


//----------------------------------------------------------------------------

bool GameObject::onAdd()
{
   if ( !Parent::onAdd() )
      return false;

   setProcessTick( true );

   return true;
}

void GameObject::onRemove()
{
   Parent::onRemove();
}

//----------------------------------------------------------------------------

void GameObject::processMove(const Move * move)
{
#ifdef TORQUE_DEBUG_NET_MOVES
   if (!move)
      mTicksSinceLastMove++;

   const char * srv = isClientObject() ? "client" : "server";
   const char * who = "";
   if (isClientObject())
   {
      if (this == (GameObject*)GameConnection::getConnectionToServer()->getControlObject())
         who = " player";
      else
         who = " ghost";
      if (mIsAiControlled)
         who = " ai";
   }
   if (isServerObject())
   {
      if (dynamic_cast<AIConnection*>(getControllingClient()))
      {
         who = " ai";
         mIsAiControlled = true;
      }
      else if (getControllingClient())
      {
         who = " player";
         mIsAiControlled = false;
      }
      else
      {
         who = "";
         mIsAiControlled = false;
      }
   }
   U32 moveid = mLastMoveId+mTicksSinceLastMove;
   if (move)
      moveid = move->id;

   if (getTypeMask() & GameObjectHiFiObjectType)
   {
      if (move)
         Con::printf("Processing (%s%s id %i) move %i",srv,who,getId(), move->id);
      else
         Con::printf("Processing (%s%s id %i) move %i (%i)",srv,who,getId(),mLastMoveId+mTicksSinceLastMove,mTicksSinceLastMove);
   }

   if (move)
   {
      mLastMoveId = move->id;
      mTicksSinceLastMove=0;
   }
#endif
}

//----------------------------------------------------------------------------

void GameObject::setControllingClient(GameConnection* client)
{
   mControllingClient = client;
}

U32 GameObject::getPacketDataChecksum(GameConnection * connection)
{
   // just write the packet data into a buffer
   // then we can CRC the buffer.  This should always let us
   // know when there is a checksum problem.

   static U8 buffer[1500] = { 0, };
   BitStream stream(buffer, sizeof(buffer));

   writePacketData(connection, &stream);
   U32 byteCount = stream.getPosition();
   U32 ret = calculateCRC(buffer, byteCount, 0xFFFFFFFF);
   dMemset(buffer, 0, byteCount);
   return ret;
}

void GameObject::writePacketData(GameConnection*, BitStream*)
{
}

void GameObject::readPacketData(GameConnection*, BitStream*)
{
}

U32 GameObject::packUpdate( NetConnection *connection, U32 mask, BitStream *stream )
{
   U32 retMask = Parent::packUpdate( connection, mask, stream );

#ifdef TORQUE_DEBUG_NET_MOVES
   stream->write(mLastMoveId);
   stream->writeFlag(mIsAiControlled);
#endif

   return retMask;
}

void GameObject::unpackUpdate(NetConnection *con, BitStream *stream)
{
   Parent::unpackUpdate( con, stream );

#ifdef TORQUE_DEBUG_NET_MOVES
   stream->read(&mLastMoveId);
   mTicksSinceLastMove = 0;
   mIsAiControlled = stream->readFlag();
#endif
}

MoveList* GameObject::getMoveList()
{ 
   return mControllingClient ? mControllingClient->mMoveList : NULL; 
}

void GameObject::setProcessTick( bool t )
{
   if ( t == mProcessTick )
      return;

   if ( mProcessTick )
   {
      plUnlink();
      mProcessTick = false;
   }
   else
   {
      // Just to be sure...
      plUnlink();

      getProcessList()->addObject( this );

      mProcessTick = true;  
   }   
}

//-----------------------------------------------------------------------------

ProcessList* GameObject::getProcessList() const
{
   if ( isClientObject() )      
      return ClientProcessList::get();
   else
      return ServerProcessList::get();
}
