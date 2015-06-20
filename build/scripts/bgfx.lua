    project "bgfx"
        location (BUILD_DIR .. "lib")
        targetdir (BUILD_DIR .. "lib/bin")

        targetname "bgfx"
        language "C++"
        kind "StaticLib"

        includedirs {
            "../../engine/lib/bgfx/include",
            "../../engine/lib/bgfx/3rdparty",
            "../../engine/lib/bgfx/3rdparty/khronos",
            "../../engine/lib/bgfx/3rdparty/fcpp",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/include",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/mapi",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/mesa",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/getopt",
            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src",

            "../../engine/lib/bgfx/3rdparty/scintilla/include",
			"../../engine/lib/bgfx/3rdparty/scintilla/lexlib",
        }

        files {
            "../../engine/lib/bgfx/src/**.h",
            "../../engine/lib/bgfx/src/**.c",
            "../../engine/lib/bgfx/src/**.cpp",
            "../../engine/lib/bgfx/3rdparty/ocornut-imgui/**.cpp",
            "../../engine/lib/bgfx/3rdparty/ocornut-imgui/**.h",
            "../../engine/lib/bgfx/common/cube_atlas.h",
            "../../engine/lib/bgfx/common/cube_atlas.cpp",
            "../../engine/lib/bgfx/common/font/**.h",
            "../../engine/lib/bgfx/common/font/**.cpp",
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
            "../../engine/lib/bgfx/tools/shaderc/**.cpp",
            "../../engine/lib/bgfx/tools/shaderc/**.h",

            "../../engine/lib/bgfx/3rdparty/fcpp/**.h",
		    "../../engine/lib/bgfx/3rdparty/fcpp/cpp1.c",
		    "../../engine/lib/bgfx/3rdparty/fcpp/cpp2.c",
		    "../../engine/lib/bgfx/3rdparty/fcpp/cpp3.c",
		    "../../engine/lib/bgfx/3rdparty/fcpp/cpp4.c",
		    "../../engine/lib/bgfx/3rdparty/fcpp/cpp5.c",
		    "../../engine/lib/bgfx/3rdparty/fcpp/cpp6.c",
		    "../../engine/lib/bgfx/3rdparty/fcpp/cpp6.c",

		    "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/mesa/**.c",
		    "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/**.cpp",
		    "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/mesa/**.h",
		    "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/**.c",
		    "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/**.cpp",
		    "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/**.h",
		    "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/util/**.c",
		    "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/util/**.h",

			"../../engine/lib/bgfx/3rdparty/scintilla/src/**.cxx",
			"../../engine/lib/bgfx/3rdparty/scintilla/src/**.h",
			"../../engine/lib/bgfx/3rdparty/scintilla/lexlib/**.cxx",
			"../../engine/lib/bgfx/3rdparty/scintilla/lexlib/**.h",
			"../../engine/lib/bgfx/3rdparty/scintilla/lexers/**.cxx",
        }

        removefiles {
            "../../engine/lib/bgfx/src/amalgamated.cpp",
            "../../engine/lib/bgfx/src/**.bin.h",
            "../../engine/lib/bgfx/common/**.ttf.h",
            "../../engine/lib/bgfx/common/**.bin.h",

            "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/glcpp/glcpp.c",
		    "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/glcpp/tests/**",
		    "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/glcpp/**.l",
		    "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/glcpp/**.y",
		    "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/ir_set_program_inouts.cpp",
		    "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/main.cpp",
		    "../../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/builtin_stubs.cpp",
        }

		defines {
			"DISABLED_SCI_NAMESPACE",
			"DISABLED_SCI_LEXER",
		}

		buildoptions {
--			"-Wno-missing-field-initializers",
		}

        configuration "Debug"
            defines     { "TORQUE_DEBUG", "BGFX_CONFIG_DEBUG=1" }
            flags       { "Symbols" }

        configuration "Release"
            defines     {  }

        configuration "vs*"
            includedirs { "../../engine/lib/bgfx/include/compat/msvc" }
            defines     { "_CRT_SECURE_NO_WARNINGS", "__STDC_FORMAT_MACROS" }
            buildoptions {
                "/wd4996",
                "/wd4291"
            }

        configuration "windows"
            links { "ole32" }

        configuration { "mingw* or linux or osx" }
		    buildoptions {
			    "-fno-strict-aliasing", -- glsl-optimizer has bugs if strict aliasing is used.
			    "-Wno-unused-parameter",
                "-std=c++0x",
		    }
		    removebuildoptions {
			    "-Wshadow", -- glsl-optimizer is full of -Wshadow warnings ignore it.
		    }

        configuration "bsd"
            targetdir   "../bin/bsd"

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
