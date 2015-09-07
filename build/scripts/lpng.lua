function lpng()
    project "lpng"
        location (BUILD_DIR .. "lib")
        targetdir (BUILD_DIR .. "lib/bin")

        targetname "lpng"
        language "C++"
        kind "StaticLib"

        includedirs {
            SRC_DIR,
            path.join(LIB_DIR, "lpng"),
            path.join(LIB_DIR, "zlib"),
        }

        files {
            path.join(LIB_DIR, "lpng/**.h"),
            path.join(LIB_DIR, "lpng/**.c"),
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

        configuration "vs2015"
            windowstargetplatformversion "10.0.10240.0"

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
end
