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

function addMessageCallback(%msgType, %func)
{
   $MSGCB[%msgType] = %func;
}

function defaultMessageCallback(%msgString)
{
   //onServerMessage(detag(%msgString));
}

addMessageCallback('ClientJoined', clientJoined);
function clientJoined(%client)
{
}

addMessageCallback('ClientDropped', clientDropped);
function clientDropped(%client)
{
}

//---------------------------------------------------------------------------------------------
// ServerMessage
// Called from the server to pass a message to the client. %msgType is a message type that
// can be associated by the client with a callback
//---------------------------------------------------------------------------------------------
function clientCmdServerMessage(%msgType, %msgString)
{
   if ($MSGCB[%msgType] !$= "")
   {
      call($MSGCB[%msgType], %msgString);
      return;
   }
   
   // If nothing has handled this message yet, send it to the default callback.
   defaultMessageCallback(%msgString);
}

//---------------------------------------------------------------------------------------------
// ChatMessage
// Receives a chat message.
//---------------------------------------------------------------------------------------------
function clientCmdChatMessage(%sender, %message)
{
   onChatMessage(%sender, detag(%message));
}
