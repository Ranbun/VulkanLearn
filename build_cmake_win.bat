
cmake -DCMAKE_BUILD_TYPE=Debug -G "Ninja" -DGLFW_INSTALL=OFF -DCMAKE_TOOLCHAIN_FILE=D:\OpenSources\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B ./out/build
ninja -C ./out/build/  -j8
