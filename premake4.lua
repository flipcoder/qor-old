solution("qor")
    configurations {"Debug", "Release"}

    targetdir("bin")
    
    project("qor")
        uuid("51577de0-6128-11e1-b86c-0800200c9a66")
        kind("WindowedApp")
        language("C++")
        links {
            "pthread",
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
            
            "SimulationController",
            "SceneQuery",
            "LowLevel",
            "LowLevelCloth",
            "PhysX3",
            "PhysX3Vehicle",
            "PhysX3Cooking",
            "PhysX3Extensions",
            "PhysX3CharacterKinematic",
            "RepX3",
            "RepXUpgrader3",
            "PhysXProfileSDK",
            "PhysXVisualDebuggerSDK","PxTask",
            "PxTask",
            "PhysX3Common",

            --"SimulationControllerCHECKED",
            --"SceneQueryCHECKED",
            --"LowLevelCHECKED",
            --"LowLevelClothCHECKED",
            --"PhysX3CHECKED",
            --"PhysX3VehicleCHECKED",
            --"PhysX3CookingCHECKED",
            --"PhysX3ExtensionsCHECKED",
            --"PhysX3CharacterKinematicCHECKED",
            --"RepX3CHECKED",
            --"RepXUpgrader3CHECKED",
            --"PhysXProfileSDKCHECKED",
            --"PhysXVisualDebuggerSDKCHECKED","PxTaskCHECKED",
            --"PxTask",
            --"PhysX3CommonCHECKED",

            --"Newton",
            --"dJointLibrary",
            --"BulletSoftBody",
            --"BulletDynamics",
            --"BulletCollision",
            --"LinearMath",
            "ogg",
            "vorbis",
            "vorbisfile",
            "ftgl",
            "boost_system",
            "boost_filesystem",
            --"lua5.1",
            --"luabind"
            "angelscript"
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
                "/usr/local/include/physx"
            }
            links {
                "rt"
            }
        configuration {}

        configuration {"windows"}
            defines { "FS_IGNORE_CASE" }
        configuration {}

        includedirs {"third_party/include/"}
        libdirs {"third_party/lib/"}

        configuration { "gmake" }
            buildoptions { "-fno-strict-aliasing -std=c++0x" }
            linkoptions { "" }
        configuration {}

        configuration { "Debug" }
            defines { "DEBUG", "_DEBUG" }
            flags { "Symbols" }
        configuration { "Release" }
            defines { "NDEBUG" }
            flags { "Optimize" }
        configuration {}

