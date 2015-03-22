solution "Torque6"
	configurations {
		"Release",
		"Debug"
	}
	location (_OPTIONS["to"])

    -- Torque6 DLL
    dofile ("Torque6.lua")

    -- Torque6 Executable
    dofile ("Torque6Executable.lua")
    
    -- Libraries  
    group "Libraries"
    dofile ("Libraries.lua")
    torque6Library("ljpeg")
    torque6Library("lpng")
    torque6Library("zlib")

    -- Plugins
    group "Plugins"
    dofile ("Plugins.lua")
    torque6Plugin("Skybox")
    torque6Plugin("Editor")
    torque6Plugin("Terrain")
