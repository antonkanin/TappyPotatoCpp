# cmake_minimum_required(VERSION 3.17)
cmake_minimum_required(VERSION 3.10.2) # Android requires cmake 3.10.2

project(TappyPotatoCpp)

set(SOURCE_FILES
        src/glad/glad.c

        src/main.cpp
        src/VideoSystem.cpp
        src/Engine.cpp
        src/EventSystem.cpp
        src/Log.cpp)

if (SDL2_SRC_DIR)
    message("Android SDL")
    add_subdirectory( # Specifies the directory of the CMakeLists.txt file.
            ${SDL2_SRC_DIR}
            # Specifies the directory for the build outputs.
            ${SDL2_BUILD_DIR})
    add_library(TappyPotatoCpp SHARED ${SOURCE_FILES})
else ()
    find_package(SDL2 REQUIRED)
    add_executable(TappyPotatoCpp ${SOURCE_FILES})
endif ()

#set(CMAKE_CXX_STANDARD )

if (UNIX)
    if (SDL2_SRC_DIR)
        target_include_directories(TappyPotatoCpp PRIVATE ${SDL2_SRC_DIR}/include)
        target_link_libraries(TappyPotatoCpp
                SDL2
                SDL2main
                )
    else ()
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
