#pragma once

#include "../include/bitree.h"
#include <stdio.h>


typedef struct {
    const char *fillcolor;
    const char *label;
} NodeStyle;

void start_digraph(FILE *dot_file);
void set_default_style(FILE *dot_file);
void init_node(FILE *dot_file, const void *ind, const char *style, const char *label);
void init_normal_node(FILE *dot_file, const void *ind, bitree_elem_t label);
void init_empty_node(FILE *dot_file, const void *ind);
void connect_nodes(FILE *dot_file, const void *first, const void *second, const char *);
void connect_placeholder(FILE *dot_file, const void *first, const char *tag);
void end_digraph(FILE *dot_file);