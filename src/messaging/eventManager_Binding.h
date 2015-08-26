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

#include "c-interface/c-interface.h"

ConsoleMethodGroupBeginWithDocs(EventManager, SimObject)

/*! 
    Register an event with the event manager.
    @param event The event to register.
    @return Whether or not the event was registered successfully.
*/
ConsoleMethodWithDocs( EventManager, registerEvent, ConsoleBool, 3, 3, ( String event ))
{
   return object->registerEvent( argv[2] );
}

/*! 
    Remove an event from the EventManager.
    @param event The event to remove.
*/
ConsoleMethodWithDocs( EventManager, unregisterEvent, ConsoleVoid, 3, 3, ( String event ))
{
   object->unregisterEvent( argv[2] );
}

/*! 
    Check if an event is registered or not.
    @param event The event to check.
    @return Whether or not the event exists.
*/
ConsoleMethodWithDocs( EventManager, isRegisteredEvent, ConsoleBool, 3, 3, ( String event ))
{
   return object->isRegisteredEvent( argv[2] );
}

/*! 
    Trigger an event.
    @param event The event to trigger.
    @param data The data associated with the event.
    @return Whether or not the event was dispatched successfully.
*/
ConsoleMethodWithDocs( EventManager, postEvent, ConsoleBool, 3, 4, ( String event, String data ))
{
   return object->postEvent( argv[2], argc > 3 ? argv[3] : "" );
}

/*! 
    Subscribe a listener to an event.
    @param listener The listener to subscribe.
    @param event The event to subscribe to.
    @param callback Optional method name to receive the event notification. If this is not specified, \on[event]\ will be used.
    @return Whether or not the subscription was successful.
*/
ConsoleMethodWithDocs( EventManager, subscribe, ConsoleBool, 4, 5, ( SimObject listener, String event, String callback ))
{
   // Find the listener object.
   SimObject *cbObj = dynamic_cast<SimObject *>(Sim::findObject(argv[2]));
   if( cbObj == NULL )
   {
      Con::warnf( "EventManager::subscribe - Invalid listener." );
      return false;
   }

   return object->subscribe( cbObj, argv[3], argc > 4 ? argv[4] : NULL );
}

/*! 
    Remove a listener from an event.
    @param listener The listener to remove.
    @param event The event to be removed from.
*/
ConsoleMethodWithDocs( EventManager, remove, ConsoleVoid, 4, 4, ( SimObject listener, String event ))
{
   // Find the listener object.
   SimObject * listener = dynamic_cast< SimObject * >( Sim::findObject( argv[2] ) );
   if( listener )
      object->remove( listener, argv[3] );
}

/*! 
    Print all registered events to the console.
*/
ConsoleMethodWithDocs( EventManager, dumpEvents, ConsoleVoid, 2, 2, ())
{
   object->dumpEvents();
}

/*! 
    Print all subscribers to an event to the console.
    @param event The event whose subscribers are to be printed. If this parameter isn't specified, all events will be dumped.
*/
ConsoleMethodWithDocs( EventManager, dumpSubscribers, ConsoleVoid, 2, 3, ( String event ))
{
   if( argc > 2 )
      object->dumpSubscribers( argv[2] );
   else
      object->dumpSubscribers();
}

ConsoleMethodGroupEndWithDocs(EventManager)

extern "C" {
   DLL_PUBLIC EventManager* EventManagerCreateInstance()
   {
      return new EventManager();
   }

   DLL_PUBLIC const char* EventManagerGetQueue(EventManager* eventManager)
   {
      return CInterface::GetMarshallableString(eventManager->getMessageQueue());
   }

   DLL_PUBLIC void EventManagerSetQueue(EventManager* eventManager, const char* value)
   {
      eventManager->setMessageQueue(value);
   }

   DLL_PUBLIC bool EventManagerRegisterEvent(EventManager* eventManager, const char* eventName)
   {
      return eventManager->registerEvent(eventName);
   }

   DLL_PUBLIC void EventManagerUnregisterEvent(EventManager* eventManager, const char* eventName)
   {
      eventManager->unregisterEvent(eventName);
   }

   DLL_PUBLIC bool EventManagerIsRegisteredEvent(EventManager* eventManager, const char* eventName)
   {
      return eventManager->isRegisteredEvent(eventName);
   }

   DLL_PUBLIC bool EventManagerPostEvent(EventManager* eventManager, const char* eventName, const char* data)
   {
      return eventManager->postEvent(eventName, data);
   }

   DLL_PUBLIC bool EventManagerSubscribe(EventManager* eventManager, SimObject* listener, const char* eventName, const char* callback)
   {
      return eventManager->subscribe(listener, eventName, callback);
   }

   DLL_PUBLIC void EventManagerRemove(EventManager* eventManager, SimObject* listener, const char* eventName)
   {
      eventManager->remove(listener, eventName);
   }

   DLL_PUBLIC void EventManagerDumpEvents(EventManager* eventManager)
   {
      eventManager->dumpEvents();
   }

   DLL_PUBLIC void EventManagerDumpSubscribers(EventManager* eventManager, const char* eventName)
   {
      if (eventName)
         eventManager->dumpSubscribers(eventName);
      else   
         eventManager->dumpSubscribers();
   }
}