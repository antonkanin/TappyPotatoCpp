IF not exist build-cmake (mkdir build-cmake)

cd build-cmake
cmake -G "CodeBlocks - MinGW Makefiles" ..
cmake --build . -j 9
close
