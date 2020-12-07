cmake_minimum_required(VERSION 3.17)
project(TappyPotatoCpp)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)

add_executable(TappyPotatoCpp
        main.cpp
        VideoSystem.cpp
        Engine.cpp
        EventSystem.cpp)

if (UNIX)
    target_link_libraries(TappyPotatoCpp
            SDL2
            GL)

elseif (WIN32)
    target_link_libraries(TappyPotatoCpp
            SDL2
            opengl32)

else ()
    message(FATAL_ERROR "Platform not supported")

endif ()