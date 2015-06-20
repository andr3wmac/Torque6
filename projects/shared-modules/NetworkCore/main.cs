function NetworkCore::create( %this )
{
  // Network Prefs
  $pref::Master0 = "2:master.garagegames.com:28002";
  $pref::Net::LagThreshold = 400;
  $pref::Net::Port = 28000;
  $pref::Server::RegionMask = 2;
  $pref::Net::RegionMask = 2;
   
  $pref::Server::Name = "T6 Server";
  $pref::Player::Name = "T6 Player";
   
  // Set up networking
  setNetPort(0);
   
  // Network structure
  exec("./scripts/client/client.cs");
  exec("./scripts/client/message.cs");
  exec("./scripts/client/serverConnection.cs");
   
  exec("./scripts/server/server.cs");
  exec("./scripts/server/message.cs");
  exec("./scripts/server/clientConnection.cs");
  
  // Simple Network
  exec("./scripts/simpleChat.cs");
}

function NetworkCore::destroy()
{
  // 
}
