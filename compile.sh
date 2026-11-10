FLAGS="-g -O0 -Wall -Wextra -fsanitize=address"
SRC="src/bitree.c src/dot_api.c src/bitree_dump.c"
gcc $FLAGS "$1" $SRC
