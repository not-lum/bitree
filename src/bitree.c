#include "../include/bitree.h"

#include <stdlib.h>
#include <assert.h>


BiTreeStatus bt_create(bitree_elem_t value, Node **out_root) {
    assert(out_root != NULL);

    Node *root = calloc(1, sizeof(Node));
    if (root == NULL)
        return BITREE_ALLOC_ERROR;

    root->value = value;

    *out_root = root;

    return BITREE_OK;
}

BiTreeStatus bt_destroy(Node *node)  {
    assert(node != NULL);

    if (node->left != NULL)
        bt_destroy(node->left);
    if (node->right != NULL)
        bt_destroy(node->right);

    free(node);

    return BITREE_OK;
}

BiTreeStatus bt_add(Node *parent, ChildKind kind, bitree_elem_t value, Node **out_child) {
    assert(parent != NULL);

    Node *new_node = calloc(1, sizeof(Node));
    if (new_node == NULL)
        return BITREE_ALLOC_ERROR;

    new_node->value = value;

    if (kind == BT_LEFT_CHILD)
        parent->left = new_node;
    else if (kind == BT_RIGHT_CHILD)
        parent->right = new_node;
    else
        return BITREE_UNKNOWN_CHILD_KIND;

    if (out_child != NULL)
        *out_child = new_node;

    return BITREE_OK;
}

BiTreeStatus bt_add_right(Node *node, bitree_elem_t value, Node **out_child) { // TODO: do we have to repeat asserts?
    return bt_add(node, BT_RIGHT_CHILD, value, out_child);
}

BiTreeStatus bt_add_left(Node *node, bitree_elem_t value, Node **out_child) {
    return bt_add(node, BT_LEFT_CHILD, value, out_child);
}



