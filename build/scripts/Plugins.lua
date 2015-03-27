function torque6Plugin(_name)
	project (_name)
        location ("../" .. _ACTION .. "/plugins/")

		targetname (_name)
		language "C++"
		kind "SharedLib"
		flags {
			"No64BitChecks",
			"ExtraWarnings",
			"StaticRuntime"
		}
		includedirs {
			"../../engine/source"
		}

		files {
			"../../plugins/" .. _name .. "/source/**.h",
			"../../plugins/" .. _name .. "/source/**.cpp",
            "../../plugins/" .. _name .. "/source/**.cc",
		}

        links {
		    "Torque6"
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
end
