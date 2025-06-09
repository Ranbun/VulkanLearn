
cmake -DCMAKE_BUILD_TYPE=Debug -G "Ninja" -DGLFW_INSTALL=OFF -S . -B ./out/build

ninja -C ./out/build/
