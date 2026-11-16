#include "../include/bitree.h"
#include "../include/bitree_dump.h"

#include <assert.h>
#include <stdbool.h>

#define DOT_FILENAME "/tmp/tree_dump.dot"
#define IMG_FILENAME "./a.png"
#define MAX_TRAVELS 1000

BiTreeStatus place_node(Node *, int);
BiTreeStatus tree_sort(int [], size_t, Node **);
void inorder_walk(Node *, int [], size_t *);

int main() {
    int arr1[] = {8, 4, 4, 4, 4, 5, 432, 321, 3334, 55, -1, 8, 9};
    const size_t arr1_len = sizeof(arr1)/sizeof(arr1[0]);

    Node *root = NULL;
    BiTreeStatus status = tree_sort(arr1, arr1_len, &root);
    if (status != BITREE_OK)
        fprintf(stderr, "Error while sorting tree: %s\n", bt_error_str(status));

    for (size_t i = 0; i < arr1_len; i++)
        printf("%d ", arr1[i]);
    printf("\n");

    FILE *logfile = fopen(DOT_FILENAME, "w");

    bt_dump_to_dot(root, logfile);

    fclose(logfile);

    char cmd[MAX_CMD_LEN] = {};
    form_cmd(cmd, DOT_FILENAME, IMG_FILENAME);
    system(cmd);

    bt_destroy(root, NULL);
}

BiTreeStatus tree_sort(int arr[], size_t len, Node **out_root) {
    assert(arr != NULL);
    assert(out_root != NULL);
    assert(len > 0);

    Node *root = NULL;
    BiTreeStatus status = bt_create(arr[0], &root);
    if (status != BITREE_OK)
        return status;

    *out_root = root;

    for (size_t i = 1; i < len; i++) {
        status = place_node(root, arr[i]);
        if (status != BITREE_OK)
            return status;
    }

    size_t counter = 0;
    inorder_walk(root, arr, &counter);

    return BITREE_OK;
}

BiTreeStatus place_node(Node *node, int val) {
    assert(node != NULL);

    for (size_t i = 0; i < MAX_TRAVELS; i++) { // TODO: wanna rename i to _
        if (node->value >= val) {
            if (node->left == NULL) {
                BiTreeStatus status = bt_add_left(node, val, NULL);
                return status;
            }

            node = node->left;
        } else {
            if (node->right == NULL) {
                BiTreeStatus status = bt_add_right(node, val, NULL);
                return status;
            }

            node = node->right;
        }
    }

    return BITREE_ERROR;
} 

void inorder_walk(Node *node, int arr[], size_t *counter) {
    assert(node != NULL);

    if (node->left != NULL)
        inorder_walk(node->left, arr, counter);
    
    arr[*counter] = node->value;
    (*counter)++;

    if (node->right != NULL)
        inorder_walk(node->right, arr, counter);
}
