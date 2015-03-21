function TerrainExample::create(%this)
{     
    Scene::setDirectionalLight("1 1 -1", "0.8 0.8 0.8", "0.1 0.1 0.1");

    Terrain::load(expandPath("^Terrain/heightmaps/test.png"));
    Terrain::enable();

    Skybox::load(expandPath("^TerrainExample/textures/desertSky.dds"));
    Skybox::enable();

}

function TerrainExample::destroy( %this )
{

}
