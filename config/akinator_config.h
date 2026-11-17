#pragma once

#include <stdio.h>

#define BT_USE_PARENT_FIELD
typedef char* bitree_elem_t;
#define BITREE_ELEM_FMT "\"%s\""

#define BT_DESTROY_ELEM destroy_str
#define BT_SER_ELEM     ser_str
#define BT_DOT_SER_ELEM ser_dot_str
#define BT_DESER_ELEM   deser_str

void destroy_str(bitree_elem_t *value_ptr);
void ser_str(bitree_elem_t, FILE *out_file);
void ser_dot_str(bitree_elem_t value, FILE *out_file);
char *deser_str(char *buff, bitree_elem_t *out_elem);