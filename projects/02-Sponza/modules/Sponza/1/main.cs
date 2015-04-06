function Sponza::create(%this)
{     
    %sponza = new SceneEntity();
    %sponza.template = "Sponza:Sponza";
    %sponza.position = "0 0 0";
    Scene::addEntity(%sponza, "Sponza");

    Skybox::load(expandPath("^Sponza/textures/desertSky.dds"));
    Skybox::enable();

    Scene::setDirectionalLight("1 1 0", "0.9 0.9 0.9", "0.1 0.1 0.1");
}

function Sponza::destroy( %this )
{
    
}
