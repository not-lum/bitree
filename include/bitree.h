#pragma once

#include <stdlib.h>

// BiTree configuration:
#ifndef BITREE_CUSTOM_CFG
#   include "../config/default_config.h"
#else
#   include BITREE_CUSTOM_CFG
#endif

typedef struct Node {
    bitree_elem_t value;
    struct Node *left;
    struct Node *right;
} Node;

typedef enum {
    BT_LEFT_CHILD = 0, 
    BT_RIGHT_CHILD
} BtChildKind;

typedef enum {
    BITREE_OK = 0,
    BITREE_ALLOC_ERROR,
    BITREE_UNABLE_TO_FORM_CMD,
    BITREE_ERROR
} BiTreeStatus;

typedef enum {
    BT_ENTER = 0,
    BT_IN,
    BT_LEAVE,
    BT_EMPTY
} BtWalkState;

typedef void (*bt_walk_func_t)(Node* node, BtWalkState state, void *ctx);
typedef void (*bt_destroy_elem_func_t)(bitree_elem_t *);

const char *bt_error_str(BiTreeStatus status);
BiTreeStatus bt_create(bitree_elem_t value, Node **out_root);
void bt_destroy(Node *node, bt_destroy_elem_func_t);
BiTreeStatus bt_add(Node *node, BtChildKind kind, bitree_elem_t value, Node **out_child);
BiTreeStatus bt_add_left(Node *node, bitree_elem_t value, Node **);
BiTreeStatus bt_add_right(Node *node, bitree_elem_t value, Node **);
BiTreeStatus bt_connect_left(Node *parent, Node *child);
BiTreeStatus bt_connect_right(Node *parent, Node *child);
BiTreeStatus bt_walk(Node *node, bt_walk_func_t func, void *ctx);