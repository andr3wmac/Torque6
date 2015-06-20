function AnimatedMeshExample::create(%this)
{     
    Scene::setDirectionalLight("1 1 -1", "1.0 1.0 1.0", "0.2 0.2 0.2");

    Skybox::load(expandPath("^Skybox/textures/pisa_lod.dds"));
    Skybox::enable();
}

function AnimatedMeshExample::destroy( %this )
{
    // String to tell us the Console has been called to be destroyed.  
    echo("AnimatedMeshExample::destroy function called");
}
