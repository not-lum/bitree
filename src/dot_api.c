#include "../include/dot_api.h"
#include "../include/bitree.h"

#include "../include/dot_theme.h"

#include <stdio.h>
#include <assert.h>


void start_digraph(FILE *dot_file) {
    assert(dot_file != NULL);

    fputs("digraph {\n", dot_file);
}

void set_default_style(FILE *dot_file) {
    assert(dot_file != NULL);

    fputs("rankdir=TB;\n"
          "node["
          "style=\""     NODE_STYLE      "\", "
          "fontname=\""  NODE_FONTNAME   "\", "
          "shape=\""     NODE_SHAPE      "\", "
          "fillcolor=\"" NODE_FILLCOLOR  "\", "
          "];\n"
          , dot_file);
}

void init_node(FILE *dot_file, ssize_t ind, const char *fillcolor, const char *label) {
    assert(dot_file != NULL);
    
    fprintf(dot_file, "node%zi[", ind); // TODO: replace '%d' in future
    if (fillcolor != NULL) fprintf(dot_file, "fillcolor=\"%s\", ", fillcolor);
    if (label != NULL) fprintf(dot_file, "label=\"%s\", ", label);

    fputs("];\n", dot_file);
}

void init_normal_node(FILE *dot_file, ssize_t ind, tree_elem_t label) {
    assert(dot_file != NULL);

    char label_str[32] = {};
    snprintf(label_str, 32, TREE_ELEM_FMT, label);

    init_node(dot_file, ind, NULL, label_str);
}

void init_empty_node(FILE *dot_file, ssize_t ind) {
    assert(dot_file != NULL);

    init_node(dot_file, ind, EMPTY_NODE_FILLCOLOR, "");
}

void connect_nodes(FILE *dot_file, ssize_t first, ssize_t second) {
    assert(dot_file != NULL);

    fprintf(dot_file, "node%zi->node%zi;\n", first, second);
}

void end_digraph(FILE *dot_file) {
    assert(dot_file != NULL);

    fputs("}\n", dot_file);
}
