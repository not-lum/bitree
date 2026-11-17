#include "akinator_config.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void destroy_str(bitree_elem_t *value_ptr) {
    assert(value_ptr != NULL);
    // fprintf(stderr, ">>>>> node->value: " BITREE_ELEM_FMT "\n", *value_ptr);

    free(*value_ptr);
}

void ser_str(bitree_elem_t value, FILE *out_file) {
    assert(out_file != NULL);
    
    fprintf(out_file, "\"%s\"", value);
}

char *deser_str(char *buff, bitree_elem_t *out_elem) {
    assert(buff != NULL);
    assert(out_elem != NULL);
    fprintf(stderr, "DESER_STR called.\n");
    fprintf(stderr, "[0]: %c [%p]\n", *buff, buff);

    buff++;
    size_t counter = 0;

    while (*(buff + counter) != '"') {
        fprintf(stderr, "next: %c\n", *(buff + counter));
        counter++;
    }

    // TODO: use String structure instead of calloc everytime.
    // we just write the length of string and its ptr
    // but we will have to write the serializer too
    *out_elem = calloc(1, counter + 1);
    memcpy(*out_elem, buff, counter);
    (*out_elem)[counter] = '\0';

    return (buff + counter);
}
