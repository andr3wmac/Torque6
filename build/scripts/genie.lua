solution "Torque6"
    -- Settings
    BUILD_DIR           = "../bin"
    PROJECT_DIR         = "../" .. _ACTION .. "/"
    LIB_DIR             = "../../lib/"
    LIB_PROJECT_DIR     = PROJECT_DIR .. "lib"
    LIB_BUILD_DIR       = PROJECT_DIR .. "lib/bin"
    SRC_DIR             = "../../src/"
    PLUGIN_DIR          = "../../plugins/"
    PLUGIN_BUILD_DIR    = "../../projects/shared-modules/"
    PROJECT             = "../../projects/00-Console/"

    -- Solution
    configurations {
        "Release",
        "Debug"
    }
    platforms {
        "x32",
        "x64",
    }
    location (PROJECT_DIR)

    -- Torque6 App
    dofile ("Torque6App.lua")
    Torque6App()
    startproject "Torque6App"

    -- Torque6 DLL
    dofile ("Torque6.lua")
    Torque6()
    
    -- Libraries  
    group "Libraries"
    dofile ("assimp.lua")
    assimp()
    dofile ("bgfx.lua")
    bgfx()
    dofile ("bullet.lua")
    bullet()
    dofile ("ljpeg.lua")
    ljpeg()
    dofile ("lpng.lua")
    lpng()
    dofile ("zlib.lua")
    zlib()

    -- Plugins
    group "Plugins"
    dofile ("Plugins.lua")

    -- This loads plugins by folder name in the plugins directory.
    -- The name must match a module in the PLUGIN_DIR
    torque6Plugin("Sky")
    torque6Plugin("Terrain")
    torque6Plugin("Particles")
    torque6Plugin("CSG")
