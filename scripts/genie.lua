solution "Torque6"
	configurations {
		"Release",
		"Debug"
	}
	location ("./out")

    -- Torque6 DLL
    dofile ("Torque6.lua")

    -- Torque6 Executable
    dofile ("Torque6Executable.lua")
    
    -- Libraries  
    group "Libraries"
    dofile ("assimp.lua")
    dofile ("bgfx.lua")
    dofile ("ljpeg.lua")
    dofile ("lpng.lua")
    dofile ("zlib.lua")

    -- Plugins
    group "Plugins"
    dofile ("Plugins.lua")
    torque6Plugin("Skybox")
    torque6Plugin("Editor")
    torque6Plugin("Terrain")
