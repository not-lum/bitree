#include "../include/bitree.h"
#include "../include/bitree_dump.h"

#define DOT_FILENAME "/tmp/tree_dump.dot"
#define IMG_FILENAME "./a.png"

int main() {
    Node *root = NULL;
    BiTreeStatus status = bt_create(50, &root);
    if (status != BITREE_OK) fprintf(stderr, "Error: %s", bt_error_str(status));

    status = bt_add_left(root, 10, NULL);
    if (status != BITREE_OK) fprintf(stderr, "Error: %s", bt_error_str(status));

    Node *node = NULL;
    status = bt_add_right(root, 70, &node);
    if (status != BITREE_OK) fprintf(stderr, "Error: %s", bt_error_str(status));

    status = bt_add_left(node, 60, NULL);
    if (status != BITREE_OK) fprintf(stderr, "Error: %s", bt_error_str(status));

    status = bt_add_right(node, 80, &node);
    if (status != BITREE_OK) fprintf(stderr, "Error: %s", bt_error_str(status));

    FILE *dot_file = fopen(DOT_FILENAME, "w");

    bt_dump_to_dot(root, dot_file);

    fclose(dot_file);

    status = compile_dot(DOT_FILENAME, IMG_FILENAME);
    if (status != BITREE_OK) fprintf(stderr, "Error: %s", bt_error_str(status));

    fputs("preorder: ", stdout);
    bt_print(root, BT_PREORDER, stdout);
    putc('\n', stdout);

    fputs("inorder: ", stdout);
    bt_print(root, BT_INORDER, stdout);
    putc('\n', stdout);

    fputs("postorder: ", stdout);
    bt_print(root, BT_POSTORDER, stdout);
    putc('\n', stdout);

    bt_destroy(root, NULL);
}