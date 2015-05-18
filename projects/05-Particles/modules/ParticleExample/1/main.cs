function ParticleExample::create(%this)
{     
    Scene::setDirectionalLight("1 1 -1", "0.8 0.8 0.8", "0.1 0.1 0.1");

    //Particles::enable();

    Skybox::load(expandPath("^Skybox/textures/desertSky.dds"));
    Skybox::enable();

    %example_particles = new SceneEntity();
    %example_particles.template = "ParticleExample:ParticleEntity";
    %example_particles.position = "0 0 0";
    Scene::addEntity(%example_particles, "Particles");  
}

function ParticleExample::destroy( %this )
{

}
