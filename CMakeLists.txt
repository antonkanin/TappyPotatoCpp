cmake_minimum_required(VERSION 3.17)
project(TappyPotatoCpp)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)

add_executable(TappyPotatoCpp
        main.cpp
        VideoSystem.cpp
        Engine.cpp EventSystem.hpp EventSystem.cpp)

target_link_libraries(TappyPotatoCpp
        SDL2
        GL)
