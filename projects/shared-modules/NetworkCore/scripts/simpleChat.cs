// ----------------------
// Client
// ----------------------
function onConnect()
{
    echo("[CLIENT] Connected to server.");
}

function onDisconnect()
{
    echo("[CLIENT] Disconnected from server.");
}

function chat(%text)
{
   commandToServer('Chat', %text); 
}

function clientCmdChat(%name, %text)
{
  echo("[CHAT] " @ %name @ ": " @ %text);
}

// ----------------------
// Server
// ----------------------
function onServerCreated()
{
    echo("[SERVER] Server created.");
}

function onClientConnected()
{
    echo("[SERVER] Client connected.");
}

function broadcast(%text)
{
   %count = ClientGroup.getCount();
   for(%i = 0; %i < %count; %i++)
   {
      %recipient = ClientGroup.getObject(%i);
      commandToClient(%recipient, 'Chat', "SERVER", %text);
   }
}

function serverCmdChat(%client, %text)
{
   %count = ClientGroup.getCount();
   for(%i = 0; %i < %count; %i++)
   {
      %recipient = ClientGroup.getObject(%i);
      commandToClient(%recipient, 'Chat', "CLIENT" @ %i, %text);
   }
}
