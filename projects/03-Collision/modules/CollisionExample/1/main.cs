// Players motion component.
$player = 0;
$physics = 0;

function CollisionExample::create(%this)
{     
    // Floor
    %example_room = new SceneEntity();
    %example_room.template = "CollisionExample:ExampleRoom";
    %example_room.position = "0 -100 0";
    Scene::addEntity(%example_room, "Floor");

    // Lighting
    Scene::setDirectionalLight("1 1 -1", "1 1 1", "0.1 0.1 0.1");

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
    spawnObstacle("1", "50 10 50");
    spawnObstacle("2", "-50 10 50");
    spawnObstacle("3", "50 10 -50");
    spawnObstacle("4", "-50 10 -50");
}

function CollisionExample::destroy( %this )
{
    
}

function spawnProjectile(%val)
{
    if ( %val )
    {
        %ob = new SceneEntity();
        %ob.template = "Projectile";
        %ob.position = $player.getPosition();
        %ob.scale = "20 5 5";

        if ( $lookDir $= "0 0 -1" || $lookDir $= "0 0 1" )
        {
            %ob.rotation = "0 1.57 0";
        }

        %ob.schedule(3000, "removeSelf");
        Scene::addEntity(%ob, "Projectile");

        %ob_physics = %ob.findComponentByType("Physics");
        %ob_physics.setLinearVelocity($lookDir);
    }
}

function spawnObstacle(%num, %pos)
{
    %ob = new SceneEntity();
    %ob.template = "CollisionExample:Obstacle";
    %ob.position = %pos;
    %ob.scale = "20 20 20";
    %ob.resetColor();
    %ob.resetColorTimer = -1;
    Scene::addEntity(%ob, "Obstacle" @ %num);
}

function SceneEntity::removeSelf( %this )
{
    Scene::removeEntity(%this);
}

function SceneEntity::onCollide ( %this, %hit, %type )
{
    if ( %type $= "Projectile" )
    {
        Scene::removeEntity(%this);
        Scene::removeEntity(%hit);
    } else {
        %this.findComponent("Cube").setUniformVec3("cubeColor", "0.2 1.0 0.2");
        cancel(%this.resetColorTimer);
        %this.resetColorTimer = %this.schedule(1000, "resetColor");
    }
}

function SceneEntity::resetColor ( %this )
{
    %this.findComponent("Cube").setUniformVec3("cubeColor", "0.2 0.2 0.2");
}
