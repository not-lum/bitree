#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// BiTree configuration:
#ifndef BITREE_CUSTOM_CFG
#   include "../config/default_config.h"
#else
#   include BITREE_CUSTOM_CFG
#endif

typedef struct Node {
    bitree_elem_t value;

    #ifdef BITREE_USE_PARENT_FIELD
    struct Node *parent;
    #endif

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

typedef void (*bt_deser_elem_func_t)(char *, bitree_elem_t *);
typedef void (*bt_ser_elem_func_t)(bitree_elem_t *, char *);

const char *bt_error_str(BiTreeStatus status);
BiTreeStatus bt_create(bitree_elem_t value, Node **out_root);
void bt_change(Node *node, bitree_elem_t value);
void bt_destroy(Node *node);
BiTreeStatus bt_add(Node *node, BtChildKind kind, bitree_elem_t value, Node **out_child);
BiTreeStatus bt_add_left(Node *node, bitree_elem_t value, Node **);
BiTreeStatus bt_add_right(Node *node, bitree_elem_t value, Node **);
BiTreeStatus bt_connect_left(Node *parent, Node *child);
BiTreeStatus bt_connect_right(Node *parent, Node *child);
Node *bt_get_child(Node *node, BtChildKind kind);
Node *bt_get_left(Node *node);
Node *bt_get_right(Node *node);
BiTreeStatus bt_walk(Node *node, bt_walk_func_t func, void *ctx);
bool node_is_leaf(Node *node);

BiTreeStatus bt_serialize(Node *root, FILE *out_file);
BiTreeStatus bt_deserialize(char *buff, Node **out_node);