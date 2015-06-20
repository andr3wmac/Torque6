solution "Torque6"
    -- Settings
    BUILD_DIR = "../" .. _ACTION .. "/"
    PLUGIN_DIR = "../../projects/shared-modules/"
    PROJECT = "../../projects/00-Console/"

    -- Solution
    startproject "Torque6App"
    configurations {
        "Release",
        "Debug"
    }
    platforms {
			"x32",
			"x64",
    }
    location (BUILD_DIR)

    -- Torque6 App
    dofile ("Torque6App.lua")

    -- Torque6 DLL
    dofile ("Torque6.lua")
    
    -- Libraries  
    group "Libraries"
    dofile ("assimp.lua")
    dofile ("bgfx.lua")
    dofile ("bullet.lua")
    dofile ("ljpeg.lua")
    dofile ("lpng.lua")
    dofile ("zlib.lua")

    -- Plugins
    group "Plugins"
    dofile ("Plugins.lua")

    -- This loads plugins by folder name in the plugins directory.
    -- The name must match a module in the PLUGIN_DIR
    torque6Plugin("Skybox")
    torque6Plugin("Editor")
    torque6Plugin("Terrain")
    torque6Plugin("Particles")
