//-----------------------------------------------------------------------------
// Copyright (c) 2014 Andrew Mac
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

function CollisionExample::create(%this)
{     
    Skybox::load(expandPath("^Skybox/textures/pisa_lod.dds"));
    Skybox::enable();

    // Load Camera
    exec("./scripts/camera.cs");
    loadCamera();

    // Load Controls
    exec("./scripts/controls.cs");
    loadControls();
    
    // Spawn 4 obstacles to collide with.
    exec("./scripts/obstacles.cs");
    spawnObstacle(" 50 10  50");
    spawnObstacle("-50 10  50");
    spawnObstacle(" 50 10 -50");
    spawnObstacle("-50 10 -50");
}

function CollisionExample::destroy( %this )
{
    
}

function onClientConnected(%client)
{
    // Create a player
    %player = new SceneEntity();
    %player.template = "CollisionExample:CubePlayer";
    %player.position = "0 10 0";
    %player.scale = "20 20 20";
    %player.ghosted = true;
    Scene::addEntity(%player);

    // Give the new client control of it.
    %client.setControlObject(%player);
    %client.player = %player;
}
