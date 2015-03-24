	project "bgfx"
		targetname "bgfx"
		language "C++"
		kind "SharedLib"
		flags {
			"No64BitChecks",
			"ExtraWarnings",
			"StaticRuntime"
		}
		includedirs {
			"../engine/source",
            "../engine/lib/bgfx/include/bx",
            "../engine/lib/bgfx/include",
            "../engine/lib/bgfx/include/compat/msvc",
            "../engine/lib/bgfx/3rdparty",
            "../engine/lib/bgfx/3rdparty/khronos",
            "../engine/lib/bgfx/3rdparty/fcpp",
            "../engine/lib/bgfx/3rdparty/glsl-optimizer/include",
            "../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl",
            "../engine/lib/bgfx/3rdparty/glsl-optimizer/src/mapi",
            "../engine/lib/bgfx/3rdparty/glsl-optimizer/src/mesa",
            "../engine/lib/bgfx/3rdparty/glsl-optimizer/src/getopt",
            "../engine/lib/bgfx/3rdparty/glsl-optimizer/src",
            "$(DXSDK_DIR)/Include",
		}

		files {
			"../engine/lib/bgfx/src/**.h",
            "../engine/lib/bgfx/src/**.c",
            "../engine/lib/bgfx/src/**.cpp",
            "../engine/lib/bgfx/3rdparty/fcpp/**.c",
			"../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/glcpp/**.c",
			"../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/**.h",
			"../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/**.cpp",
			"../engine/lib/bgfx/3rdparty/glsl-optimizer/src/glsl/**.c",
			"../engine/lib/bgfx/3rdparty/glsl-optimizer/src/mesa/main/imports.c",
			"../engine/lib/bgfx/3rdparty/glsl-optimizer/src/mesa/program/prog_hash_table.c",
			"../engine/lib/bgfx/3rdparty/glsl-optimizer/src/mesa/program/symbol_table.c",
			"../engine/lib/bgfx/3rdparty/glsl-optimizer/src/util/hash_table.c",
			"../engine/lib/bgfx/3rdparty/glsl-optimizer/src/util/ralloc.c",
            "../engine/lib/bgfx/common/imgui/**.cpp",
			"../engine/lib/bgfx/common/imgui/**.h",
            "../engine/lib/bgfx/common/imgui/**.cpp",
			"../engine/lib/bgfx/common/nanovg/**.h",
            "../engine/lib/bgfx/common/nanovg/**.cpp",
            "../engine/lib/bgfx/include/**.h",
			"../engine/lib/bgfx/tools/shaderc/**.h",
            "../engine/lib/bgfx/tools/shaderc/**.cpp",
		}

        removefiles {
			"../engine/lib/bgfx/3rdparty/fcpp/usecpp.c",
        }

		configuration "Debug"
			defines     { "_DEBUG", "LUA_COMPAT_MODULE" }
			flags       { "Symbols" }

		configuration "Release"
			defines     { "NDEBUG", "LUA_COMPAT_MODULE" }
			flags       { "OptimizeSize" }

		configuration "vs*"
			defines     { "_CRT_SECURE_NO_WARNINGS" }

		configuration "windows"
			targetdir   "../bin/windows"
			links { "ole32" }

		configuration "linux"
			targetdir   "../bin/linux"
			links       { "dl" }

		configuration "bsd"
			targetdir   "../bin/bsd"

		configuration "linux or bsd"
			defines     { "LUA_USE_POSIX", "LUA_USE_DLOPEN" }
			links       { "m" }
			linkoptions { "-rdynamic" }

		configuration "macosx"
			targetdir   "../bin/darwin"
			defines     { "LUA_USE_MACOSX" }
			links       { "CoreServices.framework" }

		configuration { "macosx", "gmake" }
			buildoptions { "-mmacosx-version-min=10.4" }
			linkoptions  { "-mmacosx-version-min=10.4" }
