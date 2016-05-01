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

#ifndef SIM_PUBLISHER_H
#define SIM_PUBLISHER_H

#ifndef _SIM_EVENT_H_
#include "sim/simEvent.h"
#endif

#ifndef HASHTABLE_H
#include "collection/hashTable.h"
#endif

//-----------------------------------------------------------------------------
// Publisher/Subscriber Event System.
//-----------------------------------------------------------------------------

class SimPublisher
{
   protected:
      struct SimSubscriber
      {
         SimObject*        object;
         StringTableEntry  function;
      };

      typedef Vector<SimSubscriber> typeSubsciberVector;
      typedef HashMap<StringTableEntry, typeSubsciberVector> typeSubsciberHash;

      typeSubsciberHash mSubscriberTable;
      SimObject*        mOwnerObject;

   public:
      SimPublisher(SimObject* owner);
      ~SimPublisher();

      void subscribe(StringTableEntry eventName, SimObject* subscriberObject, StringTableEntry subscriberFunction);
      void publish(StringTableEntry eventName, S32 argc, const char **argv);
};

class SimSubscibeEvent : public SimEvent
{
   protected:
      StringTableEntry  mEventName;
      SimObject*        mSubscriberObject;
      StringTableEntry  mSubscriberFunction;

   public:
      SimSubscibeEvent(StringTableEntry eventName, SimObject* subscriberObject, StringTableEntry subscriberFunction);
      ~SimSubscibeEvent();

      virtual void process(SimObject *object);
};

class SimPublishEvent : public SimEvent
{
   protected:
      StringTableEntry  mEventName;
      S32               mArgc;
      char**            mArgv;

   public:
      SimPublishEvent(StringTableEntry eventName, S32 argc, const char **argv);
      ~SimPublishEvent();

      virtual void process(SimObject *object);
};

#endif // SIM_PUBLISHER_H