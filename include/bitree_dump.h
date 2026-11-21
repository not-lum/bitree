#pragma once

#include "../include/bitree.h"

#include <stdio.h>
#include <stdbool.h>

#define MAX_CMD_LEN 100

typedef enum {
    BT_PREORDER = 0,
    BT_INORDER,
    BT_POSTORDER
} WalkKind;

BiTreeStatus compile_dot(const char* inp_filename, const char *out_filename);
void bt_print(Node *root, WalkKind kind, FILE *ctx);
bool form_cmd(char cmd[MAX_CMD_LEN],
              const char *inp_filename,
              const char *out_filename);
void bt_dump_to_dot(Node *root, FILE *ctx);