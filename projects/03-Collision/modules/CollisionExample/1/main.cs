// Players motion component.
$player = 0;
$physics = 0;
$obstacleCount = 0;

function CollisionExample::create(%this)
{     
    // Lighting
    Scene::setDirectionalLight("1 1 -1", "1 1 1", "0.1 0.1 0.1");

    Skybox::load(expandPath("^Skybox/textures/pisa_lod.dds"));
    Skybox::enable();

    // Create Player
    $player = new SceneEntity();
    $player.template = "CollisionExample:CubePlayer";
    $player.position = "0 10 0";
    $player.scale = "20 20 20";
    Scene::addEntity($player, "Cube Player");

    // We want to hold on to the players motion component to move it.
    $physics = $player.findComponentByType("Physics");

    // Load Camera
    exec("./scripts/camera.cs");
    loadCamera();

    // Load Controls
    exec("./scripts/controls.cs");
    loadControls();
    
    // Spawn 4 obstacles to collide with.
    spawnObstacle(" 50 10  50");
    spawnObstacle("-50 10  50");
    spawnObstacle(" 50 10 -50");
    spawnObstacle("-50 10 -50");
}

function CollisionExample::destroy( %this )
{
    
}

function spawnObstacle(%pos)
{
    %ob = new SceneEntity();
    %ob.template = "CollisionExample:Obstacle";
    %ob.position = %pos;
    %ob.scale = "20 20 20";
    %ob.resetColor();
    %ob.resetColorTimer = -1;
    Scene::addEntity(%ob, "Obstacle" @ $obstacleCount);
    $obstacleCount = $obstacleCount + 1;
}

function spawnNewObstacle(%val)
{
    if ( %val )
    {
        spawnObstacle("0 100 0");
    }
}

function SceneEntity::removeSelf( %this )
{
    Scene::removeEntity(%this);
}

function SceneEntity::onCollide ( %this, %hit, %type )
{
    %this.findComponent("Cube").setUniformVec4("cubeColor", "0.2 1.0 0.2 0.0");
    cancel(%this.resetColorTimer);
    %this.resetColorTimer = %this.schedule(1000, "resetColor");
}

function SceneEntity::resetColor ( %this )
{
    %this.findComponent("Cube").setUniformVec4("cubeColor", "0.2 0.2 0.2 0.0");
}
