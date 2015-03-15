// Players motion component.
$motion = 0;

function CollisionExample::create(%this)
{     
    // Floor
    %example_room = new SceneEntity();
    %example_room.template = "./entities/ExampleRoom.taml";
    %example_room.position = "0 -100 0";
    Scene::addEntity(%example_room, "Floor");

    // Lighting
    Scene::setDirectionalLight("1 1 0", "1 1 1", "0.1 0.1 0.1");

    // Create Player
    %cube_player = new SceneEntity();
    %cube_player.template = "./entities/CubePlayer.taml";
    %cube_player.position = "0 10 0";
    %cube_player.scale = "20 20 20";
    Scene::addEntity(%cube_player, "Cube Player");

    // We want to hold on to the players motion component to move it.
    $motion = %cube_player.findComponentByType("Motion");

    // Load Camera
    exec("./scripts/camera.cs");
    loadCamera();

    // Load Controls
    exec("./scripts/controls.cs");
    loadControls();
    
    // Spawn 4 obstacles to collide with.
    spawnObstacle("50 10 50");
    spawnObstacle("-50 10 50");
    spawnObstacle("50 10 -50");
    spawnObstacle("-50 10 -50");
}

function CollisionExample::destroy( %this )
{
    
}

function spawnObstacle(%pos)
{
    %ob = new SceneEntity();
    %ob.template = "./entities/Obstacle.taml";
    %ob.position = %pos;
    %ob.scale = "20 20 20";
    %ob.resetColor();
    Scene::addEntity(%ob, "Obstacle");
}

function SceneEntity::onCollide ( %this, %hit )
{
    %this.findComponent("Cube").setUniformVec3("cubeColor", "0.2 1.0 0.2");
    %this.schedule(1000, "resetColor");
}

function SceneEntity::resetColor ( %this )
{
    %this.findComponent("Cube").setUniformVec3("cubeColor", "0.2 0.2 0.2");
}
