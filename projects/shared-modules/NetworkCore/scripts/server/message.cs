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
// Various server side messaging commands to send messages to everyone, all clients on a team,
// or specific clients.
//---------------------------------------------------------------------------------------------
function messageClient(%client, %msgType, %msgString)
{
   commandToClient(%client, 'ServerMessage', %msgType, %msgString);
}

function messageTeam(%team, %msgType, %msgString)
{
   %count = ClientGroup.getCount();
   for(%i = 0; %i < %count; %i++)
   {
      %recipient = ClientGroup.getObject(%i);
      if(%recipient.team == %team)
         messageClient(%recipient, %msgType, %msgString);
   }
}

function messageTeamExcept(%client, %msgType, %msgString)
{
   %team = %client.team;
   %count = ClientGroup.getCount();
   for(%i = 0; %i < %count; %i++)
   {
      %recipient = ClientGroup.getObject(%i);
      if((%recipient.team == %team) && (%recipient != %client))
         messageClient(%recipient, %msgType, %msgString);
   }
}

function messageAll(%msgType, %msgString)
{
   %count = ClientGroup.getCount();
   for(%i = 0; %i < %count; %i++)
   {
      %recipient = ClientGroup.getObject(%i);
      messageClient(%recipient, %msgType, %msgString);
   }
}

function messageAllExcept(%client, %team, %msgtype, %msgString)
{
   // To ignore %client or %team, pass in -1 for it.
   %count = ClientGroup.getCount();
   for(%i = 0; %i < %count; %i++)
   {
      %recipient = ClientGroup.getObject(%i);
      if((%recipient != %client) && (%recipient.team != %team))
         messageClient(%recipient, %msgType, %msgString);
   }
}

//---------------------------------------------------------------------------------------------
// Spam protection for chat.
//---------------------------------------------------------------------------------------------
$SPAM_PROTECTION_PERIOD = 10000;
$SPAM_MESSAGE_THRESHOLD = 4;
$SPAM_PENALTY_PERIOD = 10000;
$SPAM_MESSAGE = '\c3FLOOD PROTECTION:\cr You must wait another %1 seconds.';

function GameConnection::spamMessageTimeout(%this)
{
   if(%this.spamMessageCount > 0)
      %this.spamMessageCount--;
}

function GameConnection::spamReset(%this)
{
   %this.isSpamming = false;
}

function spamAlert(%client)
{
   if($Pref::Server::FloodProtectionEnabled != true)
      return(false);

   if(!%client.isSpamming && (%client.spamMessageCount >= $SPAM_MESSAGE_THRESHOLD))
   {
      %client.spamProtectStart = getSimTime();
      %client.isSpamming = true;
      %client.schedule($SPAM_PENALTY_PERIOD, spamReset);
   }

   if(%client.isSpamming)
   {
      %wait = mFloor(($SPAM_PENALTY_PERIOD - (getSimTime() - %client.spamProtectStart)) / 1000);
      messageClient(%client, "", $SPAM_MESSAGE, %wait);
      return true;
   }

   %client.spamMessageCount++;
   %client.schedule($SPAM_PROTECTION_PERIOD, spamMessageTimeout);
   return false;
}

function chatMessageClient(%client, %sender, %msgString)
{
   if (!%client.muted[%sender])
      commandToClient(%client, 'ChatMessage', %sender, %msgString);
}

function chatMessageTeam( %sender, %team, %msgString)
{
   if ((%msgString $= "") || spamAlert(%sender))
      return;
   
   %count = ClientGroup.getCount();
   for (%i = 0; %i < %count; %i++)
   {
      %receiver = ClientGroup.getObject( %i );
      if (%receiver.team == %sender.team)
         chatMessageClient(%receiver, %sender, %msgString);
   }
}

function chatMessageAll(%sender, %msgString)
{
   if ((%msgString $= "") || spamAlert(%sender))
      return;
      
   %count = ClientGroup.getCount();
   for (%i = 0; %i < %count; %i++)
   {
      %receiver = ClientGroup.getObject(%i);
      chatMessageClient(%receiver, %sender, %msgString);
   }
}
