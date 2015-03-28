    project "bgfx"
        location (path.join(BUILD_DIR, "lib"))
        targetdir (path.join(BUILD_DIR, "lib"))

        targetname "bgfx"
        language "C++"
        kind "StaticLib"

        includedirs {
            "../../engine/lib/bgfx/include",
            "../../engine/lib/bgfx/include/compat/msvc",
            "../../engine/lib/bgfx/3rdparty",
            "../../engine/lib/bgfx/3rdparty/khronos",
            "../../engine/lib/bgfx/3rdparty/fcpp",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/include",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/mapi",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/mesa",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/getopt",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src",
            "$(DXSDK_DIR)/Include",
        }

        files {
            "../../engine/lib/bgfx/src/**.h",
            "../../engine/lib/bgfx/src/**.c",
            "../../engine/lib/bgfx/src/**.cpp",
            "../../engine/lib/bgfx/3rdparty/fcpp/**.c",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/glcpp/**.c",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/**.h",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/**.cpp",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/**.c",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/mesa/main/imports.c",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/mesa/program/prog_hash_table.c",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/mesa/program/symbol_table.c",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/util/hash_table.c",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/util/ralloc.c",
            "../../engine/lib/bgfx/3rdparty/ocornut-imgui/**.cpp",
            "../../engine/lib/bgfx/3rdparty/ocornut-imgui/**.h",
            "../../engine/lib/bgfx/common/imgui/**.cpp",
            "../../engine/lib/bgfx/common/imgui/**.h",
            "../../engine/lib/bgfx/common/imgui/**.cpp",
            "../../engine/lib/bgfx/common/nanovg/**.h",
            "../../engine/lib/bgfx/common/nanovg/**.cpp",
            "../../engine/lib/bgfx/include/bgfx.c99.h",
            "../../engine/lib/bgfx/include/bgfx.h",
            "../../engine/lib/bgfx/include/bgfxdefines.h",
            "../../engine/lib/bgfx/include/bgfxplatform.c99.h",
            "../../engine/lib/bgfx/include/bgfxplatform.h",
            "../../engine/lib/bgfx/tools/shaderc/**.h",
            "../../engine/lib/bgfx/tools/shaderc/**.cpp",
        }

        removefiles {
            "../../engine/lib/bgfx/src/**.bin.h",
            "../../engine/lib/bgfx/common/**.ttf.h",
            "../../engine/lib/bgfx/common/**.bin.h",
            "../../engine/lib/bgfx/3rdparty/fcpp/usecpp.c",
        }

        configuration "Debug"
            defines     { "TORQUE_DEBUG" }
            flags       { "Symbols" }

        configuration "Release"
            defines     {  }

        configuration "vs*"
            defines     { "_CRT_SECURE_NO_WARNINGS" }
            buildoptions {
                "/wd4996",
                "/wd4291"
            }

        configuration "windows"
            links { "ole32" }

        configuration "linux"
            links       { "dl" }

        configuration "bsd"
            targetdir   "../bin/bsd"

        configuration "linux or bsd"
            defines     {  }
            links       { "m" }
            linkoptions { "-rdynamic" }

        configuration "macosx"
            links       { "CoreServices.framework" }

        configuration { "macosx", "gmake" }
            buildoptions { "-mmacosx-version-min=10.4" }
            linkoptions  { "-mmacosx-version-min=10.4" }
