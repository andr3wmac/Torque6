function TerrainExample::create(%this)
{     
    Scene::setDirectionalLight("1 1 1", "0.8 0.8 0.8", "0.1 0.1 0.1");

    Terrain::loadTexture(0, expandPath("^Terrain/textures/grass.png"));
    Terrain::loadTexture(1, expandPath("^Terrain/textures/snow.png"));

    Terrain::loadHeightMap(0, 0, expandPath("^Terrain/heightmaps/test2.png"));
    Terrain::enable();

    Skybox::load(expandPath("^Skybox/textures/desertSky.dds"));
    Skybox::enable();
}

function TerrainExample::destroy( %this )
{

}
