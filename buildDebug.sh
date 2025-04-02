mkdir bin
cmake -D CMAKE_BUILD_TYPE=Debug -S ./ -B ./bin
cmake --install ./bin
cmake --build ./bin