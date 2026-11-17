FLAGS="-g -O0 -Wall -Wextra -fsanitize=address"
SRC="src/bitree.c src/dot_api.c src/bitree_dump.c src/bitree_serialization.c"

if [ -n "$2" ];
then
    CFG_FLAG="-DBITREE_CUSTOM_CFG=\"$2\""
fi

gcc $FLAGS $CFG_FLAG "$1" $SRC
