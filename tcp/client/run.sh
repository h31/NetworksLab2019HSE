mkdir -p build
cd build
cmake --quiet ..
make hw01-chat-client -s
if (( $# < 2 )); then
    ./hw01-calc-server
else
    ./hw01-calc-server $1 $2
fi