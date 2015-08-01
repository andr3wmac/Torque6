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

#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#ifndef _PROCESSLIST_H_
#include "processList.h"
#endif

#ifndef _NETOBJECT_H_
#include "network/netObject.h"
#endif

#ifndef _TICKCACHE_H_
#include "tickCache.h"
#endif

class NetConnection;
struct Move;
class GameConnection;
class NetObject;
class MoveList;

// For truly it is written: "The wise man extends GameObject for his purposes,
// while the fool has the ability to eject shell casings from the belly of his
// dragon." -- KillerBunny

/// Base class for game objects which use datablocks, networking, are editable,
/// and need to process ticks.
///
/// @section GameObject_process GameObject and ProcessList
///
/// GameObject adds two kinds of time-based updates. Torque works off of a concept
/// of ticks. Ticks are slices of time 32 milliseconds in length. There are three
/// methods which are used to update GameObject objects that are registered with
/// the ProcessLists:
///      - processTick(Move*) is called on each object once for every tick, regardless
///        of the "real" framerate.
///      - interpolateTick(float) is called on client objects when they need to interpolate
///        to match the next tick.
///      - advanceTime(float) is called on client objects so they can do time-based behaviour,
///        like updating animations.
///
/// Torque maintains a server and a client processing list; in a local game, both
/// are populated, while in multiplayer situations, either one or the other is
/// populated.
///
/// You can control whether an object is considered for ticking by means of the
/// setProcessTick() method.
///
/// @section GameObject_networking GameObject and Networking
///
/// writePacketData() and readPacketData() are called to transfer information needed for client
/// side prediction. They are usually used when updating a client of its control object state.
///
/// Subclasses of GameObject usually transmit positional and basic status data in the packUpdate()
/// functions, while giving velocity, momentum, and similar state information in the writePacketData().
///
/// writePacketData()/readPacketData() are called <i>in addition</i> to packUpdate/unpackUpdate().
///
/// @nosubgrouping
class DLL_PUBLIC GameObject : public NetObject, public ProcessObject
{      
   typedef NetObject Parent;

   TickCache mTickCache;
   
   // Control interface
   GameConnection* mControllingClient;

protected:

#ifdef TORQUE_DEBUG_NET_MOVES
   U32 mLastMoveId;
   U32 mTicksSinceLastMove;
   bool mIsAiControlled;
#endif   

public:

   GameObject();
   ~GameObject();

   // net flags added by game base
   enum
   {
      NetOrdered        = BIT(Parent::MaxNetFlagBit+1), /// Process in same order on client and server.
      NetNearbyAdded    = BIT(Parent::MaxNetFlagBit+2), /// Is set during client catchup when neighbors have been checked.
      GhostUpdated      = BIT(Parent::MaxNetFlagBit+3), /// Is set whenever ghost updated (and reset) on the client, for hifi objects.
      TickLast          = BIT(Parent::MaxNetFlagBit+4), /// Tick this object after all others.
      NewGhost          = BIT(Parent::MaxNetFlagBit+5), /// This ghost was just added during the last update.
      HiFiPassive       = BIT(Parent::MaxNetFlagBit+6), /// Do not interact with other hifi passive objects.
      MaxNetFlagBit     = Parent::MaxNetFlagBit+6
   };

   /// @name Inherited Functionality.
   /// @{

   bool onAdd();
   void onRemove();

   /// @}

   // ProcessObject override
   void processMove( const Move *move ); 

   /// @name GameObject NetFlags & Hifi-Net Interface   
   /// @{
   
   /// Set or clear the GhostUpdated bit in our NetFlags.
   /// @see GhostUpdated
   void setGhostUpdated( bool b ) { if (b) mNetFlags.set(GhostUpdated); else mNetFlags.clear(GhostUpdated); }

   /// Returns true if the GhostUpdated bit in our NetFlags is set.
   /// @see GhostUpdated
   bool isGhostUpdated() const { return mNetFlags.test(GhostUpdated); }

   /// Set or clear the NewGhost bit in our NetFlags.
   /// @see NewGhost
   void setNewGhost( bool n ) { if (n) mNetFlags.set(NewGhost); else mNetFlags.clear(NewGhost); }

   /// Returns true if the NewGhost bit in out NetFlags is set.
   /// @see NewGhost
   bool isNewGhost() const { return mNetFlags.test(NewGhost); }

   /// Set or clear the NetNearbyAdded bit in our NetFlags.
   /// @see NetNearbyAdded
   void setNetNearbyAdded( bool b ) { if (b) mNetFlags.set(NetNearbyAdded); else mNetFlags.clear(NetNearbyAdded); }

   /// Returns true if the NetNearby bit in our NetFlags is set.
   /// @see NetNearbyAdded
   bool isNetNearbyAdded() const { return mNetFlags.test(NetNearbyAdded); }

   /// Returns true if the HiFiPassive bit in our NetFlags is set.
   /// @see HiFiPassive
   bool isHifiPassive() const { return mNetFlags.test(HiFiPassive); }

   /// Returns true if the TickLast bit in our NetFlags is set.
   /// @see TickLast
   bool isTickLast() const { return mNetFlags.test(TickLast); }

   /// Returns true if the NetOrdered bit in our NetFlags is set.
   /// @see NetOrdered
   bool isNetOrdered() const { return mNetFlags.test(NetOrdered); }
   
   /// Called during client catchup under the hifi-net model.
   virtual void computeNetSmooth( F32 backDelta ) {}

   /// Returns TickCache used under the hifi-net model.
   TickCache& getTickCache() { return mTickCache; }
   /// @}

   /// @name Network
   /// @see NetObject, NetConnection
   /// @{

   U32  packUpdate  ( NetConnection *conn, U32 mask, BitStream *stream );
   void unpackUpdate( NetConnection *conn,           BitStream *stream );

   /// Write state information necessary to perform client side prediction of an object.
   ///
   /// This information is sent only to the controlling object. For example, if you are a client
   /// controlling a Player, the server uses writePacketData() instead of packUpdate() to
   /// generate the data you receive.
   ///
   /// @param   conn     Connection for which we're generating this data.
   /// @param   stream   Bitstream for output.
   virtual void writePacketData( GameConnection *conn, BitStream *stream );

   /// Read data written with writePacketData() and update the object state.
   ///
   /// @param   conn    Connection for which we're generating this data.
   /// @param   stream  Bitstream to read.
   virtual void readPacketData( GameConnection *conn, BitStream *stream );

   /// Gets the checksum for packet data.
   ///
   /// Basically writes a packet, does a CRC check on it, and returns
   /// that CRC.
   ///
   /// @see writePacketData
   /// @param   conn   Game connection
   virtual U32 getPacketDataChecksum( GameConnection *conn );
   ///@}

public:

   /// @}

   /// @name User control
   /// @{

   /// Returns the client controlling this object
   GameConnection *getControllingClient() { return mControllingClient; }
   const GameConnection *getControllingClient() const { return mControllingClient; }

   /// Returns the MoveList of the client controlling this object.
   /// If there is no client it returns NULL;
   MoveList* getMoveList();

   /// Sets the client controlling this object
   /// @param  client   Client that is now controlling this object
   virtual void setControllingClient( GameConnection *client );

   /// @}
   
   #ifdef TORQUE_DEBUG_NET_MOVES
   bool isAIControlled() const { return mIsAiControlled; }
   #endif

   DECLARE_CONOBJECT (GameObject );

public:

   void setProcessTick( bool t );
   ProcessList* getProcessList() const;
};


#endif // _GAMEOBJECT_H_
