#include "../include/bitree.h"
#include "../include/bitree_dump.h"


int main() {
    Node *root = NULL;
    BiTreeStatus status = bt_create(20, &root);
    bt_add_right(root, 30, NULL);
    bt_add_left(root, 50, NULL);

    bt_serialize(root, stdout);
    fputc('\n', stdout);

    bt_destroy(root);

    char buff[] = "(20(50##)(30##))";
    Node *deserialized = NULL;
    bt_deserialize(buff, &deserialized);

    bt_serialize(deserialized, stdout);
    fputc('\n', stdout);

    bt_destroy(deserialized);

    return 0;
}
