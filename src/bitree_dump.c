#include "../include/bitree_dump.h"
#include "../include/bitree.h"
#include "../include/dot_api.h"

#include <assert.h>
#include <stdio.h>

void bitree_dump(Node *root, FILE *logfile, size_t *counter) {
    assert(logfile != NULL);
    assert(counter != NULL);

    if (*counter == 0) {
        start_digraph(logfile);
        set_default_style(logfile);
    }
    
    if (root == NULL) {
        init_empty_node(logfile, *counter);
        return;
    }

    // fprintf(stderr, "root->value: %d\n", root->value);
    init_normal_node(logfile, *counter, root->value);
    size_t root_ind = *counter;
    (*counter)++;

    connect_nodes(logfile, root_ind, *counter);
    bitree_dump(root->left, logfile, counter);
    (*counter)++;

    connect_nodes(logfile, root_ind, *counter);
    bitree_dump(root->right, logfile, counter);
    (*counter)++;

    if (root_ind == 0)
        end_digraph(logfile);
}