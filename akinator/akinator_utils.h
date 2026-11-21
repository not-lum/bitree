#pragma once

#include "../include/bitree.h"

#define AKINATOR_VERSION "v1.1"
#define MAX_FILENAME_LEN 128
#define MAX_FILENAME_LEN_STR "127"

typedef struct {
    char db_filename[MAX_FILENAME_LEN];
    Node *root;
} AkiContext;

typedef struct {
    const char *target;
    Node *found_node;
} AkiFindContext;

typedef enum {
    AKI_YES = 0,
    AKI_NO,
    AKI_INVALID
} AkiAnswer;


typedef enum {
    AKI_OK = 0,
    AKI_CANNOT_LOAD_FILE,
    AKI_CANNOT_OPEN_FILE,
    AKI_TREE_DESERIALIZATION_FAILED,
    AKI_TREE_SERIALIZATION_FAILED,
    AKI_STACK_ERROR,
    AKI_NO_SUCH_OBJECT,
    AKI_GETLINE_ERROR,
    AKI_TREE_ERROR,
    AKI_CANNOT_CLOSE_FILE
} AkiStatus;

const char* aki_error_str(AkiStatus status);
void aki_greet();
AkiStatus aki_prompt_load_db(AkiContext *ctx);
void aki_start_game(AkiContext *ctx);