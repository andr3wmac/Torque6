function Torque6App()
    project "Torque6App"
        targetname "Torque6App"
        language "C++"
        kind "WindowedApp"
        debugdir (PROJECT)
        targetdir (BUILD_DIR)

        includedirs {
            SRC_DIR,
        }

        files {
            path.join(SRC_DIR, "exe/main.cpp")
        }

        links {
            "Torque6"
        }

        configuration { "windows", "x32", "Release" }
            targetdir (BUILD_DIR .. "/windows.x32.release")

        configuration { "windows", "x32", "Debug" }
            targetdir (BUILD_DIR .. "/windows.x32.debug")

        configuration { "windows", "x64", "Release" }
            targetdir (BUILD_DIR .. "/windows.x64.release")

        configuration { "windows", "x64", "Debug" }
            targetdir (BUILD_DIR .. "/windows.x64.debug")

        configuration "Release"
            defines     { "TORQUE_ENABLE_PROFILER" }

        configuration "Debug"
            targetname "Torque6App_DEBUG"
            defines     { "TORQUE_DEBUG", "TORQUE_ENABLE_PROFILER" }
            flags       { "Symbols" }

        configuration "vs*"
            defines     { "_CRT_SECURE_NO_WARNINGS" }

        configuration "vs2015"
            windowstargetplatformversion "10.0.10240.0"

        configuration "windows"
            links { "ole32" }

        configuration "linux"
            links       { "dl" }
            linkoptions { "-rdynamic" }

        configuration "linux or bsd"
            links       { "m" }
            linkoptions { "-rdynamic" }

        configuration "macosx"
end
