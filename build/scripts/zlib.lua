	project "zlib"
        location (BUILD_DIR .. "lib")
        targetdir (BUILD_DIR .. "lib/bin")

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
			defines     {  }
			links       { "CoreServices.framework" }

		configuration { "macosx", "gmake" }
			buildoptions { "-mmacosx-version-min=10.4" }
			linkoptions  { "-mmacosx-version-min=10.4" }
