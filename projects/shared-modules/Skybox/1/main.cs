function Skybox::create(%this)
{   
    Plugins::load("./SkyboxDLL.dll");   
    Skybox::enable();
}

function Skybox::destroy( %this )
{
    //
}