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

BiTreeStatus compile_dot(const char* inp_filename, const char *out_filename) {
    assert(inp_filename != NULL);
    assert(out_filename != NULL);

    char cmd[MAX_CMD_LEN] = {};
    if (form_cmd(cmd, inp_filename, out_filename)) {
        system(cmd);
        return BITREE_OK;
    } else
        return BITREE_UNABLE_TO_FORM_CMD;
}

static bool bt_preorder_print(Node *node, BtWalkState state, void *ctx) {
    assert(ctx != NULL);

    FILE *file = (FILE *)ctx;

    switch (state) {
        case BT_ENTER:
            fputs("(", file);
            BT_SER_ELEM(node->value, file);
            break;
        case BT_IN:
            break;
        case BT_LEAVE:
            fputs(")", file);
            break;
        case BT_EMPTY:
            fputs("#", file);
            break;
    }

    return false;
}

static bool bt_inorder_print(Node *node, BtWalkState state, void *ctx) {
    assert(ctx != NULL);

    FILE *file = (FILE *)ctx;

    switch (state) {
        case BT_ENTER:
            fputs("(", file);
            break;
        case BT_IN:
            BT_SER_ELEM(node->value, file);
            break;
        case BT_LEAVE:
            fputs(")", file);
            break;
        case BT_EMPTY:
            fputs("#", file);
            break;
    }

    return false;
}

static bool bt_postorder_print(Node *node, BtWalkState state, void *ctx) {
    assert(ctx != NULL);

    FILE *file = (FILE *)ctx;

    switch (state) {
        case BT_ENTER:
            fputs("(", file);
            break;
        case BT_IN:
            break;
        case BT_LEAVE:
            BT_SER_ELEM(node->value, file);
            fputs(")", file);
            break;
        case BT_EMPTY:
            fputs("#", file);
            break;
    }

    return false;
}

static bool bt_dot_dump_node(Node *node, BtWalkState state, void *ctx) {
    assert(ctx != NULL);

    if (state != BT_ENTER)
        return false;

    FILE *file = ctx;

    init_normal_node(file, node, node->value);

    if (node->left != NULL)
        connect_nodes(file, node, node->left, NULL);
    else
        connect_placeholder(file, node, "L");

    if (node->right != NULL)
        connect_nodes(file, node, node->right, NULL);
    else
        connect_placeholder(file, node, "R");

    return false;
}

void bt_print(Node *root, WalkKind kind, FILE *ctx) {
    assert(root != NULL);
    assert(ctx != NULL);

    switch (kind) {
        case BT_PREORDER:
            bt_walk(root, &bt_preorder_print, ctx);
            break;
        case BT_INORDER:
            bt_walk(root, &bt_inorder_print, ctx);
            break;
        case BT_POSTORDER:
            bt_walk(root, &bt_postorder_print, ctx);
            break;
    }
}

void bt_dump_to_dot(Node *root, FILE *dot_file) {
    assert(root != NULL);
    assert(dot_file != NULL);

    start_digraph(dot_file);
    set_default_style(dot_file);

    bt_walk(root, &bt_dot_dump_node, dot_file);

    end_digraph(dot_file);
}
