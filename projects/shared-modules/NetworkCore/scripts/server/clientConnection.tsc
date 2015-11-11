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

//---------------------------------------------------------------------------------------------
// GameConnection.onConnectRequest
// This script function is called before a client connection is accepted.  Returning "" will
// accept the connection, anything else will be sent back as an error to the client. All the
// connect args are passed also to onConnectRequest.
//---------------------------------------------------------------------------------------------
function GameConnection::onConnectRequest(%client, %netAddress)
{
   echo("Connect request from: " @ %netAddress);
   return "";
}

//---------------------------------------------------------------------------------------------
// GameConnection.onConnect
// This function is called when a client connection is accepted.
//---------------------------------------------------------------------------------------------
function GameConnection::onConnect(%client, %name)
{
   // Send down the connection error info. The client is responsible for displaying this
   // message if a connection error occures.
   commandToClient(%client, 'SetConnectionError', $Pref::Server::ConnectionError);
   
   %client.setPlayerName(%name);
   echo("Client Connected: " @ %client @ " " @ %client.getAddress());
   $Server::PlayerCount++;
   
   // Tell everyone about the new guy.
   messageAllExcept(%client, -1, 'ClientJoined', %client);
   
   // Tell the new guy about everyone else.
   %count = ClientGroup.getCount();
   for (%i = 0; %i < %count; %i++)
   {
      %other = ClientGroup.getObject(%i);
      if (%other != %client)
         messageClient(%client, 'ClientJoined', %other);
   }
 
   %client.activateGhosting();

   onClientConnected(%client);
}

//---------------------------------------------------------------------------------------------
// GameConnection.onDrop
// This function is called when a client drops for any reason.
//---------------------------------------------------------------------------------------------
function GameConnection::onDrop(%client, %reason)
{
   removeTaggedString(%client.name);
   echo("Client Dropped: " @ %client @ " " @ %client.getAddress());
   $Server::PlayerCount--;
   
   messageAllExcept(%client, -1, 'ClientDropped', %client);
   
   onClientDropped(%client);
}

//---------------------------------------------------------------------------------------------
// GameConnection.setPlayerName
// Sets %clients name based on %name. If %name is already used, a number is appended or
// incremented until the name is unique.
//---------------------------------------------------------------------------------------------
function GameConnection::setPlayerName(%client, %name)
{
   echo("name = " @ %name);
   
   // If nothing was passed, set a default name.
   %name = trim(%name);
   if (%name $= "")
      %name = "T6 Gamer";

   // Make sure the name is unique, we'll hit something eventually.
   %nameTest = %name;
   %count = 0;
   while(!isNameUnique(%nameTest))
   {
      %count++;
      %nameTest = %name @ %count;
   }

   // Set the name.
   %client.name = %nameTest;
}

//---------------------------------------------------------------------------------------------
// isNameUnique
// Checks if %name is not used by any clients.
//---------------------------------------------------------------------------------------------
function isNameUnique(%name)
{
   %count = ClientGroup.getCount();
   for (%i = 0; %i < %count; %i++)
   {
      %client = ClientGroup.getObject(%i);
      %test = %client.name;
      if (strcmp(%name, %test) == 0)
         return false;
   }
   
   // If we get here, the name is unique.
   return true;
}
