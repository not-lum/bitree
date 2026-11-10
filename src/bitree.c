#include "../include/bitree.h"

#include <stdlib.h>
#include <assert.h>


BiTreeStatus create_root(tree_elem_t value, Node **out_root) {
    assert(out_root != NULL);

    Node *root = calloc(1, sizeof(Node));
    if (root == NULL)
        return BITREE_ALLOC_ERROR;

    root->value = value;

    *out_root = root;

    return BITREE_OK;
}

BiTreeStatus node_destroy(Node *node)  {
    assert(node != NULL);

    if (node->left != NULL)
        node_destroy(node->left);
    if (node->right != NULL)
        node_destroy(node->right);

    free(node);

    return BITREE_OK;
}

BiTreeStatus add_left_child(Node *node, tree_elem_t value, Node **out_child) {
    assert(node != NULL);
    assert(out_child != NULL);

    Node *new_node = calloc(1, sizeof(Node));
    if (new_node == NULL)
        return BITREE_ALLOC_ERROR;

    new_node->value = value;
    node->left = new_node;

    *out_child = new_node;

    return BITREE_OK;
}

BiTreeStatus add_right_child(Node *node, tree_elem_t value, Node **out_child) {
    assert(node != NULL);
    assert(out_child != NULL);

    Node *new_node = calloc(1, sizeof(Node));
    if (new_node == NULL)
        return BITREE_ALLOC_ERROR;

    new_node->value = value;
    node->right = new_node;

    *out_child = new_node;

    return BITREE_OK;
}



