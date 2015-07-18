function bgfx()
    project "bgfx"
        location (BUILD_DIR .. "lib")
        targetdir (BUILD_DIR .. "lib/bin")

        targetname "bgfx"
        language "C++"
        kind "StaticLib"

        includedirs {
            path.join(LIB_DIR, "bgfx/include"),
            path.join(LIB_DIR, "bgfx/3rdparty"),
            path.join(LIB_DIR, "bgfx/3rdparty/khronos"),
            path.join(LIB_DIR, "bgfx/3rdparty/fcpp"),
            path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/include"),
            path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/glsl"),
            path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/mapi"),
            path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/mesa"),
            path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/getopt"),
            path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src"),

            path.join(LIB_DIR, "bgfx/3rdparty/scintilla/include"),
			path.join(LIB_DIR, "bgfx/3rdparty/scintilla/lexlib"),
        }

        files {
            path.join(LIB_DIR, "bgfx/src/**.h"),
            path.join(LIB_DIR, "bgfx/src/**.c"),
            path.join(LIB_DIR, "bgfx/src/**.cpp"),
            path.join(LIB_DIR, "bgfx/3rdparty/ocornut-imgui/**.cpp"),
            path.join(LIB_DIR, "bgfx/3rdparty/ocornut-imgui/**.h"),
            path.join(LIB_DIR, "bgfx/common/cube_atlas.h"),
            path.join(LIB_DIR, "bgfx/common/cube_atlas.cpp"),
            path.join(LIB_DIR, "bgfx/common/font/**.h"),
            path.join(LIB_DIR, "bgfx/common/font/**.cpp"),
            path.join(LIB_DIR, "bgfx/common/imgui/**.cpp"),
            path.join(LIB_DIR, "bgfx/common/imgui/**.h"),
            path.join(LIB_DIR, "bgfx/common/imgui/**.cpp"),
            path.join(LIB_DIR, "bgfx/common/nanovg/**.h"),
            path.join(LIB_DIR, "bgfx/common/nanovg/**.cpp"),
            path.join(LIB_DIR, "bgfx/include/bgfx.c99.h"),
            path.join(LIB_DIR, "bgfx/include/bgfx.h"),
            path.join(LIB_DIR, "bgfx/include/bgfxdefines.h"),
            path.join(LIB_DIR, "bgfx/include/bgfxplatform.c99.h"),
            path.join(LIB_DIR, "bgfx/include/bgfxplatform.h"),
            path.join(LIB_DIR, "bgfx/tools/shaderc/**.cpp"),
            path.join(LIB_DIR, "bgfx/tools/shaderc/**.h"),

            path.join(LIB_DIR, "bgfx/3rdparty/fcpp/**.h"),
		    path.join(LIB_DIR, "bgfx/3rdparty/fcpp/cpp1.c"),
		    path.join(LIB_DIR, "bgfx/3rdparty/fcpp/cpp2.c"),
		    path.join(LIB_DIR, "bgfx/3rdparty/fcpp/cpp3.c"),
		    path.join(LIB_DIR, "bgfx/3rdparty/fcpp/cpp4.c"),
		    path.join(LIB_DIR, "bgfx/3rdparty/fcpp/cpp5.c"),
		    path.join(LIB_DIR, "bgfx/3rdparty/fcpp/cpp6.c"),
		    path.join(LIB_DIR, "bgfx/3rdparty/fcpp/cpp6.c"),

		    path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/mesa/**.c"),
		    path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/glsl/**.cpp"),
		    path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/mesa/**.h"),
		    path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/glsl/**.c"),
		    path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/glsl/**.cpp"),
		    path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/glsl/**.h"),
		    path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/util/**.c"),
		    path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/util/**.h"),

			path.join(LIB_DIR, "bgfx/3rdparty/scintilla/src/**.cxx"),
			path.join(LIB_DIR, "bgfx/3rdparty/scintilla/src/**.h"),
			path.join(LIB_DIR, "bgfx/3rdparty/scintilla/lexlib/**.cxx"),
			path.join(LIB_DIR, "bgfx/3rdparty/scintilla/lexlib/**.h"),
			path.join(LIB_DIR, "bgfx/3rdparty/scintilla/lexers/**.cxx"),
        }

        removefiles {
            path.join(LIB_DIR, "bgfx/src/amalgamated.cpp"),
            path.join(LIB_DIR, "bgfx/src/**.bin.h"),
            path.join(LIB_DIR, "bgfx/common/**.ttf.h"),
            path.join(LIB_DIR, "bgfx/common/**.bin.h"),

            path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/glsl/glcpp/glcpp.c"),
		    path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/glsl/glcpp/tests/**"),
		    path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/glsl/glcpp/**.l"),
		    path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/glsl/glcpp/**.y"),
		    path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/glsl/ir_set_program_inouts.cpp"),
		    path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/glsl/main.cpp"),
		    path.join(LIB_DIR, "bgfx/3rdparty/glsl-optimizer/src/glsl/builtin_stubs.cpp"),
        }

		defines {
			"DISABLED_SCI_NAMESPACE",
			"DISABLED_SCI_LEXER",
		}

		buildoptions {
--			"-Wno-missing-field-initializers"),
		}

        configuration "Debug"
            defines     { "TORQUE_DEBUG", "BGFX_CONFIG_DEBUG=1" }
            flags       { "Symbols" }

        configuration "Release"
            defines     {  }

        configuration "vs*"
            includedirs { path.join(LIB_DIR, "bgfx/include/compat/msvc") }
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
end
