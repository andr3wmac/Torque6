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
			"../../engine/source",
            "../../engine/lib/assimp/include",
            "../../engine/lib/bgfx/include",
            "../../engine/lib/bgfx/common/imgui",
            "../../engine/lib/bgfx/common/nanovg",
            "../../engine/lib/openal/win32",
		}

		files {
			"../../plugins/" .. _name .. "/source/**.h",
			"../../plugins/" .. _name .. "/source/**.cpp",
            "../../plugins/" .. _name .. "/source/**.cc",
		}

        links {
		    "Torque6"
	    }
     
        defines {
            "TORQUE_PLUGIN",
            "_USRDLL"
        }

		configuration "Debug"
			defines     { "TORQUE_DEBUG" }
			flags       {  }
            buildoptions { "/DEBUG" }

		configuration "Release"
			defines     { }
			flags       { }

		configuration "vs*"
			defines     { "_CRT_SECURE_NO_WARNINGS" }

		configuration "windows"
			targetdir   "../bin/windows"
			links { "ole32" }
            defines { "WIN32", "_WINDOWS" }

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
