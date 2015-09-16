function TerrainExample::create(%this)
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

    %dirLight = new DirectionalLight();
    Scene::addFeature(%dirLight);
    %dirLight.activate();
    Scene::setDirectionalLight("1 1 -1", "1.0 1.0 1.0", "0.2 0.2 0.2");

    Terrain::loadTexture(0, expandPath("^Terrain/textures/grass.png"));
    Terrain::loadTexture(1, expandPath("^Terrain/textures/snow.png"));
    Terrain::loadHeightMap(0, 0, expandPath("^Terrain/heightmaps/test2.png"));
    Terrain::enable();

    %skybox = new SimpleSkybox();
    Scene::addFeature(%skybox);
    %skybox.loadTexture(expandPath("^Skybox/textures/desertSky.dds"));
    %skybox.activate();
}

function TerrainExample::destroy( %this )
{

}
