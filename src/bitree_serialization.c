#include "../include/bitree_dump.h"

#include <assert.h>


BiTreeStatus bt_serialize(Node *root,
                          FILE *out_file) {
    assert(root != NULL);
    assert(out_file != NULL);

    bt_print(root, BT_PREORDER, out_file);
    
    return BITREE_OK;
}

static BiTreeStatus bt_deserialize_body(char *buff, size_t *counter,
                                        Node **out_node) {
    assert(buff != NULL);
    assert(out_node != NULL);
    
    // fprintf(stderr, "buff: <%s>\n", buff + *counter);
    // fprintf(stderr, "counter: <%zu>\n", *counter);

    BiTreeStatus status = 0;

    if (buff[*counter] == '(') {
        if (*out_node == NULL) {
            status = bt_create((bitree_elem_t){}, out_node);
            if (status != BITREE_OK) return status;
        }

        char *end_ptr = BT_DESER_ELEM((buff + *counter) + 1, &(*out_node)->value);
        *counter += (size_t)end_ptr - (size_t)buff - *counter + 1;
    
        // fprintf(stderr, "LEFT recursion:\n");
        status = bt_deserialize_body(buff, counter, &(*out_node)->left);
        if (status != BITREE_OK) return status;

        // fprintf(stderr, "RIGHT recursion:\n");
        status = bt_deserialize_body(buff, counter, &(*out_node)->right);
        if (status != BITREE_OK) return status;

        #ifdef BT_USE_PARENT_FIELD
        if ((*out_node)->left != NULL)  (*out_node)->left->parent  = *out_node;
        if ((*out_node)->right != NULL) (*out_node)->right->parent = *out_node;
        #endif
    } else if (buff[*counter] == '#') {
        (*counter)++;
    } else {
        (*counter)++;
        status = bt_deserialize_body(buff, counter, out_node);
        if (status != BITREE_OK) return status;
    }

    return BITREE_OK;
}


BiTreeStatus bt_deserialize(char *buff, Node **out_node) {
    assert(buff != NULL);
    assert(out_node != NULL);

    size_t counter = 0;
    return bt_deserialize_body(buff, &counter, out_node);
}
