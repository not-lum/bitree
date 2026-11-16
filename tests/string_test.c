// compile with -DBITREE_CUSTOM_CFG="../config/string_config.h"
// or via compile.sh: ./compile.sh test/string_test.c "../config/string_config.h"

#include "../include/bitree.h"
#include "../include/bitree_dump.h"
#include <stdio.h>

#define DOT_FILENAME "/tmp/tree_dump.dot"
#define IMG_FILENAME "./a.png"

int main() {
    Node *root = NULL;
    BiTreeStatus status = bt_create("root", &root);
    
    Node *left_child = NULL;
    status = bt_add_left(root, "left", &left_child);
    status = bt_add_left(left_child, "left of left", &left_child);
    status = bt_add_right(left_child, "right of left of left", NULL);

    Node *right_child = NULL;
    status = bt_add_right(root, "abracadabra", &right_child);
    status = bt_add_right(right_child, "bibobabobus", &right_child);
    status = bt_add_right(right_child, "fffff", &right_child);
    status = bt_add_right(right_child, "aaaaa aa", &right_child);

    FILE *logfile = fopen(DOT_FILENAME, "w");
    bt_dump_to_dot(root, logfile);
    fclose(logfile);

    compile_dot(DOT_FILENAME, IMG_FILENAME);

    FILE *dumpfile = fopen("dump.txt", "w");
    bt_print(root, BT_PREORDER, dumpfile);
    fclose(dumpfile);

    bt_destroy(root, NULL);
}