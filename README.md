![t6_logo](http://i.imgur.com/xHh9MAY.png)
-------

Torque 6 is an MIT licensed 3D engine loosely based on [Torque2D](https://github.com/GarageGames/Torque2D). Being neither Torque2D or Torque3D it is the 6th derivative of the original Torque Engine, giving it the name Torque 6. It takes the best of Torque2D and replaces the 2D renderer with a deferred shaded 3D renderer with forward rendering for special materials. The engine is still under heavy development and not yet ready for use in a game.

Links
--------

 - [Twitter](https://twitter.com/torque6engine)
 - [YouTube](https://www.youtube.com/channel/UCD--TmjTZU9FstD5yg4yKDg)
 - [Forums](http://forums.torque3d.org/viewforum.php?f=32)

Features
--------

 - Renderer supports DirectX 9, DirectX 11, OpenGL 2.1, OpenGL 3.1+, OpenGLES 2.1 and OpenGLES 3.1+.
 - Multiplatform support for Linux, Mac OS X and Windows.
 - Asset import support major formats such as FBX, Collada, OBJ and more.
 - TorqueScript scripting language
 - Module and asset management system
 - C++ DLL plugin support
 - Single glsl-like language for all shaders

Building
--------

Torque 6 uses [GENie](https://github.com/bkaradzic/genie) to generate projects to build the engine. The binaries and scripts are found in the build directory. Windows users can use build/GENERATE_VS2010/12/13.BAT to generate Visual Studio projects. 

Example Projects
----------------

In Visual Studio open Properties of Torque6App and set Debugging -> Working Directory to the directory of the example you want to run and then Start Debugging.

### [00-Console](https://github.com/andr3wmac/Torque6/tree/master/projects/00-Console)

Simple console example.

![example-00-Console](https://raw.githubusercontent.com/andr3wmac/Torque6/master/projects/00-Console/screenshot.png)

### [01-AnimatedMesh](https://github.com/andr3wmac/Torque6/tree/master/projects/01-AnimatedMesh)

A floor, 2 animated dwarves and a set of lights.

![example-01-AnimatedMesh](https://raw.githubusercontent.com/andr3wmac/Torque6/master/projects/01-AnimatedMesh/screenshot.png)

### [02-Sponza](https://github.com/andr3wmac/Torque6/tree/master/projects/02-Sponza)

Crytek Sponza with directional light.

![example-02-Sponza](https://raw.githubusercontent.com/andr3wmac/Torque6/master/projects/02-Sponza/screenshot.png)

### [03-Collision](https://github.com/andr3wmac/Torque6/tree/master/projects/03-Collision)

Grey cubes light up green when red cube collides with them.

![example-03-Collision](https://raw.githubusercontent.com/andr3wmac/Torque6/master/projects/03-Collision/screenshot.png)

### [04-Terrain](https://github.com/andr3wmac/Torque6/tree/master/projects/04-Terrain)

Terrain loaded from heightmap. Textured with a technique similar to [Leadwerk's Virtual Texture Terrain](http://www.leadwerks.com/werkspace/blog/41/entry-1112-virtual-texture-terrain/).

![example-04-Terrain](https://raw.githubusercontent.com/andr3wmac/Torque6/master/projects/04-Terrain/screenshot.png)

### [05-Particles](https://github.com/andr3wmac/Torque6/tree/master/projects/05-Particles)

Simple particle emitter demo.

![example-05-Particles](https://raw.githubusercontent.com/andr3wmac/Torque6/master/projects/05-Particles/screenshot.png)

### [06-Materials](https://github.com/andr3wmac/Torque6/tree/master/projects/06-Materials)

Material demo showing varying metallic/roughness values.

![example-06-Materials](https://raw.githubusercontent.com/andr3wmac/Torque6/master/projects/06-Materials/screenshot.png)

Third Party Libraries
---------------------

 - [bgfx](https://github.com/bkaradzic/bgfx)
 - [Open Asset Import Library](https://github.com/assimp/assimp)
 
Special Thanks
--------------

 - [Branimir Karadžić](https://github.com/bkaradzic) for support with bgfx.
 - [Dario Manesku](https://github.com/dariomanesku) for his bgfx examples and shaders.
 - GraphicSynth for the logo.
