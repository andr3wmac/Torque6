//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
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

#include "gui/messageVector.h"
#include "io/fileObject.h"

#include "gui/messageVector_Binding.h"

IMPLEMENT_CONOBJECT(MessageVector);

//--------------------------------------------------------------------------
MessageVector::MessageVector()
{
   VECTOR_SET_ASSOCIATION(mMessageLines);
   VECTOR_SET_ASSOCIATION(mSpectators);
}


//--------------------------------------------------------------------------
MessageVector::~MessageVector()
{
   for (U32 i = 0; i < (U32)mMessageLines.size(); i++) {
      char* pDelete = const_cast<char*>(mMessageLines[i].message);
      delete [] pDelete;
      mMessageLines[i].message = 0;
      mMessageLines[i].messageTag = 0xFFFFFFFF;
   }
   mMessageLines.clear();
}


//--------------------------------------------------------------------------
void MessageVector::initPersistFields()
{
   Parent::initPersistFields();
}



//--------------------------------------------------------------------------
bool MessageVector::onAdd()
{
   return Parent::onAdd();
}


//--------------------------------------------------------------------------
void MessageVector::onRemove()
{
   // Delete all the lines from the observers, and then forcibly detatch ourselves
   //
   for (S32 i = mMessageLines.size() - 1; i >= 0; i--)
      spectatorMessage(LineDeleted, i);
   spectatorMessage(VectorDeletion, 0);
   mSpectators.clear();

   Parent::onRemove();
}


//--------------------------------------------------------------------------
void MessageVector::pushBackLine(const char* newMessage, const S32 newMessageTag)
{
   insertLine(mMessageLines.size(), newMessage, newMessageTag);
}


void MessageVector::popBackLine()
{
   AssertFatal(mMessageLines.size() != 0, "MessageVector::popBackLine: nothing to pop!");
   if (mMessageLines.size() == 0)
      return;

   deleteLine(mMessageLines.size() - 1);
}

void MessageVector::clear()
{
   while(mMessageLines.size())
      deleteLine(mMessageLines.size() - 1);
}

//--------------------------------------------------------------------------
void MessageVector::pushFrontLine(const char* newMessage, const S32 newMessageTag)
{
   insertLine(0, newMessage, newMessageTag);
}


void MessageVector::popFrontLine()
{
   AssertFatal(mMessageLines.size() != 0, "MessageVector::popBackLine: nothing to pop!");
   if (mMessageLines.size() == 0)
      return;

   deleteLine(0);
}


//--------------------------------------------------------------------------
void MessageVector::insertLine(const U32   position,
                               const char* newMessage,
                               const S32   newMessageTag)
{
   AssertFatal(position >= 0 && position <= (U32)mMessageLines.size(), "MessageVector::insertLine: out of range position!");
   AssertFatal(newMessage != NULL, "Error, no message to insert!");

   U32 len = dStrlen(newMessage) + 1;
   char* copy = new char[len];
   dStrcpy(copy, newMessage);

   mMessageLines.insert(position);
   mMessageLines[position].message    = copy;
   mMessageLines[position].messageTag = newMessageTag;

   // Notify of insert
   spectatorMessage(LineInserted, position);
}


//--------------------------------------------------------------------------
void MessageVector::deleteLine(const U32 position)
{
   AssertFatal(position >= 0 && position < (U32)mMessageLines.size(), "MessageVector::deleteLine: out of range position!");

   char* pDelete = const_cast<char*>(mMessageLines[position].message);
   delete [] pDelete;
   mMessageLines[position].message    = NULL;
   mMessageLines[position].messageTag = 0xFFFFFFFF;

   mMessageLines.erase(position);

   // Notify of delete
   spectatorMessage(LineDeleted, position);
}


//--------------------------------------------------------------------------
bool MessageVector::dump( const char* filename, const char* header )
{
   Con::printf( "Dumping message vector %s to %s...", getName(), filename );
   FileObject file;
   if ( !file.openForWrite( filename ) )
      return( false );

   // If passed a header line, write it out first:
   if ( header )
      file.writeLine( (const U8*) header );

   // First write out the record count:
   char* lineBuf = (char*) dMalloc( 10 );
   dSprintf( lineBuf, 10, "%d", mMessageLines.size() );
   file.writeLine( (const U8*) lineBuf );

   // Write all of the lines of the message vector:
   U32 len;
   for ( U32 i = 0; i < (U32)mMessageLines.size(); i++ )
   {
      len = ( dStrlen( mMessageLines[i].message ) * 2 ) + 10;
      lineBuf = (char*) dRealloc( lineBuf, len );
      dSprintf( lineBuf, len, "%d ", mMessageLines[i].messageTag );
      expandEscape( lineBuf + dStrlen( lineBuf ), mMessageLines[i].message );
      file.writeLine( (const U8*) lineBuf );
   }

   file.close();
   return( true );
}


//--------------------------------------------------------------------------
void MessageVector::registerSpectator(SpectatorCallback callBack, void *spectatorKey)
{
   AssertFatal(callBack != NULL, "Error, must have a callback!");

   // First, make sure that this hasn't been registered already...
   U32 i;
   for (i = 0; i < (U32)mSpectators.size(); i++) {
      AssertFatal(mSpectators[i].key != spectatorKey, "Error, spectator key already registered!");
      if (mSpectators[i].key == spectatorKey)
         return;
   }

   mSpectators.increment();
   mSpectators.last().callback = callBack;
   mSpectators.last().key      = spectatorKey;

   // Need to message this spectator of all the lines currently inserted...
   for (i = 0; i < (U32)mMessageLines.size(); i++) {
      (*mSpectators.last().callback)(mSpectators.last().key,
                                     LineInserted, i);
   }
}

void MessageVector::unregisterSpectator(void * spectatorKey)
{
   for (U32 i = 0; i < (U32)mSpectators.size(); i++) {
      if (mSpectators[i].key == spectatorKey) {
         // Need to message this spectator of all the lines currently inserted...
         for (S32 j = mMessageLines.size() - 1; j >= 0 ; j--) {
            (*mSpectators[i].callback)(mSpectators[i].key,
                                       LineDeleted, j);
         }

         mSpectators.erase(i);
         return;
      }
   }

   AssertFatal(false, "MessageVector::unregisterSpectator: tried to unregister a spectator that isn't subscribed!");
   Con::errorf(ConsoleLogEntry::General,
               "MessageVector::unregisterSpectator: tried to unregister a spectator that isn't subscribed!");
}

void MessageVector::spectatorMessage(MessageCode code, const U32 arg)
{
   for (U32 i = 0; i < (U32)mSpectators.size(); i++) {
      (*mSpectators[i].callback)(mSpectators[i].key,
                                 code, arg);
   }
}

