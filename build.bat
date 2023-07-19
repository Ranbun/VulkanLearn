call "%VS2022INSTALLDIR%/VC/Auxiliary/Build/vcvars64.bat"

cmake.exe -DCMAKE_BUILD_TYPE=Debug -G Ninja -DGLFW_INSTALL:BOOL=OFF -S ./ -B ./build
cmake.exe --build ./build --target all -j 8
