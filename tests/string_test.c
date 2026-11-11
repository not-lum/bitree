// compile with -DBITREE_CUSTOM_CFG="../config/string_config.h"
// or via compile.sh: ./compile.sh test/string_test.c "../config/string_config.h"

#include "../include/bitree.h"
#include "../include/bitree_dump.h"
#include <stdio.h>


int main() {
    Node *root = NULL;
    BiTreeStatus status = bt_create("root", &root);
    
    Node *left_child = NULL;
    status = bt_add_left(root, "left", &left_child);

    Node *right_child = NULL;
    status = bt_add_right(root, "r1", &right_child);
    status = bt_add_right(right_child, "r2", &right_child);
    status = bt_add_right(right_child, "r3", &right_child);
    status = bt_add_right(right_child, "r4", &right_child);

    FILE *logfile = fopen("log.dot", "w");

    size_t counter = 0;
    bt_dump_to_dot(root, logfile, &counter);

    fclose(logfile);

    bt_destroy(root);
}