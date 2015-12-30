function Torque6()
    project "Torque6"
        targetname "Torque6"
        language "C++"
        kind "SharedLib"
        targetdir (BUILD_DIR)

        includedirs {
            path.join(LIB_DIR, "assimp/include"),
            path.join(LIB_DIR, "bgfx/include"),
            path.join(LIB_DIR, "bullet"),
            path.join(LIB_DIR, "bgfx/3rdparty"),
            path.join(LIB_DIR, "bgfx/common"),
            path.join(LIB_DIR, "zlib"),
            path.join(LIB_DIR, "lpng"),
            path.join(LIB_DIR, "ljpeg"),
            path.join(LIB_DIR, "openal/win32"),
            SRC_DIR,
            path.join(SRC_DIR, "persistence/rapidjson/include"),
            path.join(SRC_DIR, "persistence/libjson"),
            path.join(SRC_DIR, "testing/googleTest"),
            path.join(SRC_DIR, "testing/googleTest/include"),
            path.join(SRC_DIR, "spine"),
        }

        files {
            path.join(SRC_DIR, "**.h"),
            path.join(SRC_DIR, "**.cc"),
            path.join(SRC_DIR, "**.cpp"),
            path.join(SRC_DIR, "**.c"),
        }

        removefiles {
            path.join(SRC_DIR, "console/runtimeClassRep.cc"),
            path.join(SRC_DIR, "exe/**"),
            path.join(SRC_DIR, "input/leapMotion/**"),
            path.join(SRC_DIR, "graphics/bitmapPvr.cc"),
            path.join(SRC_DIR, "math/mMathAMD.cc"),
            path.join(SRC_DIR, "math/mMathSSE.cc"),
            path.join(SRC_DIR, "persistence/rapidjson/example/**"),
            path.join(SRC_DIR, "persistence/rapidjson/test/**"),
            path.join(SRC_DIR, "persistence/rapidjson/thirdparty/**"),
            path.join(SRC_DIR, "testing/googleTest/**"),
        }

        links {
            "assimp",
            "bgfx",
            "bullet",
            "ljpeg",
            "lpng",
            "zlib",
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
            targetname "Torque6_DEBUG"
            defines     { "TORQUE_DEBUG",
                          "TORQUE_ENABLE_PROFILER",
                          "TORQUE_DEBUG_GUARD",
                        }
            flags       { "Symbols" }

        configuration "vs*"
            defines         { "_CRT_SECURE_NO_WARNINGS", "UNICODE" }
            flags           { "NoNativeWChar" }
            buildoptions    { "/wd4100", "/wd4800" }
            includedirs     { path.join(LIB_DIR, "bgfx/include/compat/msvc"), }

        configuration "vs2015"
            windowstargetplatformversion "10.0.10240.0"

        configuration "windows"
            links { "COMCTL32",
                    "COMDLG32",
                    "USER32",
                    "ADVAPI32",
                    "GDI32",
                    "RPCRT4",
                    "WINMM",
                    "WSOCK32",
                    "vfw32",
                    "Imm32",
                    "shell32",
                    "shlwapi",
                    "ole32",
                    "psapi",
                    "d3dcompiler",
                    "dxguid",
                  }
            removefiles {
                    path.join(SRC_DIR, "platform/**.unix.cc"),
                    path.join(SRC_DIR, "platformAndroid/**"),
                    path.join(SRC_DIR, "platformEmscripten/**"),
                    path.join(SRC_DIR, "platformiOS/**"),
                    path.join(SRC_DIR, "platformOSX/**"),
                    path.join(SRC_DIR, "platformX86UNIX/**"),
                }

        configuration "linux"
            links       { "stdc++", "m", "dl", "pthread", "rt", "X11", "Xft", "SDL", "openal" }
            includedirs { "/usr/include/freetype2" }
            removefiles {
                    path.join(SRC_DIR, "input/leapMotion/**"),
                    path.join(SRC_DIR, "platformX86UNIX/x86UNIXDedicatedStub.cc"),
                    path.join(SRC_DIR, "platformAndroid/**"),
                    path.join(SRC_DIR, "platformEmscripten/**"),
                    path.join(SRC_DIR, "platformiOS/**"),
                    path.join(SRC_DIR, "platformOSX/**"),
                    path.join(SRC_DIR, "platformWin32/**"),
                }

        configuration "linux or bsd"
            links       { "m" }
            linkoptions { "-rdynamic", "-shared" }
            buildoptions { "-fpermissive", "-fPIC" }

        configuration "macosx"
            links       { "CoreServices.framework" }
            linkoptions {
                "-framework Cocoa",
                "-framework Metal",
                "-framework QuartzCore",
                "-framework OpenAL",
            }
            includedirs { path.join(LIB_DIR, "bgfx/include/compat/osx"), }
            files {
                path.join(SRC_DIR, "platformOSX/**.mm"),
            }
            removefiles {
                    path.join(SRC_DIR, "input/leapMotion/**"),
                    path.join(SRC_DIR, "platformX86UNIX/x86UNIXDedicatedStub.cc"),
                    path.join(SRC_DIR, "platformAndroid/**"),
                    path.join(SRC_DIR, "platformEmscripten/**"),
                    path.join(SRC_DIR, "platformiOS/**"),
                    path.join(SRC_DIR, "platformWin32/**"),
                    path.join(SRC_DIR, "platformX86UNIX/**"),
                    path.join(SRC_DIR, "testing/**"),
                }
end
