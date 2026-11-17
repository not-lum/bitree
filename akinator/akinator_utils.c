#include "akinator_utils.h"
#include "../include/bitree_dump.h"
#include "file_utils.h"

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#define DOT_FILENAME "/tmp/tree_dump.dot"
#define IMG_FILENAME "./b.png"
#define MAX_STR_SIZE 1024
#define MAX_FILENAME_LEN 128

static void read_bd(AkiContext *ctx) {
    assert(ctx != NULL);

    LoadedFile l_file = {};
    // if we alloc every node, we don't need this. TODO: remove
    FileStatus f_status = load_file_alloc(&l_file, ctx->db_filename); 
    if (f_status != FS_OK)
        return;

    Node *node = NULL;
    BiTreeStatus bt_status = bt_deserialize((char *)l_file.content, &ctx->root);
    if (bt_status != BITREE_OK)
        return;

    destroy_loaded_file(&l_file);
}

static void save_bd(AkiContext *ctx) {
    assert(ctx != NULL);

    FILE *file = fopen(ctx->db_filename, "w");
    bt_serialize(ctx->root, file);
    fclose(file);
}

static bool parse_char(char *out_buff) {
    assert(out_buff != NULL);
    scanf(" %c", out_buff);

    char end_char = getchar();
    if (end_char != '\n')
        return false;

    return true;
}

static AkiAnswer ask(const char *text, ...) {
    assert(text != NULL);

    va_list args = {};
    va_start(args, text);

    AkiAnswer res = AKI_INVALID;
    while (res == AKI_INVALID) {
        vprintf(text, args);
        printf("? (y/n)\n");

        char answer = '\0';
        if (!parse_char(&answer))
            continue;

        if (answer == 'y' || answer == 'Y') {
            res = AKI_YES;
        } else if (answer == 'n' || answer == 'N') {
            res = AKI_NO;
        }
    }

    va_end(args);
    return res;
}

static char *aki_getline(const char *prompt, ...) {
    assert(prompt != NULL);

    va_list args = {};
    va_start(args, prompt);

    vprintf(prompt, args);

    va_end(args);

    char *line = NULL;
    size_t size = 0;
    ssize_t len = getline(&line, &size, stdin);
    line[len - 1] = '\0';
    
    return line;
}

static void add_object(Node *parent, BtChildKind ch_kind, AkiContext *ctx) {
    assert(parent != NULL);

    char *line = aki_getline("Uknown object!\n"
                             "What is it? ");
    
    bt_add(parent, ch_kind, line, NULL);

    printf("Added '%s'!\n", line);
}

static BtChildKind switch_child_kind(BtChildKind kind) {
    return kind ^ 1;
}

static void present_guess(Node *parent, BtChildKind ch_kind) {
    assert(parent != NULL);

    Node *guessed_obj = bt_get_child(parent, ch_kind);
    char *guessed_obj_name = guessed_obj->value;

    AkiAnswer ans = ask("I guessed it! It's \"%s\", right", guessed_obj_name);
    if (ans == AKI_NO) {
        char *new_obj_name = aki_getline("Then what is it? ");
        char *diff_str = aki_getline("Explain what makes '%s' diffrent from '%s': ",
                                      new_obj_name, guessed_obj_name);

        bt_change(guessed_obj, diff_str);
    
        bt_add(guessed_obj, ch_kind, new_obj_name, NULL);
        bt_add(guessed_obj, switch_child_kind(ch_kind), guessed_obj_name, NULL);
    }
}

static void guess_loop(AkiContext *ctx) {
    assert(ctx != NULL);

    Node *curr = ctx->root;

    while (true) {
        AkiAnswer ans = ask(curr->value);

        switch (ans) { // TODO: think about copypaste here
            case AKI_YES:
                if (curr->right == NULL) {
                    add_object(curr, BT_RIGHT_CHILD, ctx);
                    save_bd(ctx);
                    return;
                } else if (node_is_leaf(curr->right)) {
                    present_guess(curr, BT_RIGHT_CHILD);
                    save_bd(ctx);
                    return;
                }
                curr = curr->right;
                break;
            case AKI_NO:
                if (curr->left == NULL) {
                    add_object(curr, BT_LEFT_CHILD, ctx);
                    save_bd(ctx);
                    return;
                } else if (node_is_leaf(curr->left)) {
                    present_guess(curr, BT_LEFT_CHILD);
                    save_bd(ctx);
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

static void handle_guess_action(AkiContext *ctx) {
    assert(ctx != NULL);

    fputs("Enter database filename: ", stdout);
    scanf("%s", ctx->db_filename);

    read_bd(ctx);
    guess_loop(ctx); 

    FILE *dot_file = fopen(DOT_FILENAME, "w");
    bt_dump_to_dot(ctx->root, dot_file);
    fclose(dot_file);

    compile_dot(DOT_FILENAME, IMG_FILENAME);

    bt_destroy(ctx->root);
    ctx->root = NULL;
}


void aki_main_loop(AkiContext *ctx) {
    while (true) {
        fputs("\nSelect action:\n"
              "1. Play akinator\n"
              "2. Quit\n", stdout);

        char choice = '\0';
        if (!parse_char(&choice))
            return;

        switch (choice) {
            case '1':
                handle_guess_action(ctx);
                break;
            case '2':
                return;
            default:
                printf("Unknown action. Try again\n");
        }
    }
}

