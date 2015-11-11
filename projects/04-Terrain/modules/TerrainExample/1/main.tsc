function TerrainExample::create(%this)
{     
    Scene::load("^TerrainExample/scene.taml");
    Scene::setDirectionalLight("1 -1 1", "1.0 1.0 1.0", "0.2 0.2 0.2");

    Terrain::loadTexture(0, expandPath("^Terrain/textures/grass.png"));
    Terrain::loadTexture(1, expandPath("^Terrain/textures/snow.png"));
    Terrain::loadHeightMap(0, 0, expandPath("^Terrain/heightmaps/test2.png"));
    Terrain::enable();
}

function TerrainExample::destroy( %this )
{

}
