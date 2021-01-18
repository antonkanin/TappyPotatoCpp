# cmake_minimum_required(VERSION 3.17)
cmake_minimum_required(VERSION 3.10.2) # Android requires cmake 3.10.2

project(TappyPotatoCpp)

set(CMAKE_CXX_STANDARD 17)

if (NOT DEFINED CMAKE_BUILD_TYPE OR NOT DEFINED ${CMAKE_BUILD_TUPE})
    set(CMAKE_BUILD_TYPE "Release")
endif ()

if (${CMAKE_VERSION} GREATER_EQUAL "3.12")
    add_compile_definitions(DEBUG_OPENGL_ON) # enables glGetError check
    add_compile_definitions(TP_DEBUG)
elseif ()
    # TODO add compiler definitions for old cmake
endif ()

set(SOURCE_FILES
        src/glad/glad.c
        src/stb_image/stb_image.h

        src/AudioSystem.cpp
        src/Engine.cpp
        src/Engine.cpp
        src/EventSystem.cpp
        src/Image.cpp
        src/Log.cpp
        src/ShaderProgram.cpp
        src/TextRenderer.cpp
        src/Utils.cpp
        src/VideoSystem.cpp

        src/main.cpp)

if (ANDROID_BUILD)
    if (NOT DEFINED SDL2_SRC_DIR)
        message(FATAL_ERROR "SDL2 sources directory not defined")
    endif ()

    if (NOT DEFINED FREETYPE_SRC_DIR)
        message(FATAL_ERROR "FreeType sources directory not defined")
    endif ()

    add_library(TappyPotatoCpp SHARED ${SOURCE_FILES})

    set(SDL_CPUINFO_ENABLED_BY_DEFAULT OFF)

    message("ANDROID_ABI" ${ANDROID_ABI})
    add_subdirectory(${SDL2_SRC_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/ext_builds/${ANDROID_ABI}/sdl2_build)
    add_subdirectory(${FREETYPE_SRC_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/ext_builds/${ANDROID_ABI}/freetype_build)

    target_include_directories(TappyPotatoCpp PRIVATE
            ${SDL2_SRC_DIR}/include
            ${FREETYPE_SRC_DIR}/include)

    target_link_libraries(TappyPotatoCpp
            SDL2
            SDL2main
            freetype)

else () # not android
    find_package(SDL2 REQUIRED)
    find_package(Freetype REQUIRED)
    add_executable(TappyPotatoCpp ${SOURCE_FILES})

    if (UNIX)
        if (CMAKE_BUILD_TYPE MATCHES "Release")
            message("Unix Release Build")
            target_link_libraries(TappyPotatoCpp
                    SDL2
                    GL
                    Freetype::Freetype)

        elseif (CMAKE_BUILD_TYPE MATCHES "Debug")
            message("Unix Debug Build")
            target_link_libraries(TappyPotatoCpp
                    "/home/anton/projects/SDL2-2.0.12/build/libSDL2-2.0d.so"
                    GL
                    Freetype::Freetype)

        else ()
            message(FATAL_ERROR "Unsupported build type")
        endif ()

    elseif (WIN32)
        message("Windows Build")
        target_link_libraries(TappyPotatoCpp
                SDL2::SDL2
                opengl32
                Freetype::Freetype)
    else ()
        message(FATAL_ERROR "Platform not supported")
    endif ()
endif ()

target_include_directories(TappyPotatoCpp PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)
