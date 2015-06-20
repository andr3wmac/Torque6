function torque6Plugin(_name)
    project (_name)
        location (path.join(BUILD_DIR, "plugins"))
        targetdir (path.join(PLUGIN_DIR, _name) .. "/1/")


        targetname (_name)
        language "C++"
        kind "SharedLib"

        flags {
            "No64BitChecks",
            "ExtraWarnings",
            "StaticRuntime"
        }

        includedirs {
            "../../engine/source",
            "../../engine/lib/assimp/include",
            "../../engine/lib/bgfx/include",
            "../../engine/lib/bgfx/common",
            "../../engine/lib/bgfx/common/imgui",
            "../../engine/lib/bgfx/common/nanovg",
            "../../engine/lib/openal/win32",
        }

        files {
            "../../plugins/" .. _name .. "/**.h",
            "../../plugins/" .. _name .. "/**.cpp",
            "../../plugins/" .. _name .. "/**.cc",
        }

        links {
            "Torque6"
        }
     
        defines {
            "TORQUE_PLUGIN",
            "_USRDLL"
        }

        configuration "Debug"
            defines     { "TORQUE_DEBUG" }
            flags       { "Symbols" }

        configuration "Release"
            defines     { }
            flags       { }

        configuration "vs*"
            defines     { "_CRT_SECURE_NO_WARNINGS" }
            buildoptions    { "/wd4100", "/wd4800" }

        configuration "windows"
            links { "ole32" }
            defines { "WIN32", "_WINDOWS" }

        configuration "linux"
            links       { "dl" }

        configuration "bsd"

        configuration "linux or bsd"
            defines     {  }
            links       { "m" }
            linkoptions { "-rdynamic" }

        configuration "macosx"
            defines     {  }
            links       { "CoreServices.framework" }

        configuration { "macosx", "gmake" }
            buildoptions { "-mmacosx-version-min=10.4" }
            linkoptions  { "-mmacosx-version-min=10.4" }
end
