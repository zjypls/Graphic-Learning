cd ./Include/GLFW
cmake ./ -G "MinGW Makefiles"
make -j 8
cd ../../
cmake -B build
cd build
make -j 12