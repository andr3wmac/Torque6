function MaterialExample::create(%this)
{     
    Scene::load("^MaterialExample/scene.taml");
    Scene::setDirectionalLight("1 -1 1", "1.0 1.0 1.0", "0.2 0.2 0.2");

    %spheres = new SceneEntity();
    %spheres.template = "MaterialExample:TestSpheres";
    %spheres.position = "0 0 0";
    %spheres.rotation = "0 0 0";
    Scene::addObject(%spheres, "Spheres");

    %metal = 0.0;
    %rough = 0.0;
    for(%n = 0; %n < 25; %n++)
    {
        %spheres.findComponent("TestSphere" @ %n).setUniformVec4("sphereMetalVal", %metal @ " 0 0 0");
        %spheres.findComponent("TestSphere" @ %n).setUniformVec4("sphereRoughVal", %rough @ " 0 0 0");
  
        %random_color = getRandom() SPC getRandom() SPC getRandom() SPC "1.0";
        %spheres.findComponent("TestSphere" @ %n).setUniformVec4("sphereColorVal", %random_color);

        %metal += 0.25;
        if ( %metal > 1.0 )
        {
            %metal = 0.0;
            %rough += 0.25;
        }
    }
}

function MaterialExample::destroy( %this )
{
    // String to tell us the Console has been called to be destroyed.  
    echo("MaterialExample::destroy function called");
}
