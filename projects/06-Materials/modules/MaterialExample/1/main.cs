function MaterialExample::create(%this)
{     
    %spheres = new SceneEntity();
    %spheres.template = "MaterialExample:TestSpheres";
    %spheres.position = "0 0 0";
    %spheres.rotation = "0 0 0";
    Scene::addEntity(%spheres, "Spheres");

    %metal = 0.0;
    %rough = 0.0;
    for(%n = 0; %n < 25; %n++)
    {
        %spheres.findComponent("TestSphere" @ %n).setUniformFloat("sphereMetalVal", %metal);
        %spheres.findComponent("TestSphere" @ %n).setUniformFloat("sphereRoughVal", %rough);
  
        %metal += 0.25;
        if ( %metal > 1.0 )
        {
            %metal = 0.0;
            %rough += 0.25;
        }
    }

    Scene::setDirectionalLight("1 1 -1", "1.0 1.0 1.0", "0.2 0.2 0.2");

    Skybox::load(expandPath("^Skybox/textures/pisa_lod.dds"));
    Skybox::enable();
}

function MaterialExample::destroy( %this )
{
    // String to tell us the Console has been called to be destroyed.  
    echo("MaterialExample::destroy function called");
}
