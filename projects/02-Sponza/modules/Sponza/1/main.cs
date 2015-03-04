function Sponza::create(%this)
{     
    %sponza = new SceneEntity();
    %sponza.template = "./entities/Sponza.taml";
    %sponza.position = "0 0 0";
    Scene::addEntity(%sponza, "Sponza");
}

function Sponza::destroy( %this )
{
    
}
