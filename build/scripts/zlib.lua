	project "zlib"
        location ("../" .. _ACTION .. "/lib/")

		targetname "zlib"
		language "C++"
		kind "StaticLib"

		includedirs {
			"../../engine/source"
		}

		files {
			"../../engine/lib/zlib/**.h",
            "../../engine/lib/zlib/**.c",
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
			targetdir   "../bin/windows"
			links { "ole32" }

		configuration "linux"
			targetdir   "../bin/linux"
			links       { "dl" }

		configuration "bsd"
			targetdir   "../bin/bsd"

		configuration "linux or bsd"
			defines     {  }
			links       { "m" }
			linkoptions { "-rdynamic" }

		configuration "macosx"
			targetdir   "../bin/darwin"
			defines     {  }
			links       { "CoreServices.framework" }

		configuration { "macosx", "gmake" }
			buildoptions { "-mmacosx-version-min=10.4" }
			linkoptions  { "-mmacosx-version-min=10.4" }
