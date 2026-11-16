#include "../include/bitree.h"
#include "../include/bitree_dump.h"

#include <assert.h>
#include <string.h>

#define DOT_FILENAME "/tmp/tree_dump.dot"
#define IMG_FILENAME "./b.png"

#define MAX_STR_SIZE 1024

static BtChildKind switch_child_kind(BtChildKind ch_kind) {
    if (ch_kind == BT_LEFT_CHILD)
        return BT_RIGHT_CHILD;
    else
        return BT_LEFT_CHILD;
}

BiTreeStatus bt_from_dump(char *buff, size_t *counter,
                  char *(*deser)(char *, bitree_elem_t *) ,
                  Node *out_node) {
    assert(buff != NULL);
    assert(deser != NULL);
    assert(out_node != NULL);
    
    fprintf(stderr, "buff: <%s>\n", buff + *counter);
    fprintf(stderr, "counter: <%zu>\n", *counter);

    BiTreeStatus status = 0;

    if (*(buff + *counter) == '(') {
        // if (out_node == NULL)
        //     bt_create((bitree_elem_t){}, &out_node);

        char *end_ptr = deser((buff + *counter) + 1, &out_node->value);
        *counter += (size_t)end_ptr - (size_t)buff - *counter + 1;
    
        fprintf(stderr, "LEFT recursion:\n");
        bt_add_left(out_node, (bitree_elem_t){}, NULL);
        bt_from_dump(buff, counter, deser, out_node->left);

        fprintf(stderr, "RIGHT recursion:\n");
        bt_add_right(out_node, (bitree_elem_t){}, NULL);
        bt_from_dump(buff, counter, deser, out_node->right);
    } else if (*(buff + *counter) == '#') {
        (*counter)++;
    } else {
        (*counter)++;
        bt_from_dump(buff, counter, deser, out_node);
    }
}


void destroyer(bitree_elem_t *value_ptr) {
    assert(value_ptr != NULL);
    fprintf(stderr, ">>>>> node->value: " BITREE_ELEM_FMT "\n", *value_ptr);

    free(*value_ptr);
}

char *deser_str(char *buff, bitree_elem_t *out_elem) {
    assert(buff != NULL);
    assert(out_elem != NULL);
    fprintf(stderr, "DESER_STR called.\n");
    fprintf(stderr, "[0]: %c [%p]\n", *buff, buff);

    buff++;
    size_t counter = 0;

    while (*(buff + counter) != '"') {
        fprintf(stderr, "next: %c\n", *buff);
        counter++;
    }

    // TODO: use String structure instead of calloc everytime.
    // we just write the length of string and its ptr
    // but we will have to write the serializer too
    *out_elem = calloc(1, counter + 1);
    memcpy(*out_elem, buff, counter);
    (*out_elem)[counter] = '\0';

    return buff;
}

int main() {
    Node *root = NULL;
    bt_create((bitree_elem_t){}, &root);
    // FILE *txt_file = fopen("dump.txt", "r");

    char buff[] = "(\"root\"(\"left\"(\"left of left\"#(\"right of left of left\"##))#)(\"abracadabra\"#(\"bibobabobus\"#(\"fffff\"#(\"aaaaa aa\"##)))))";
    // char buff[] = "(\"a\"(\"b\"##)(\"c\"##))";
    size_t counter = 0;
    bt_from_dump(buff, &counter, deser_str, root);

    fprintf(stderr, "=== END ===\n");
    // fclose(txt_file);

    FILE *dot_file = fopen(DOT_FILENAME, "w");
    bt_dump_to_dot(root, dot_file);
    fclose(dot_file);

    compile_dot(DOT_FILENAME, IMG_FILENAME);

    bt_destroy(root, destroyer);
}