function AnimatedMeshExample::create(%this)
{     
    %entity1 = new SceneEntity();
    %entity1.template = "AnimatedMeshExample:BigDwarfLittleDwarf";
    %entity1.position = "0 0 0";
    %entity1.rotation = "0 0 0";
    Scene::addEntity(%entity1, "Dwarf Meshes");

    %light1 = new SceneEntity();
    %light1.template = "AnimatedMeshExample:Lights";
    %light1.position = "0 0 0";
    Scene::addEntity(%light1, "Lights");

    Scene::setDirectionalLight("1 1 -1", "0.8 0.8 0.8", "0.2 0.2 0.2");
}

function AnimatedMeshExample::destroy( %this )
{
    // String to tell us the Console has been called to be destroyed.  
    echo("AnimatedMeshExample::destroy function called");
}
