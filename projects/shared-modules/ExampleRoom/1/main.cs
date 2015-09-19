function ExampleRoom::create(%this)
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
    %skybox.loadTexture(expandPath("^Skybox/textures/pisa_lod.dds"));
    %skybox.activate();

    %dirLight = new DirectionalLight();
    Scene::addFeature(%dirLight);
    %dirLight.activate();

    Scene::setDirectionalLight("1 1 -1", "1.0 1.0 1.0", "0.2 0.2 0.2");

    %example_room = new SceneEntity();
    %example_room.template = "ExampleRoom:ExampleRoom";
    %example_room.position = "0 0 0";
    %example_room.rotation = "1.57 0 0";
    %example_room.scale = "1 1 1";
    Scene::addEntity(%example_room, "Floor");
}

function ExampleRoom::destroy( %this )
{
    
}
