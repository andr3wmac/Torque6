    project "Torque6"
        targetname "Torque6"
        language "C++"
        kind "SharedLib"

        includedirs {
            "../../engine/lib/assimp/include",
            "../../engine/lib/bgfx/include",
            "../../engine/lib/bullet",
            "../../engine/lib/bgfx/3rdparty",
            "../../engine/lib/bgfx/common",
            "../../engine/lib/LeapSDK/include",
            "../../engine/lib/zlib",
            "../../engine/lib/lpng",
            "../../engine/lib/ljpeg",
            "../../engine/lib/openal/win32",
            "../../engine/source",
            "../../engine/source/persistence/rapidjson/include",
            "../../engine/source/persistence/libjson",
            "../../engine/source/testing/googleTest",
            "../../engine/source/testing/googleTest/include",
            "../../engine/source/spine",
        }

        files {
            "../../engine/source/**.h",
            "../../engine/source/**.cc",
            "../../engine/source/**.cpp",
            "../../engine/source/**.c",
        }

        removefiles {
            "../../engine/source/exe/**",
            "../../engine/source/graphics/bitmapPvr.cc",
            "../../engine/source/persistence/rapidjson/example/**",
            "../../engine/source/persistence/rapidjson/test/**",
            "../../engine/source/persistence/rapidjson/thirdparty/**",
            "../../engine/source/testing/googleTest/**",
            "../../engine/source/console/runtimeClassRep.cc",
            "../../engine/source/math/mMathAMD.cc",
            "../../engine/source/math/mMathSSE.cc",
        }

        libdirs { "$(DXSDK_DIR)/Lib/x86",
                  "../../engine/lib/LeapSDK/lib/x86" }

        links {
            "assimp",
            "bgfx",
            "bullet",
            "ljpeg",
            "lpng",
            "zlib",
        }

        configuration "Debug"
            targetname "Torque6_DEBUG"
            defines     { "TORQUE_DEBUG",
                          "TORQUE_ENABLE_PROFILER",
                          "TORQUE_DEBUG_GUARD",
                        }
            flags       { "Symbols" }

        configuration "Release"
            defines     {  }

        configuration "vs*"
            defines         { "_CRT_SECURE_NO_WARNINGS", "UNICODE" }
            flags           { "NoNativeWChar" }
            buildoptions    { "/wd4100", "/wd4800" }
            includedirs     { "../../engine/lib/bgfx/include/compat/msvc", }

        configuration "windows"
            targetdir   "../bin/windows"
            links { "Leapd",
                    "COMCTL32",
                    "COMDLG32",
                    "USER32",
                    "ADVAPI32",
                    "GDI32",
                    "RPCRT4",
                    "WINMM",
                    "WSOCK32",
                    "vfw32",
                    "Imm32",
                    "unicows",
                    "shell32",
                    "shlwapi",
                    "ole32",
                    "psapi",
                    "d3dcompiler",
                    "dxguid",
                  }
            removefiles {
                    "../../engine/source/platform/**.unix.cc",
                    "../../engine/source/platformAndroid/**",
                    "../../engine/source/platformEmscripten/**",
                    "../../engine/source/platformiOS/**",
                    "../../engine/source/platformOSX/**",
                    "../../engine/source/platformX86UNIX/**",
                }

        configuration "linux"
            targetdir   "../bin/linux"
            links       { "stdc++", "m", "dl", "pthread", "rt", "X11", "Xft", "SDL", "openal" }
            includedirs { "/usr/include/freetype2" }
            removefiles {
                    "../../engine/source/input/leapMotion/**",
                    "../../engine/source/platformX86UNIX/x86UNIXDedicatedStub.cc",
                    "../../engine/source/platformAndroid/**",
                    "../../engine/source/platformEmscripten/**",
                    "../../engine/source/platformiOS/**",
                    "../../engine/source/platformOSX/**",
                    "../../engine/source/platformWin32/**",
                }

        configuration "bsd"
            targetdir   "../bin/bsd"

        configuration "linux or bsd"
            defines     {  }
            links       { "m" }
            linkoptions { "-rdynamic", "-shared" }
            buildoptions { "-fpermissive", "-fPIC" }

        configuration "macosx"
            targetdir   "../bin/darwin"
            defines     {  }
            links       { "CoreServices.framework" }

        configuration { "macosx", "gmake" }
            buildoptions { "-mmacosx-version-min=10.4" }
            linkoptions  { "-mmacosx-version-min=10.4" }
