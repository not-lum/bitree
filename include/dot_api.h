#pragma once

#include "../include/bitree.h"
#include <stdio.h>


typedef struct {
    const char *fillcolor;
    const char *label;
} NodeStyle;

void start_digraph(FILE *dot_file);
void set_default_style(FILE *dot_file);
void init_node(FILE *dot_file, ssize_t ind, const char *fillcolor, const char *label);
void init_normal_node(FILE *dot_file, ssize_t ind, tree_elem_t label);
void init_empty_node(FILE *dot_file, ssize_t ind);
void connect_nodes(FILE *dot_file, ssize_t first, ssize_t second);
void end_digraph(FILE *dot_file);

