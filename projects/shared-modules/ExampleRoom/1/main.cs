function ExampleRoom::create(%this)
{     
    %example_room = new SceneEntity();
    %example_room.template = "ExampleRoom:ExampleRoom";
    %example_room.position = "0 -100 0";
    Scene::addEntity(%example_room, "Floor");
}

function ExampleRoom::destroy( %this )
{
    
}
