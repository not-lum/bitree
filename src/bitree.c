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
        case BITREE_WALK_STOPPED:
            return "walk on tree stopped";
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

    #ifdef BT_USE_PARENT_FIELD
    root->parent = NULL;
    #endif

    *out_root = root;

    return BITREE_OK;
}

// TODO: rewrite via bt_walk:
void bt_destroy(Node *node) {
    if (node == NULL)
        return;
    // fprintf(stderr, ">>>>> node_addr: %p\n", node);
    // fprintf(stderr, ">>>>> node_left: %p\n", node->left);
    // fprintf(stderr, ">>>>> node_right: %p\n", node->right);

    if (node->left != NULL)
        bt_destroy(node->left);
    if (node->right != NULL)
        bt_destroy(node->right);

    BT_DESTROY_ELEM(&node->value);

    free(node);
}

BiTreeStatus bt_connect(Node *parent, BtChildKind ch_kind, Node *child) {
    assert(parent != NULL);
    assert(child != NULL);

    #ifdef BT_USE_PARENT_FIELD
    child->parent = parent;
    #endif

    if (ch_kind == BT_LEFT_CHILD)
        parent->left = child;
    else
        parent->right = child;

    return BITREE_OK;
}

BiTreeStatus bt_connect_left(Node *parent, Node *child) {
    return bt_connect(parent, BT_LEFT_CHILD, child);
}

BiTreeStatus bt_connect_right(Node *parent, Node *child) {
    return bt_connect(parent, BT_RIGHT_CHILD, child);
}

BiTreeStatus bt_add(Node *parent, BtChildKind kind, bitree_elem_t value, Node **out_child) {
    assert(parent != NULL);

    Node *new_node = calloc(1, sizeof(Node));
    if (new_node == NULL)
        return BITREE_ALLOC_ERROR;

    new_node->value = value;

    #ifdef BT_USE_PARENT_FIELD
    new_node->parent = parent;
    #endif

    if (kind == BT_LEFT_CHILD)
        parent->left = new_node;
    else
        parent->right = new_node;

    if (out_child != NULL)
        *out_child = new_node;

    return BITREE_OK;
}


Node *bt_get_child(Node *node, BtChildKind kind) {
    assert(node != NULL);

    if (kind == BT_LEFT_CHILD)
        return node->left;
    else 
        return node->right;
}

Node *bt_get_left(Node *node) {
    return bt_get_child(node, BT_LEFT_CHILD);
}

Node *bt_get_right(Node *node) {
    return bt_get_child(node, BT_RIGHT_CHILD);
}

BiTreeStatus bt_add_right(Node *node, bitree_elem_t value, Node **out_child) {
    return bt_add(node, BT_RIGHT_CHILD, value, out_child);
}

BiTreeStatus bt_add_left(Node *node, bitree_elem_t value, Node **out_child) {
    return bt_add(node, BT_LEFT_CHILD, value, out_child);
}

void bt_change(Node *node, bitree_elem_t value) {
    assert(node != NULL);

    node->value = value;
}

BiTreeStatus bt_walk(Node *node, bt_walk_func_t func, void *ctx) {
    assert(func != NULL);

    if (node == NULL) {
        func(node, BT_EMPTY, ctx);
        return BITREE_OK;
    }

    if (func(node, BT_ENTER, ctx))
        return BITREE_WALK_STOPPED;

    if (bt_walk(node->left, func, ctx) != BITREE_OK)
        return BITREE_WALK_STOPPED;

    if (func(node, BT_IN, ctx))
        return BITREE_WALK_STOPPED;

    if (bt_walk(node->right, func, ctx) != BITREE_OK)
        return BITREE_WALK_STOPPED;

    if (func(node, BT_LEAVE, ctx))
        return BITREE_WALK_STOPPED;

    return BITREE_OK;
}

bool node_is_leaf(Node *node) {
    assert(node != NULL);

    if (node->left == NULL && node->right == NULL)
        return true;
    
    return false;
}
