#include "../include/bitree.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


const char *bt_error_str(BiTreeStatus status) {
    switch (status) {
        case BITREE_OK:
            return "bitree ok";
        case BITREE_ALLOC_ERROR:
            return "bitree alloc failed";
        case BITREE_UNABLE_TO_FORM_CMD:
            return "unable to from cmd to compile dot";
        default:
            return "unknown error";
    }
}

BiTreeStatus bt_create(bitree_elem_t value, Node **out_root) {
    assert(out_root != NULL);

    Node *root = calloc(1, sizeof(Node));
    if (root == NULL)
        return BITREE_ALLOC_ERROR;

    root->value = value;

    *out_root = root;

    return BITREE_OK;
}

// TODO: rewrite via bt_walk:
void bt_destroy(Node *node, bt_destroy_elem_func_t destroyer) {
    assert(node != NULL);
    fprintf(stderr, ">>>>> node_addr: %p\n", node);
    fprintf(stderr, ">>>>> node_left: %p\n", node->left);
    fprintf(stderr, ">>>>> node_right: %p\n", node->right);

    if (node->left != NULL)
        bt_destroy(node->left, destroyer);
    if (node->right != NULL)
        bt_destroy(node->right, destroyer);

    if (destroyer != NULL)
        destroyer(&node->value);

    free(node);
}

BiTreeStatus bt_connect_left(Node *parent, Node *child) {
    assert(parent != NULL);
    assert(child != NULL);

    parent->left = child;

    return BITREE_OK;
}

BiTreeStatus bt_connect_right(Node *parent, Node *child) {
    assert(parent != NULL);
    assert(child != NULL);

    parent->right = child;

    return BITREE_OK;
}

BiTreeStatus bt_add(Node *parent, BtChildKind kind, bitree_elem_t value, Node **out_child) {
    assert(parent != NULL);

    Node *new_node = calloc(1, sizeof(Node));
    if (new_node == NULL)
        return BITREE_ALLOC_ERROR;

    new_node->value = value;

    if (kind == BT_LEFT_CHILD)
        parent->left = new_node;
    else
        parent->right = new_node;

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

BiTreeStatus bt_walk(Node *node, bt_walk_func_t func, void *ctx) {
    assert(func != NULL);

    if (node == NULL) {
        func(node, BT_EMPTY, ctx);
        return BITREE_OK;
    }

    func(node, BT_ENTER, ctx);
    bt_walk(node->left, func, ctx);

    func(node, BT_IN, ctx);

    bt_walk(node->right, func, ctx);
    func(node, BT_LEAVE, ctx);

    return BITREE_OK;
}


