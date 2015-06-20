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
// createServer
// Creates a server and connects to it locally.
//---------------------------------------------------------------------------------------------
$serverCreated = false;
function createServer(%displayOnMaster)
{
   // Destroy a potentially already created server.
   destroyServer();

   // Make sure the network port is set correctly.
   %failCount = 0;
   %port = $pref::net::port;
   while(%failCount < 10 && !setNetPort(%port))
   {
      echo("Port init failed on port " @ %port @ " trying next port.");
      %port++;
      %failCount++;
   }
   
   allowConnections(true);
   if (%displayOnMaster)
      schedule(0, 0, startHeartbeat);
   
   onServerCreated();
      
   %conn = new GameConnection(ServerConnection);
   %conn.setConnectArgs($pref::Player::name);
   %conn.connectLocal();
   
   $serverConnected = true;
   $serverLocal = true;
   $serverCreated = true;
}

//---------------------------------------------------------------------------------------------
// destroyServer
// Destroys the server.
//---------------------------------------------------------------------------------------------
function destroyServer()
{
   // Can't destroy what isn't created.
   if(!$serverCreated)
      return;
      
   $serverCreated = false;
      
   allowConnections(false);
   stopHeartbeat();

   // Delete all the clients.
   while (ClientGroup.getCount())
   {
      %client = ClientGroup.getObject(0);
      %client.delete();
   }
   
   echo("Destroy Server");
   
   // Clean up.
   purgeResources();
   onServerDestroyed();
}

//---------------------------------------------------------------------------------------------
// onServerInfoQuery
//---------------------------------------------------------------------------------------------
function onServerInfoQuery()
{
   return "Doing OK";
}