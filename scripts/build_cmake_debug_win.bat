@ECHO OFF
SETLOCAL

SET BUILD_DIR=build

call "%VS2022INSTALLDIR%/VC/Auxiliary/Build/vcvars64.bat"

IF NOT EXIST %BUILD_DIR% (
    mkdir %BUILD_DIR%
)

cmake -DCMAKE_BUILD_TYPE=Debug -G "Ninja" -DGLFW_INSTALL=OFF -S . -B ./out/%BUILD_DIR%
cmake --build ./out/%BUILD_DIR% --target all -j 8

ENDLOCAL
