#include "../include/bitree.h"

#include <stdio.h>
#include <stdbool.h>

#define MAX_CMD_LEN 100

typedef enum {
    BT_PREORDER = 0,
    BT_INORDER,
    BT_POSTORDER
} WalkKind;

void bt_dump_to_str(Node *root, WalkKind kind, char out_buff[], size_t *);
bool form_cmd(char cmd[MAX_CMD_LEN],
    const char *inp_filename,
    const char *out_filename);
void bt_dump_to_dot(Node *root, FILE *logfile, size_t *counter);