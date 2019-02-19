mkdir -p build
cd build
cmake --quiet ..
make hw01-chat-client -s
if (( $# < 2 )); then
    ./hw01-chat-client
else
    ./hw01-chat-client $1 $2
fi