function AnimatedMeshExample::create(%this)
{     
    // Load Scene
    Scene::load("^AnimatedMeshExample/scene.taml");
    Scene::setDirectionalLight("1 -1 1", "1.0 1.0 1.0", "0.2 0.2 0.2");
}

function AnimatedMeshExample::destroy( %this )
{
    // String to tell us the Console has been called to be destroyed.  
    echo("AnimatedMeshExample::destroy function called");
}
