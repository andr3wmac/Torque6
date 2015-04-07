function TerrainExample::create(%this)
{     
    Scene::setDirectionalLight("1 1 -1", "0.8 0.8 0.8", "0.1 0.1 0.1");

    Terrain::loadEmptyTerrain(0, 0, 256, 256);
    Terrain::loadEmptyTerrain(0, 1, 256, 256);
    Terrain::loadEmptyTerrain(1, 0, 256, 256);
    Terrain::loadEmptyTerrain(1, 1, 256, 256);

    // Terrain Cell 0,0 
    //Terrain::loadHeightMap(0, 0, expandPath("^Terrain/heightmaps/test.png"));
    //Terrain::loadTexture(0, 0, 0, expandPath("^Terrain/textures/grass.jpg"));
    //Terrain::loadTexture(0, 0, 1, expandPath("^Terrain/textures/rocks.jpg"));
    //Terrain::loadTexture(0, 0, 2, expandPath("^Terrain/textures/snow.png"));

    // Terrain Cell 1,0
    //Terrain::loadHeightMap(1, 0, expandPath("^Terrain/heightmaps/test.png"));
    //Terrain::loadTexture(1, 0, 0, expandPath("^Terrain/textures/grass.jpg"));
    //Terrain::loadTexture(1, 0, 1, expandPath("^Terrain/textures/rocks.jpg"));
    //Terrain::loadTexture(1, 0, 2, expandPath("^Terrain/textures/snow.png"));

    // Terrain Cell 0,1
    //Terrain::loadHeightMap(0, 1, expandPath("^Terrain/heightmaps/test.png"));
    //Terrain::loadTexture(0, 1, 0, expandPath("^Terrain/textures/grass.jpg"));
    //Terrain::loadTexture(0, 1, 1, expandPath("^Terrain/textures/rocks.jpg"));
    //Terrain::loadTexture(0, 1, 2, expandPath("^Terrain/textures/snow.png"));

    // Terrain Cell 1,1
    //Terrain::loadHeightMap(1, 1, expandPath("^Terrain/heightmaps/test.png"));
    //Terrain::loadTexture(1, 1, 0, expandPath("^Terrain/textures/grass.jpg"));
    //Terrain::loadTexture(1, 1, 1, expandPath("^Terrain/textures/rocks.jpg"));
    //Terrain::loadTexture(1, 1, 2, expandPath("^Terrain/textures/snow.png"));

    Terrain::stitchEdges();
    Terrain::enable();

    Skybox::load(expandPath("^TerrainExample/textures/desertSky.dds"));
    Skybox::enable();
}

function TerrainExample::destroy( %this )
{

}
