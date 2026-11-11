#include "../include/bitree_dump.h"
#include "../include/bitree.h"
#include "../include/dot_api.h"

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

bool form_cmd(char cmd[MAX_CMD_LEN],
              const char *inp_filename,
              const char *out_filename) {
    assert(cmd != NULL);
    assert(inp_filename != NULL);
    assert(out_filename != NULL);

    int len = snprintf(cmd, MAX_CMD_LEN, "dot %s -Tpng -o %s",
                       inp_filename, out_filename);

    if (len < 0 || len >= MAX_CMD_LEN) {
        cmd[0] = '\0';

        return false;
    }

    return true;
}

void bt_dump_to_str(Node *root, WalkKind kind, char out_buff[], size_t *curr_len) {
    assert(root != NULL);
    assert(out_buff != NULL);
    // printf("out_buff from dump: <%s>[%p]\n", out_buff, out_buff);

    *curr_len += sprintf(out_buff + *curr_len, "(");
    *curr_len += sprintf(out_buff + *curr_len, BITREE_ELEM_FMT, root->value);

    if (root->left != NULL)
        bt_dump_to_str(root->left, kind, out_buff, curr_len);    
    
    if (root->right != NULL)
        bt_dump_to_str(root->right, kind, out_buff, curr_len);

    *curr_len += sprintf(out_buff + *curr_len, ")");
}

void bt_dump_to_dot(Node *root, FILE *dot_file, size_t *counter) {
    assert(dot_file != NULL);
    assert(counter != NULL);

    if (*counter == 0) {
        start_digraph(dot_file);
        set_default_style(dot_file);
    }
    
    if (root == NULL) {
        init_empty_node(dot_file, *counter);
        return;
    }

    init_normal_node(dot_file, *counter, root->value);
    size_t root_ind = *counter;
    (*counter)++;

    if (root->left == NULL)
        connect_nodes(dot_file, root_ind, *counter, "invis");
    else
        connect_nodes(dot_file, root_ind, *counter, NULL);

    bt_dump_to_dot(root->left, dot_file, counter);
    (*counter)++;

    if (root->right == NULL)
        connect_nodes(dot_file, root_ind, *counter, "invis");
    else
        connect_nodes(dot_file, root_ind, *counter, NULL);

    bt_dump_to_dot(root->right, dot_file, counter);
    (*counter)++;

    if (root_ind == 0)
        end_digraph(dot_file);
}
