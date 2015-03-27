	project "Torque6Executable"
		targetname "Torque6"
		language "C++"
		kind "WindowedApp"

		includedirs {
			"../../engine/source/"
		}

		files {
			"../../engine/source/exe/main.cpp"
		}

        links {
		    "Torque6"
	    }

		configuration "Debug"
            targetname "Torque6_DEBUG"
			defines     { "TORQUE_DEBUG" }
            buildoptions { "/DEBUG" }

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
