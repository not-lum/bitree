#include "../include/dot_api.h"

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

void init_node(FILE *dot_file, const void *ind, const char *style, const char *label) {
    assert(dot_file != NULL);
    
    fprintf(dot_file, "node_%p[", ind);
    if (style != NULL) fprintf(dot_file, "style=\"%s\", ", style);
    if (label != NULL) fprintf(dot_file, "label=\"%s\", ", label);

    fputs("];\n", dot_file);
}

void init_normal_node(FILE *dot_file, const void *ind, bitree_elem_t label) {
    assert(dot_file != NULL);

    #ifndef BITREE_ELEM_IS_STR
        char label_str[32] = {};
        snprintf(label_str, 32, BITREE_ELEM_FMT, label);

        init_node(dot_file, ind, NULL, label_str);
    #else
        init_node(dot_file, ind, NULL, label);
    #endif
}

void init_empty_node(FILE *dot_file, const void *ind) {
    assert(dot_file != NULL);
    assert(ind != NULL);

    init_node(dot_file, ind, "invis", "");
}

void connect_nodes(FILE *dot_file, const void *first, const void *second, const char *style) {
    assert(dot_file != NULL);
    assert(first != NULL);
    assert(second != NULL);

    fprintf(dot_file, "node_%p->node_%p", first, second);
    if (style != NULL) fprintf(dot_file, "[style=\"%s\"]", style);
    fputs(";\n", dot_file);
}

void connect_placeholder(FILE *dot_file, const void *first, const char *tag) {
    assert(dot_file != NULL);
    assert(first != NULL);
    assert(tag != NULL);

    fprintf(dot_file,
            "empty_%p_%s[style=invis, label=\"\"];\n"
            "node_%p->empty_%p_%s[style=\"invis\"];\n",
            first, tag, first, first, tag);
}

void end_digraph(FILE *dot_file) {
    assert(dot_file != NULL);

    fputs("}\n", dot_file);
}
