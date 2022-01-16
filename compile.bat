clang-format.exe -i -style=file ".clang-format" ./src/*.cpp ./src/*.hpp
if not exist build mkdir build
cd build
cmake -S ../ -B . -G "MinGW Makefiles"
mingw32-make.exe && mingw32-make.exe Shaders
cd ..
