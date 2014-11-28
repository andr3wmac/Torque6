function AnimatedMeshExample::create(%this)
{     
    %entity1 = new SceneEntity();
    %entity1.template = "./entities/bigDwarfRedDwarf.taml";
    %entity1.position = "0 0 0";
    %entity1.rotation = "0 3.14 0";
    Scene::addEntity(%entity1);

    %entity2 = new SceneEntity();
    %entity2.template = "./entities/bigDwarfRedDwarf.taml";
    %entity2.position = "0 0 -80";
    Scene::addEntity(%entity2);

    %cam = Scene::getCamera();
    %cam.bindMouse(true);
    %cam.setPosition("0 50 -150");
}

function AnimatedMeshExample::destroy( %this )
{
    // String to tell us the Console has been called to be destroyed.  
    echo("AnimatedMeshExample::destroy function called");
}
