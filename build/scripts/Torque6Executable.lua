	project "Torque6Executable"
		targetname "torque6executable"
		language "C++"
		kind "ConsoleApp"

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
			defines     {  }

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
