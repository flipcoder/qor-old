solution("qor")
    configurations {"Debug", "Release"}

    targetdir("bin")
    
    project("qor")
        uuid("51577de0-6128-11e1-b86c-0800200c9a66")
        kind("WindowedApp")
        language("C++")
        links {
            "GL",
            "GLU",
            "SDL",
            "SDLmain",
            "GLEW",
            "assimp",
            "IL",
            "ILU",
            "openal",
            "alut",
            "Newton",
            "dJointLibrary",
            --"BulletSoftBody",
            --"BulletDynamics",
            --"BulletCollision",
            "LinearMath",
            "ogg",
            "vorbis",
            "vorbisfile",
            "ftgl",
            "boost_system",
            "boost_filesystem",
            "lua5.1",
            "luabind"
        }
        files {
            "src/**.h",
            "src/**.cpp"
        }
        excludes {
            "src/mod_**"
        }

        configuration { "linux" }
            includedirs {
                "/usr/include/freetype2",
                "/usr/include/newton"
            }
        configuration {}

        includedirs {"third_party/include/"}
        libdirs {"third_party/lib/"}

        configuration { "gmake" }
            buildoptions { "-std=c++0x" }
            linkoptions { "" }
        configuration {}

        configuration { "Debug" }
            defines { "DEBUG" }
            flags { "Symbols" }
        configuration { "Release" }
            defines { "NDEBUG" }
            flags { "Optimize" }
        configuration {}

