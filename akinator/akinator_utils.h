#pragma once

#include "../include/bitree.h"

typedef struct {
    const char* filename;
} AkiContext;

typedef enum {
    AKI_YES = 0,
    AKI_NO,
    AKI_INVALID
} AkiAnswer;

void aki_greet();
void aki_main_loop();