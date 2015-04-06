function AnimatedMeshExample::create(%this)
{     
    %entity1 = new SceneEntity();
    %entity1.template = "AnimatedMeshExample:BigDwarfRedDwarf";
    %entity1.position = "100 0 100";
    %entity1.rotation = "0 0 0";
    Scene::addEntity(%entity1, "Dwarf Meshes");

    %light1 = new SceneEntity();
    %light1.template = "AnimatedMeshExample:LightTest2";
    %light1.position = "100 0 100";
    Scene::addEntity(%light1, "Lights");

    Scene::setDirectionalLight("1 1 -1", "0.8 0.8 0.8", "0.1 0.1 0.1");
}

function AnimatedMeshExample::destroy( %this )
{
    // String to tell us the Console has been called to be destroyed.  
    echo("AnimatedMeshExample::destroy function called");
}
