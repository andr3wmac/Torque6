	project "assimp"
		targetname "assimp"
		language "C++"
		kind "SharedLib"
		flags {
			"No64BitChecks",
			"ExtraWarnings",
			"StaticRuntime"
		}
		includedirs {
			"../engine/source",
            "../engine/lib/assimp/code/BoostWorkaround",
            "../engine/lib/assimp/include",
		}

		files {
			"../engine/lib/assimp/code/**.h",
            "../engine/lib/assimp/code/**.cpp",
			"../engine/lib/assimp/contrib/clipper/clipper.hpp",
			"../engine/lib/assimp/contrib/clipper/clipper.cpp",
			"../engine/lib/assimp/contrib/ConvertUTF/ConvertUTF.c",
			"../engine/lib/assimp/contrib/ConvertUTF/ConvertUTF.h",
			"../engine/lib/assimp/contrib/irrXML/**.h",
			"../engine/lib/assimp/contrib/irrXML/**.cpp",
			"../engine/lib/assimp/contrib/poly2tri/poly2tri/common/**.h",
			"../engine/lib/assimp/contrib/poly2tri/poly2tri/common/**.cc",
			"../engine/lib/assimp/contrib/poly2tri/poly2tri/sweep/**.h",
			"../engine/lib/assimp/contrib/poly2tri/poly2tri/sweep/**.cc",
            "../engine/lib/assimp/contrib/poly2tri/poly2tri/poly2tri.h",
			"../engine/lib/assimp/contrib/unzip/**.h",
			"../engine/lib/assimp/contrib/unzip/**.c",
			"../engine/lib/assimp/contrib/zlib/**.h",
			"../engine/lib/assimp/contrib/zlib/**.c",
            "../engine/lib/assimp/include/assimp/**.hpp",
            "../engine/lib/assimp/include/assimp/**.h",
		}

        removefiles {
			
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
