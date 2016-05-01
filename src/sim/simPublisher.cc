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

#include "sim/simPublisher.h"
#include "platform/platform.h"
#include "console/consoleInternal.h"

//-----------------------------------------------------------------------------
// Publisher/Subscriber Event System.
//-----------------------------------------------------------------------------

SimPublisher::SimPublisher(SimObject* owner)
   : mOwnerObject(owner)
{
   //
}

SimPublisher::~SimPublisher()
{
   //
}

void SimPublisher::subscribe(StringTableEntry eventName, SimObject* subscriberObject, StringTableEntry subscriberFunction)
{
   SimSubscriber sub;
   sub.object     = subscriberObject;
   sub.function   = subscriberFunction;

   typeSubscriberHash::iterator itr = mSubscriberTable.find(eventName);
   if (itr != mSubscriberTable.end())
   {
      (*itr).value.push_back(sub);
   }
   else 
   {
      typeSubscriberVector newEntry;
      newEntry.push_back(sub);
      mSubscriberTable.insert(eventName, newEntry);
   }
}

void SimPublisher::publish(StringTableEntry eventName, S32 argc, const char **argv)
{
   S32 finalArgc = argc + 3;
   const char** finalArgv = new const char*[finalArgc];
   finalArgv[2] = Con::getIntArg(mOwnerObject->getId());
   for (S32 n = 0; n < argc; ++n)
      finalArgv[n + 3] = argv[n];

   typeSubscriberHash::iterator itr = mSubscriberTable.find(eventName);
   if (itr != mSubscriberTable.end())
   {
      typeSubscriberVector* vec = &(*itr).value;
      for (S32 n = 0; n < vec->size(); ++n)
      {
         SimSubscriber* sub = &vec->at(n);
         finalArgv[0] = sub->function;
         finalArgv[1] = Con::getIntArg(sub->object->getId());
         Con::execute(vec->at(n).object, finalArgc, finalArgv);
      }
   }

   delete[] finalArgv;
}

//-----------------------------------------------------------------------------

SimSubscribeEvent::SimSubscribeEvent(StringTableEntry eventName, SimObject* subscriberObject, StringTableEntry subscriberFunction)
{
   mEventName           = eventName;
   mSubscriberObject    = subscriberObject;
   mSubscriberFunction  = subscriberFunction;
}

SimSubscribeEvent::~SimSubscribeEvent()
{

}

void SimSubscribeEvent::process(SimObject* object)
{
   SimPublisher* publisher = object->getPublisher();
   publisher->subscribe(mEventName, mSubscriberObject, mSubscriberFunction);
}

//-----------------------------------------------------------------------------

SimPublishEvent::SimPublishEvent(StringTableEntry eventName, S32 argc, const char **argv)
{
   mEventName = eventName;

   mArgc = argc;
   U32 totalSize = 0;
   S32 i;
   for (i = 0; i < argc; i++)
      totalSize += dStrlen(argv[i]) + 1;
   totalSize += sizeof(char *) * argc;

   mArgv = (char **)dMalloc(totalSize);
   char *argBase = (char *)&mArgv[argc];

   for (i = 0; i < argc; i++)
   {
      mArgv[i] = argBase;
      dStrcpy(mArgv[i], argv[i]);
      argBase += dStrlen(argv[i]) + 1;
   }
}

SimPublishEvent::~SimPublishEvent()
{
   dFree(mArgv);
}

void SimPublishEvent::process(SimObject* object)
{
   SimPublisher* publisher = object->getPublisher();
   publisher->publish(mEventName, mArgc, const_cast<const char**>(mArgv));
}
