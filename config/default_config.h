#pragma once

typedef int bitree_elem_t;

#define BT_DESTROY_ELEM destroy_int
#define BT_SER_ELEM     ser_int
#define BT_DOT_SER_ELEM ser_int
#define BT_DESER_ELEM   deser_int

#include <stdio.h>
#include <assert.h>

static void destroy_int(bitree_elem_t *) {}

static void ser_int(bitree_elem_t value, FILE *file) {
    assert(file != NULL);
    fprintf(file, "%d", value);
}

static char *deser_int(char *buff, bitree_elem_t *value) {
    assert(buff != NULL);
    assert(value != NULL);

    int bytes_read = 0;
    sscanf(buff, "%d%n", value, &bytes_read);
    
    return buff + bytes_read;
}
