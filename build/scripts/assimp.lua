function assimp()
    project "assimp"
        location (BUILD_DIR .. "lib")
        targetdir (BUILD_DIR .. "lib/bin")

        targetname "assimp"
        language "C++"
        kind "StaticLib"

        includedirs {
            SRC_DIR,
            path.join(LIB_DIR, "assimp/code/BoostWorkaround"),
            path.join(LIB_DIR, "assimp/include"),
        }

        files {
            path.join(LIB_DIR, "assimp/code/**.h"),
            path.join(LIB_DIR, "assimp/code/**.cpp"),
            path.join(LIB_DIR, "assimp/contrib/clipper/clipper.hpp"),
            path.join(LIB_DIR, "assimp/contrib/clipper/clipper.cpp"),
            path.join(LIB_DIR, "assimp/contrib/ConvertUTF/ConvertUTF.c"),
            path.join(LIB_DIR, "assimp/contrib/ConvertUTF/ConvertUTF.h"),
            path.join(LIB_DIR, "assimp/contrib/irrXML/**.h"),
            path.join(LIB_DIR, "assimp/contrib/irrXML/**.cpp"),
            path.join(LIB_DIR, "assimp/contrib/poly2tri/poly2tri/common/**.h"),
            path.join(LIB_DIR, "assimp/contrib/poly2tri/poly2tri/common/**.cc"),
            path.join(LIB_DIR, "assimp/contrib/poly2tri/poly2tri/sweep/**.h"),
            path.join(LIB_DIR, "assimp/contrib/poly2tri/poly2tri/sweep/**.cc"),
            path.join(LIB_DIR, "assimp/contrib/poly2tri/poly2tri/poly2tri.h"),
            path.join(LIB_DIR, "assimp/contrib/unzip/**.h"),
            path.join(LIB_DIR, "assimp/contrib/unzip/**.c"),
            path.join(LIB_DIR, "assimp/contrib/zlib/**.h"),
            path.join(LIB_DIR, "assimp/contrib/zlib/**.c"),
            path.join(LIB_DIR, "assimp/include/assimp/**.hpp"),
            path.join(LIB_DIR, "assimp/include/assimp/**.h"),
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

        configuration "vs2015"
            windowstargetplatformversion "10.0.10240.0"

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
end
