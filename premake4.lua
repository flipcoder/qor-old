solution("qor")
    configurations {"Debug", "Release"}

    location("build")
    targetdir("bin")

    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }
    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }

    project("qor")
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

        if (linux) then
            includedirs {"/usr/include/freetype2/**"}
        end

        includedirs {"third_party/include/"}
        libdirs {"third_party/lib/"}

        configuration { "gmake" }
            buildoptions { "-std=c++0x" }
