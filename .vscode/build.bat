@ECHO OFF

SET args1=%1

if %args1% == generate (
    call "%VS2022INSTALLDIR%/VC/Auxiliary/Build/vcvars64.bat"
)

if %args1% == generate (
    @ECHO ON
    cmake -DCMAKE_BUILD_TYPE=Debug -G Ninja -DGLFW_INSTALL:BOOL=OFF -S ./ -B ./out/build/
    EXIT /B 0
)

if %args1% == build (
    @ECHO ON
    rem cmake --build ./out/build --target all -j 8
    ninja -C ./out/build/
    EXIT /B 0
)

EXIT /B -1
