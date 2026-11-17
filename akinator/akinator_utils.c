#include "akinator_utils.h"
#include "../include/bitree_dump.h"
#include "file_utils.h"

#include <assert.h>
#include <string.h>
#include <stdbool.h>


#define DOT_FILENAME "/tmp/tree_dump.dot"
#define IMG_FILENAME "./b.png"
#define MAX_STR_SIZE 1024
#define MAX_FILENAME_LEN 128


static void read_bd(const char *filename, Node **out_node) {
    assert(filename != NULL);
    assert(out_node != NULL);

    LoadedFile l_file = {};
    FileStatus f_status = load_file_alloc(&l_file, filename);
    if (f_status != FS_OK)
        return;

    Node *node = NULL;
    BiTreeStatus bt_status = bt_deserialize((char *)l_file.content, out_node);
    if (bt_status != BITREE_OK)
        return;

    destroy_loaded_file(&l_file);
}

static bool parse_char(char *out_buff) {
    assert(out_buff != NULL);
    scanf(" %c", out_buff);

    char end_char = getchar();
    if (end_char != '\n')
        return false;

    return true;
}

static AkiAnswer ask(const char *text) {
    assert(text != NULL);

    printf("> %s? (y/n)\n", text);
    char answer = '\0';
    if (!parse_char(&answer))
        return AKI_INVALID;

    if (answer == 'y' || answer == 'Y') {
        return AKI_YES;
    } else if (answer == 'n' || answer == 'N') {
        return AKI_NO;
    }

    return AKI_INVALID;
}

static void add_object(Node *parent, BtChildKind ch_kind) {
    assert(parent != NULL);

    fputs("Unknown object!\n"
          "What is it? ", stdout);
            
    char *line = calloc(MAX_STR_SIZE, sizeof(char));
    ssize_t read = scanf(" %s", line);
    
    bt_add(parent, ch_kind, line, NULL);

    printf("Added '%s'!\n", line);

    AkiAnswer ans = ask("Do you wanna save the updated tree version");
    if (ans == AKI_YES) {
        
    }
}

static void guess_loop(Node *curr) {
    assert(curr != NULL);

    while (true) {
        if (node_is_leaf(curr)) {
            printf("I guessed it! It's \"%s\"!\n", curr->value);
            break;
        }

        AkiAnswer ans = ask(curr->value);

        switch (ans) {
            case AKI_YES:
                if (curr->right == NULL) {
                    add_object(curr, BT_RIGHT_CHILD);
                    return;
                }
                curr = curr->right;
                break;
            case AKI_NO:
                if (curr->left == NULL) {
                    add_object(curr, BT_LEFT_CHILD);
                    return;
                }
                curr = curr->left;
                break;
            case AKI_INVALID:
                fputs("Only y/n allowed. Try again\n", stdout);
                break;
        }
    }
}

void aki_greet() {
    fputs("Welcome to Akinator v.0.1\n", stdout);
}

static void handle_guess_action() {
    fputs("Enter database filename:\n", stdout);

    char filename[MAX_FILENAME_LEN] = {};
    scanf("%s", filename);

    Node *root = NULL;
    read_bd(filename, &root);

    Node *curr = root;
    guess_loop(root); 

    FILE *dot_file = fopen(DOT_FILENAME, "w");
    bt_dump_to_dot(root, dot_file);
    fclose(dot_file);

    compile_dot(DOT_FILENAME, IMG_FILENAME);

    bt_destroy(root);
}


void aki_main_loop() {
    while (true) {
        fputs("Select action:\n"
              "1. Play akinator\n"
              "2. Quit\n", stdout);

        char choice = '\0';
        if (!parse_char(&choice))
            return;

        switch (choice) {
            case '1':
                handle_guess_action();
                break;
            case '2':
                return;
            default:
                printf("Unknown action. Try again\n");
        }
    }
}

