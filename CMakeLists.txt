cmake_minimum_required(VERSION 3.17)
project(TappyPotatoCpp)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)

add_executable(TappyPotatoCpp
        src/main.cpp
        src/VideoSystem.cpp
        src/Engine.cpp
        src/EventSystem.cpp
        src/glad/glad.c
        )

if (UNIX)
    if (CMAKE_BUILD_TYPE MATCHES "Release")
        message("Unix Release Build")
        target_link_libraries(TappyPotatoCpp
                SDL2
                GL
                )

    elseif (CMAKE_BUILD_TYPE MATCHES "Debug")
        message("Unix Debug Build")
        target_link_libraries(TappyPotatoCpp
                "/home/anton/projects/SDL2-2.0.12/build/libSDL2-2.0d.so"
                GL
                )

    endif ()
elseif (WIN32)
    message("Windows Build")
    target_link_libraries(TappyPotatoCpp
            SDL2
            opengl32
            )
else ()
    message(FATAL_ERROR "Platform not supported")
endif ()

target_include_directories(TappyPotatoCpp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
