cmake_minimum_required(VERSION 2.6)
project(qor)

if(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "GCC detected - Adding compiler flags")
	set(CMAKE_CXX_FLAGS "-pthread -ggdb -Wextra -Wall -std=c++0x")
    #set(CMAKE_CXX_FLAGS "-pthread -O2 -Wextra -Wall -std=c++0x")
endif(CMAKE_COMPILER_IS_GNUCXX)

file(GLOB_RECURSE SRCS ../src/*.cpp)
add_executable(../bin/qor ${SRCS})
target_link_libraries(../bin/qor GL GLU SDL SDLmain GLEW assimp IL ILU openal alut Newton ogg vorbis vorbisfile ftgl boost_system boost_filesystem lua5.1 luabind)

if(CMAKE_COMPILER_IS_GNUCXX)
	include_directories(/usr/include/freetype2/)
endif(CMAKE_COMPILER_IS_GNUCXX)

include_directories(../third_party/include/)

