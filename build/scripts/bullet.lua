function bullet()
    project "bullet"
        location (LIB_PROJECT_DIR)
        targetdir (LIB_BUILD_DIR)

        targetname "bullet"
        language "C++"
        kind "StaticLib"

        includedirs {
            path.join(LIB_DIR, "bullet/"),
        }

        files {
            path.join(LIB_DIR, "bullet/**.h"),
            path.join(LIB_DIR, "bullet/**.cpp"),
        }

        configuration { "windows", "x32", "Release" }
            targetdir (LIB_BUILD_DIR .. "/windows.x32.release")

        configuration { "windows", "x32", "Debug" }
            targetdir (LIB_BUILD_DIR .. "/windows.x32.debug")

        configuration { "windows", "x64", "Release" }
            targetdir (LIB_BUILD_DIR .. "/windows.x64.release")

        configuration { "windows", "x64", "Debug" }
            targetdir (LIB_BUILD_DIR .. "/windows.x64.debug")

        configuration "Debug"
            defines { "TORQUE_DEBUG" }
            flags   { "Symbols" }

        configuration "vs*"
            defines { "_CRT_SECURE_NO_WARNINGS" }

        configuration "vs2015"
            windowstargetplatformversion "10.0.10240.0"

        configuration "windows"
            links { "ole32" }

        configuration "linux"
            links { "dl" }

        configuration "linux or bsd"
            links        { "m" }
            linkoptions  { "-rdynamic" }
            buildoptions { "-fPIC" }

        configuration "macosx"

        configuration { "macosx", "gmake" }
            buildoptions { "-mmacosx-version-min=10.4" }
            linkoptions  { "-mmacosx-version-min=10.4" }
end
