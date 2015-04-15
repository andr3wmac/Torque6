    project "assimp"
        location (BUILD_DIR .. "lib")
        targetdir (BUILD_DIR .. "lib/bin")

        targetname "assimp"
        language "C++"
        kind "StaticLib"

        includedirs {
            "../../engine/source",
            "../../engine/lib/assimp/code/BoostWorkaround",
            "../../engine/lib/assimp/include",
        }

        files {
            "../../engine/lib/assimp/code/**.h",
            "../../engine/lib/assimp/code/**.cpp",
            "../../engine/lib/assimp/contrib/clipper/clipper.hpp",
            "../../engine/lib/assimp/contrib/clipper/clipper.cpp",
            "../../engine/lib/assimp/contrib/ConvertUTF/ConvertUTF.c",
            "../../engine/lib/assimp/contrib/ConvertUTF/ConvertUTF.h",
            "../../engine/lib/assimp/contrib/irrXML/**.h",
            "../../engine/lib/assimp/contrib/irrXML/**.cpp",
            "../../engine/lib/assimp/contrib/poly2tri/poly2tri/common/**.h",
            "../../engine/lib/assimp/contrib/poly2tri/poly2tri/common/**.cc",
            "../../engine/lib/assimp/contrib/poly2tri/poly2tri/sweep/**.h",
            "../../engine/lib/assimp/contrib/poly2tri/poly2tri/sweep/**.cc",
            "../../engine/lib/assimp/contrib/poly2tri/poly2tri/poly2tri.h",
            "../../engine/lib/assimp/contrib/unzip/**.h",
            "../../engine/lib/assimp/contrib/unzip/**.c",
            "../../engine/lib/assimp/contrib/zlib/**.h",
            "../../engine/lib/assimp/contrib/zlib/**.c",
            "../../engine/lib/assimp/include/assimp/**.hpp",
            "../../engine/lib/assimp/include/assimp/**.h",
        }

        removefiles {
            
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
            linkoptions { "-rdynamic " }
            buildoptions { "-fPIC" }

        configuration "macosx"
            defines     {  }
            links       { "CoreServices.framework" }

        configuration { "macosx", "gmake" }
            buildoptions { "-mmacosx-version-min=10.4" }
            linkoptions  { "-mmacosx-version-min=10.4" }
