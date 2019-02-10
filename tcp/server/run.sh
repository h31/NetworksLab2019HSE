mkdir -p build
cd build
cmake --quiet ..
make hw01-calc-server -s
if (( $# < 1 )); then
    ./hw01-calc-server
else
    ./hw01-calc-server $1
fi