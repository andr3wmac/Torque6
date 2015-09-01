function Sponza::create(%this)
{     
    %dlaa = new DLAA();
    Scene::addFeature(%dlaa);
    %dlaa.activate();

    %ssao = new SSAO();
    Scene::addFeature(%ssao);
    %ssao.activate();

    %hdr = new HDR();
    Scene::addFeature(%hdr);
    %hdr.activate();

    //%cvgi = new CVGI();
    //Scene::addFeature(%cvgi);
    //%cvgi.activate();

    %dirLight = new DirectionalLight();
    Scene::addFeature(%dirLight);
    %dirLight.activate();

    Scene::setDirectionalLight("1 1 -1", "1.0 1.0 1.0", "0.2 0.2 0.2");

    %sponza = new SceneEntity();
    %sponza.template = "Sponza:Sponza";
    %sponza.position = "0 0 0";
    Scene::addEntity(%sponza, "Sponza");

    Skybox::load(expandPath("^Sponza/textures/desertSky.dds"));
    Skybox::enable();
}

function Sponza::destroy( %this )
{
    
}
