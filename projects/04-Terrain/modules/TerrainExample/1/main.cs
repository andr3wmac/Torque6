function TerrainExample::create(%this)
{     
    Scene::setDirectionalLight("1 1 -1", "0.8 0.8 0.8", "0.1 0.1 0.1");

    Terrain::load(expandPath("^Terrain/heightmaps/test.png"));
    Terrain::loadLayer(0, expandPath("^Terrain/textures/grass.jpg"));
    Terrain::loadLayer(1, expandPath("^Terrain/textures/rocks.jpg"));
    Terrain::loadLayer(2, expandPath("^Terrain/textures/snow.png"));
    Terrain::enable();

    Skybox::load(expandPath("^TerrainExample/textures/desertSky.dds"));
    Skybox::enable();

}

function TerrainExample::destroy( %this )
{

}
