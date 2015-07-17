    project "bullet"
        location (BUILD_DIR .. "lib")
        targetdir (BUILD_DIR .. "lib/bin")

        targetname "bullet"
        language "C++"
        kind "StaticLib"

        includedirs {
            path.join(LIB_DIR, "bullet/"),
        }

        files {
            path.join(LIB_DIR, "bullet/**.h"),
            path.join(LIB_DIR, "bullet/**.cpp"),
        }

        configuration "Debug"
            defines     { "TORQUE_DEBUG" }
            flags       { "Symbols" }

        configuration "Release"
            defines     {  }

        configuration "vs*"
            defines     { "_CRT_SECURE_NO_WARNINGS" }

        configuration "windows"
            links { "ole32" }

        configuration "linux"
            links       { "dl" }

        configuration "bsd"

        configuration "linux or bsd"
            defines     {  }
            links       { "m" }
            linkoptions { "-rdynamic" }
            buildoptions { "-fPIC" }

        configuration "macosx"
            links       { "CoreServices.framework" }

        configuration { "macosx", "gmake" }
            buildoptions { "-mmacosx-version-min=10.4" }
            linkoptions  { "-mmacosx-version-min=10.4" }
