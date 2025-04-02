mkdir bin
cmake -D CMAKE_BUILD_TYPE=Release -S ./ -B ./bin
cmake --install ./bin
cmake --build ./bin