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
} ChildKind;

typedef enum {
    BITREE_OK = 0,
    BITREE_ALLOC_ERROR,
    BITREE_UNKNOWN_CHILD_KIND,
    BITREE_ERROR
} BiTreeStatus;

BiTreeStatus bt_create(bitree_elem_t value, Node **out_root);
BiTreeStatus bt_destroy(Node *node);
BiTreeStatus bt_add(Node *node, ChildKind kind, bitree_elem_t value, Node **out_child);
BiTreeStatus bt_add_left(Node *node, bitree_elem_t value, Node **);
BiTreeStatus bt_add_right(Node *node, bitree_elem_t value, Node **);