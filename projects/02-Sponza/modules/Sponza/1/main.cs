function Sponza::create(%this)
{     
    %sponza = new SceneEntity();
    %sponza.template = "./entities/Sponza.taml";
    %sponza.position = "0 0 0";
    Scene::addEntity(%sponza, "Sponza");

    Scene::setDirectionalLight("1 1 0", "0.4 0.4 0.4", "0 0 0 0.7");
}

function Sponza::destroy( %this )
{
    
}
