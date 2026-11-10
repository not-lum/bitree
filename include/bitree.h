#pragma once

#include <stdlib.h>

// BiTree configuration:
#include "../config/config.h"

typedef struct Node {
    tree_elem_t value;
    struct Node *left;
    struct Node *right;
} Node;

typedef enum {
    BITREE_OK = 0,
    BITREE_ALLOC_ERROR
} BiTreeStatus;

BiTreeStatus create_root(tree_elem_t value, Node **out_root);
BiTreeStatus node_destroy(Node *node);
BiTreeStatus add_left_child(Node *node, tree_elem_t value, Node **);
BiTreeStatus add_right_child(Node *node, tree_elem_t value, Node **);