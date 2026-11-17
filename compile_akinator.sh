FLAGS="-g -O0 -Wall -Wextra -fsanitize=address"
SRC="src/bitree.c src/dot_api.c src/bitree_dump.c src/bitree_serialization.c akinator/main.c akinator/file_utils.c akinator/akinator_utils.c config/akinator_config.c"
CFG_FLAG="-DBITREE_CUSTOM_CFG=\"../config/akinator_config.h\""

gcc $FLAGS $CFG_FLAG $SRC

