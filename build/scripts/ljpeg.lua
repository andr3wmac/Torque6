    project "ljpeg"
        location (BUILD_DIR .. "lib")
        targetdir (BUILD_DIR .. "lib/bin")

        targetname "ljpeg"
        language "C++"
        kind "StaticLib"

        includedirs {
            "../../engine/source"
        }

        files {
            "../../engine/lib/ljpeg/**.h",
            "../../engine/lib/ljpeg/**.c",
        }

        removefiles {
            "../../engine/lib/ljpeg/extras/**",
            "../../engine/lib/ljpeg/**.mac.h",
            "../../engine/lib/ljpeg/**.linux.h",
            "../../engine/lib/ljpeg/jmemansi.c",
            "../../engine/lib/ljpeg/jmemdos.c",
            "../../engine/lib/ljpeg/jmemmac.c",
            "../../engine/lib/ljpeg/jmemname.c",
            "../../engine/lib/ljpeg/jpegtran.c",
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
