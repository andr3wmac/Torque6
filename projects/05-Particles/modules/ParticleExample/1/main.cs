function ParticleExample::create(%this)
{     
    Scene::load("^ParticleExample/scene.taml");
    Scene::setDirectionalLight("1 -1 1", "1.0 1.0 1.0", "0.2 0.2 0.2");

    %example_particles = new SceneEntity();
    %example_particles.template = "ParticleExample:ParticleEntity";
    %example_particles.position = "0 0 0";
    Scene::addEntity(%example_particles, "Particles");  
}

function ParticleExample::destroy( %this )
{

}
