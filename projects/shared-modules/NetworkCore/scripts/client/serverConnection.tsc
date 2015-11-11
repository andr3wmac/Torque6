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
// SetConnectionError
// Upon connecting to a server, the server will call this function on the client to describe a 
// connection error. If there is no error, this will still be called, but %error will be
// empty. All we do here is save the error message for display when the server reports the
// actual connection error.
//---------------------------------------------------------------------------------------------
function clientCmdSetConnectionError(%error)
{
   $ServerConnectionErrorMessage = %error;
}

//---------------------------------------------------------------------------------------------
// GameConnection.onConnectionAccepted
// Called when a connection is established and accepted.
//---------------------------------------------------------------------------------------------
function GameConnection::onConnectionAccepted(%this)
{
   onConnect();
}

//---------------------------------------------------------------------------------------------
// GameConnection.onConnectionTimedOut
// A connection was established with the server, but has since timed out.
//---------------------------------------------------------------------------------------------
function GameConnection::onConnectionTimedOut(%this)
{
   disconnectedCleanup();
   MessageBoxOK("TIMED OUT", "The server connection has timed out.");
}

//---------------------------------------------------------------------------------------------
// GameConnection.onConnectionTimedOut
// A connection was established with the server, but has since been dropped.
//---------------------------------------------------------------------------------------------
function GameConnection::onConnectionDropped(%this, %msg)
{
   disconnectedCleanup();
   MessageBoxOK("DISCONNECT", "The server has dropped the connection: " @ %msg);
}

//---------------------------------------------------------------------------------------------
// GameConnection.onConnectionTimedOut
// A connection was established with the server, but some error (sent to 'SetConnectionError')
// has occured.
//---------------------------------------------------------------------------------------------
function GameConnection::onConnectionError(%this, %msg)
{
   disconnectedCleanup();
   MessageBoxOK("DISCONNECT", $ServerConnectionErrorMessage @ " (" @ %msg @ ")");
}


//---------------------------------------------------------------------------------------------
// GameConnection.onConnectionTimedOut
// A connection was not able to be made with the server because the server rejected it.
//---------------------------------------------------------------------------------------------
function GameConnection::onConnectRequestRejected(%this, %msg)
{
   switch$(%msg)
   {
      case "CHR_PASSWORD":
         if ($Client::Password $= "")
            %error = "The server requires a password.";
         else
            %error = "The password you entered is incorrect.";
            
      case "CHR_PROTOCOL":
         %error = "Incompatible protocol version: Your game version is not compatible with this server.";
         
      default:
         %error = "Connection error. Please try another server. Error code: (" @ %msg @ ")";
   }
   
   disconnectedCleanup();
   MessageBoxOK("REJECTED", %error);
}

//---------------------------------------------------------------------------------------------
// GameConnection.onConnectionRequestTimedOut
// A connection was not able to be made with the server and the request timed out.
//---------------------------------------------------------------------------------------------
function GameConnection::onConnectRequestTimedOut(%this)
{
   disconnectedCleanup();
   MessageBoxOK("TIMED OUT", "Your connection to the server timed out.");
}
