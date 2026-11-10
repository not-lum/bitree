#include "../include/bitree.h"
#include "../include/bitree_dump.h"
#include <stdio.h>


int main() {
    Node *root = NULL;
    BiTreeStatus status = create_root("root", &root);
    
    Node *left_child = NULL;
    status = add_left_child(root, "left", &left_child);

    Node *right_child = NULL;
    status = add_right_child(root, "r1", &right_child);

    status = add_right_child(right_child, "r2", &right_child);
    status = add_right_child(right_child, "r3", &right_child);
    status = add_right_child(right_child, "r4", &right_child);

    FILE *logfile = fopen("log.dot", "w");

    size_t counter = 0;
    bitree_dump(root, logfile, &counter);

    fclose(logfile);

    node_destroy(root);
}