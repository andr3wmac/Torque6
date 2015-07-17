    project "ljpeg"
        location (BUILD_DIR .. "lib")
        targetdir (BUILD_DIR .. "lib/bin")

        targetname "ljpeg"
        language "C++"
        kind "StaticLib"

        includedirs {
            SRC_DIR,
        }

        files {
            path.join(LIB_DIR, "ljpeg/**.h"),
            path.join(LIB_DIR, "ljpeg/**.c"),
        }

        removefiles {
            path.join(LIB_DIR, "ljpeg/extras/**"),
            path.join(LIB_DIR, "ljpeg/**.mac.h"),
            path.join(LIB_DIR, "ljpeg/**.linux.h"),
            path.join(LIB_DIR, "ljpeg/jmemansi.c"),
            path.join(LIB_DIR, "ljpeg/jmemdos.c"),
            path.join(LIB_DIR, "ljpeg/jmemmac.c"),
            path.join(LIB_DIR, "ljpeg/jmemname.c"),
            path.join(LIB_DIR, "ljpeg/jpegtran.c"),
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
