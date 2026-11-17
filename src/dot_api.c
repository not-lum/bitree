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

void init_normal_node(FILE *dot_file, const void *ind, bitree_elem_t label) {
    assert(dot_file != NULL);
    
    fprintf(dot_file, "node_%p[label=\"", ind, label);
    BT_DOT_SER_ELEM(label, dot_file);
    fputs("\"];\n", dot_file);
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
