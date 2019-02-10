mkdir -p build
cd build
cmake --quiet ..
make hw01-calc-server -s
./hw01-calc-server