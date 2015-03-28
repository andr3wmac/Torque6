function ParticleExample::create(%this)
{     
    Scene::setDirectionalLight("1 1 -1", "0.8 0.8 0.8", "0.1 0.1 0.1");

    Particles::enable();

    Skybox::load(expandPath("^ParticleExample/textures/desertSky.dds"));
    Skybox::enable();
}

function ParticleExample::destroy( %this )
{

}
