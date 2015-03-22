![t6_logo](http://i.imgur.com/xHh9MAY.png)
-------

Torque 6 is an MIT licensed 3D engine loosely based on [Torque2D](https://github.com/GarageGames/Torque2D). Being neither Torque2D or Torque3D it is the 6th derivative of the original Torque Engine, giving it the name Torque 6. It takes the best of Torque2D and replaces the 2D renderer with a deferred shaded 3D renderer with forward rendering for special materials. The engine is still under heavy development and not yet ready for use in a game.

Features
--------

 - Renderer supports DirectX 9, DirectX 11, OpenGL 2.1, OpenGL 3.1+, OpenGLES 2.1 and OpenGLES 3.1+.
 - Multiplatform support for Linux, Mac OS X and Windows.
 - Asset import support major formats such as FBX, Collada, OBJ and more.
 - TorqueScript scripting language
 - Module and asset management system
 - C++ DLL plugin support
 - Single glsl-like language for all shaders

Example Projects
----------------

In Visual Studio open Properties of Torque6Executable and set Debugging -> Working Directory to the directory of the example you want to run and then Start Debugging to run from Visual Studio.

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

Terrain loaded from heightmap. No texturing yet.

![example-04-Terrain](https://raw.githubusercontent.com/andr3wmac/Torque6/master/projects/04-Terrain/screenshot.png)

Third Party Libraries
---------------------

 - [bgfx](https://github.com/bkaradzic/bgfx)
 - [Open Asset Import Library](https://github.com/assimp/assimp)
 
Special Thanks
--------------

 - [Branimir Karadžić](https://github.com/bkaradzic) for support with bgfx.
 - GraphicSynth for the logo.
