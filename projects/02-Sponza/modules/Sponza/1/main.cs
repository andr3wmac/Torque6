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

    %skybox = new SimpleSkybox();
    Scene::addFeature(%skybox);
    %skybox.loadTexture(expandPath("^Skybox/textures/desertSky.dds"));
    %skybox.activate();

    %dirLight = new DirectionalLight();
    Scene::addFeature(%dirLight);
    %dirLight.activate();

    Scene::setDirectionalLight("1 1 0", "1.0 1.0 1.0", "0.2 0.2 0.2");

    %sponza = new SceneEntity();
    %sponza.template = "Sponza:Sponza";
    %sponza.position = "0 0 0";
    %sponza.scale = "0.1 0.1 0.1";
    Scene::addEntity(%sponza, "Sponza");
}

function Sponza::destroy( %this )
{
    
}
