#pragma once

#include "../include/bitree.h"

#define MAX_FILENAME_LEN 128
#define MAX_FILENAME_LEN_STR "128"

typedef struct {
    char db_filename[MAX_FILENAME_LEN];
    Node *root;
} AkiContext;

typedef struct {
    char *str;
    Node *node;
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
    AKI_TREE_SERIALIZATION_FAILED
} AkiStatus;

void aki_greet();
void aki_main_loop();