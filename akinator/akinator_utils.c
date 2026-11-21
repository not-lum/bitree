#include "akinator_utils.h"
#include "../include/bitree_dump.h"
#include "../secstack/src/secstack.h"

#include "file_utils.h"

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#define DOT_FILENAME "/tmp/tree_dump.dot"
#define IMG_FILENAME "./b.png"
#define MAX_STR_SIZE 1024
#define MAX_FILENAME_LEN 128

const char* aki_error_str(AkiStatus status) {
    switch (status) {
        case AKI_OK:
            return "akinator ok";
        case AKI_CANNOT_LOAD_FILE:
            return "unable to load file";
        case AKI_CANNOT_OPEN_FILE:
            return "unable to open file";
        case AKI_TREE_DESERIALIZATION_FAILED:
            return "error while deserializing tree";
        case AKI_TREE_SERIALIZATION_FAILED:
            return "error while serializing tree";
        case AKI_NO_SUCH_OBJECT:
            return "unable to find object";
        case AKI_GETLINE_ERROR:
            return "error while getting user input";
        case AKI_TREE_ERROR:
            return "error while performing operations with binary tree";
        case AKI_CANNOT_CLOSE_FILE:
            return "unable to close file";
        default:
            return "unknown error";
    }
}

static AkiStatus aki_load_db(AkiContext *ctx) {
    assert(ctx != NULL);

    LoadedFile l_file = {};

    FileStatus f_status = load_file_alloc(&l_file, ctx->db_filename); 
    if (f_status != FS_OK)
        return AKI_CANNOT_LOAD_FILE;

    BiTreeStatus bt_status = bt_deserialize((char *)l_file.content, &ctx->root);
    if (bt_status != BITREE_OK)
        return AKI_TREE_DESERIALIZATION_FAILED;

    destroy_loaded_file(&l_file);

    return AKI_OK;
}

static AkiStatus aki_save_db(AkiContext *ctx) {
    assert(ctx != NULL);

    FILE *file = fopen(ctx->db_filename, "w");
    if (file == NULL)
        return AKI_CANNOT_OPEN_FILE;

    BiTreeStatus status = bt_serialize(ctx->root, file);
    if (fclose(file) == EOF)
        return AKI_CANNOT_CLOSE_FILE;

    if (status != BITREE_OK)
        return AKI_TREE_SERIALIZATION_FAILED;   
    
    return AKI_OK;
}

static bool read_char(char *out_buff) {
    assert(out_buff != NULL);
    scanf(" %c", out_buff);
    char end_char = getchar();
    if (end_char != '\n')
        return false;

    return true;
}

static AkiAnswer aki_ask(const char *text, ...) {
    assert(text != NULL);

    va_list args = {};
    va_start(args, text);

    AkiAnswer res = AKI_INVALID;
    while (res == AKI_INVALID) {
        va_list va_list_copy = {};
        va_copy(va_list_copy, args);

        vprintf(text, va_list_copy);
        printf("? (y/n) ");
        va_end(va_list_copy);

        char answer = '\0';
        if (!read_char(&answer))
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

static AkiStatus aki_getline(char **out_buff, const char *prompt, ...) {
    assert(prompt != NULL);
    assert(out_buff != NULL);

    va_list args = {};
    va_start(args, prompt);

    vprintf(prompt, args);

    va_end(args);

    size_t size = 0;
    ssize_t len = getline(out_buff, &size, stdin);
    if (len == -1)
        return AKI_GETLINE_ERROR;

    (*out_buff)[len - 1] = '\0';
    
    return AKI_OK;
}

static AkiStatus aki_add_object(Node *parent, BtChildKind ch_kind, AkiContext *ctx) {
    assert(parent != NULL);
    assert(ctx != NULL);

    char *line = NULL;
    AkiStatus status = aki_getline(&line, "Uknown object!\n"
                                          "What is it? ");
    if (status != AKI_OK) return status;

    
    BiTreeStatus bt_status = bt_add(parent, ch_kind, line, NULL);
    if (bt_status != BITREE_OK) return AKI_TREE_ERROR;

    printf("Added '%s'!\n", line);

    return AKI_OK;
}


static AkiStatus aki_present_guess(Node *parent, BtChildKind ch_kind) {
    assert(parent != NULL);

    Node *guessed_obj = bt_get_child(parent, ch_kind);
    char *guessed_obj_name = guessed_obj->value;

    AkiAnswer ans = aki_ask(">>> It's \"%s\"", guessed_obj_name);

    if (ans == AKI_NO) {
        char *new_obj_name = NULL;
        AkiStatus status = aki_getline(&new_obj_name, "Then what is it? ");
        if (status != AKI_OK) return status;

        char *diff_str = NULL;
        status = aki_getline(&diff_str, "What is TRUE for '%s', but FALSE for '%s': ",
                             new_obj_name, guessed_obj_name);
        if (status != AKI_OK) return status;
        
        bt_change(guessed_obj, diff_str);
    
        BiTreeStatus bt_status = bt_add_right(guessed_obj, new_obj_name, NULL);
        if (bt_status != BITREE_OK) return AKI_TREE_ERROR;

        bt_status = bt_add_left(guessed_obj, guessed_obj_name, NULL);
        if (bt_status != BITREE_OK) return AKI_TREE_ERROR;
    }

    return AKI_OK;
}

void aki_greet() {
    fputs("Welcome to Akinator " AKINATOR_VERSION "\n", stdout);
}

static void get_filename(const char *prompt, char *out_filename) {
    assert(prompt != NULL);
    assert(out_filename != NULL);

    fputs(prompt, stdout);
    scanf("%" MAX_FILENAME_LEN_STR "s", out_filename);
    getchar(); // removing last \n
}

static AkiStatus handle_guess_action(AkiContext *ctx) {
    assert(ctx != NULL);

    assert(ctx != NULL);

    Node *curr = ctx->root;
    AkiStatus status = 0;

    while (true) {
        AkiAnswer ans = aki_ask("%s", curr->value);
        Node *next_node = NULL;
        BtChildKind next_node_kind = -1;

        switch (ans) {
            case AKI_YES:
                next_node = curr->right;
                next_node_kind = BT_RIGHT_CHILD;
                break;
            case AKI_NO:
                next_node = curr->left;
                next_node_kind = BT_LEFT_CHILD;
                break;
            case AKI_INVALID:
                fputs("Only y/n allowed. Try again\n", stdout);
                break;
        }

        if ((int)next_node_kind != -1) {
            if (next_node == NULL) {
                status = aki_add_object(curr, next_node_kind, ctx);
                if (status != AKI_OK) return status;

                return aki_save_db(ctx);
            } else if (node_is_leaf(next_node)) {
                status = aki_present_guess(curr, next_node_kind);
                if (status != AKI_OK) return status;

                return aki_save_db(ctx);
            }
            curr = next_node;
        }
    }

    return AKI_OK;
}

static bool find_leaf_cb(Node *node, BtWalkState state, void *ctx) {
    assert(ctx != NULL);

    if (state != BT_LEAVE)
        return false;

    AkiFindContext *find_ctx = (AkiFindContext *)ctx;

    if (node_is_leaf(node) && strcasecmp(node->value, find_ctx->target) == 0) {
        find_ctx->found_node = node;
        return true;
    }

    return false;
}

static Node *find_leaf(Node *node, const char *target) {
    assert(node != NULL);
    assert(target != NULL);

    AkiFindContext find_ctx = { .target = target };
    bt_walk(node, &find_leaf_cb, &find_ctx);

    return find_ctx.found_node;
}

static AkiStatus print_trailing_path(const char *text, stack_t *stack, Node *prev, Node *elem) {
    assert(text != NULL);
    assert(stack != NULL);
    assert(prev != NULL);
    assert(elem != NULL);

    bool first_print = true;
    StackStatus st_status = 0;

    while (!node_is_leaf(prev)) {
        if (st_status != STACK_OK)
            return AKI_STACK_ERROR;

        const char *comma = ", ";

        if (first_print) {
            fputs(text, stdout);
            comma = "";
            first_print = false;
        }

        const char *prefix = (prev->left == elem) ? "NOT " : "";

        printf("%s%s%s", comma, prefix, prev->value);
        
        prev = elem;
        elem = stack_pop(stack, &st_status);
    }

    if (!first_print)
        fputc('\n', stdout);

    return AKI_OK;
}


static AkiStatus print_definition(stack_t *path_stk, const char *obj_name) {
    assert(path_stk != NULL);
    assert(obj_name != NULL);

    StackStatus st_status = 0;

    Node *prev = stack_pop(path_stk, &st_status);
    if (st_status != STACK_OK) return AKI_STACK_ERROR;

    Node *elem = stack_pop(path_stk, &st_status);
    if (st_status != STACK_OK) return AKI_STACK_ERROR;

    return print_trailing_path("--- It's: ", path_stk, prev, elem);
}

static AkiStatus aki_path_to_node(Node *node, stack_t *stack) {
    assert(node != NULL);
    assert(stack != NULL);

    StackStatus status = 0;

    while (node != NULL) {
        stack_push(stack, node, &status);
        if (status != STACK_OK)
            return AKI_STACK_ERROR;

        node = node->parent;
    }

    return AKI_OK;
}

static AkiStatus handle_definition_action(AkiContext *ctx) {
    assert(ctx != NULL);

    char *obj_name = NULL;
    stack_t path_stk = {};

    AkiStatus aki_status = aki_getline(&obj_name, "What's the object you searching for? ");
    if (aki_status != AKI_OK) goto cleanup;

    Node *found_obj = find_leaf(ctx->root, obj_name);
    if (found_obj == NULL) {
        aki_status = AKI_NO_SUCH_OBJECT;
        goto cleanup;
    }

    StackStatus st_status = STACK_INIT_DBG(&path_stk, 16);
    if (st_status != STACK_OK) { aki_status = AKI_STACK_ERROR; goto cleanup; }

    aki_status = aki_path_to_node(found_obj, &path_stk);
    if (aki_status != AKI_OK) goto cleanup;

    aki_status = print_definition(&path_stk, found_obj->value);

cleanup:
    free(obj_name);
    stack_destroy(&path_stk);

    return aki_status;
}

static AkiStatus aki_prompt_path_to_obj(const char *prompt, AkiContext *ctx,
                                        stack_t *stack) {
    assert(ctx != NULL);
    assert(prompt != NULL);
    assert(stack != NULL);

    char *obj_name = NULL;
    AkiStatus status = aki_getline(&obj_name, "%s", prompt);
    if (status != AKI_OK) goto cleanup;

    Node *found_obj = find_leaf(ctx->root, obj_name);
    if (found_obj == NULL) {
        status = AKI_NO_SUCH_OBJECT;
        goto cleanup;
    }

    status = aki_path_to_node(found_obj, stack);

cleanup:
    free(obj_name);

    return status;
}

static void print_similarity(const char *similarity, bool is_correct) {
    assert(similarity != NULL);

    if (is_correct) {
        printf("%s", similarity);
    } else {
        printf("NOT %s", similarity);
    } 
}

static AkiStatus pop_from_two_stacks(stack_t *stack1, stack_t *stack2,
                                     Node **out_node1, Node **out_node2) {
    assert(stack1 != NULL);
    assert(stack2 != NULL);
    assert(out_node1 != NULL);
    assert(out_node2 != NULL);

    StackStatus status = 0;
    *out_node1 = stack_pop(stack1, &status);
    if (status != STACK_OK) return AKI_STACK_ERROR;

    *out_node2 = stack_pop(stack2, &status);
    if (status != STACK_OK) return AKI_STACK_ERROR;

    return AKI_OK;
}

static AkiStatus print_comparison(stack_t *stack1, stack_t *stack2) {
    assert(stack1 != NULL);
    assert(stack2 != NULL);

    fputc('\n', stdout);

    Node *prev1 = NULL, *prev2 = NULL;
    AkiStatus status = pop_from_two_stacks(stack1, stack2, &prev1, &prev2);
    if (status != AKI_OK) return status;

    Node *elem1 = NULL, *elem2 = NULL;
    bool banner_printed = false;

    while (!node_is_leaf(prev1) && !node_is_leaf(prev2)) {
        status = pop_from_two_stacks(stack1, stack2, &elem1, &elem2);
        if (status != AKI_OK) return status;

        bool went_right1 = prev1->right == elem1;
        bool went_right2 = prev2->right == elem2;

        if (went_right1 != went_right2)
            break;    
    
        if (!banner_printed) {
            fputs("--- They both are: ", stdout);
            banner_printed = true;
        } else
            fputs(", ", stdout);

        print_similarity(prev1->value, went_right1);

        prev1 = elem1;
        prev2 = elem2;
    }

    if (banner_printed) {
        fputs("\n", stdout);
        if (prev1 != prev2)
            fputs("--- BUT: ", stdout);
    }

    status = print_trailing_path("--- First object is: ", stack1, prev1, elem1);
    if (status != AKI_OK) return status;

    return print_trailing_path("--- Second object is: ", stack2, prev2, elem2);
}


static AkiStatus handle_compare_action(AkiContext *ctx) {
    assert(ctx != NULL);

    stack_t stack1 = {}, stack2 = {};
    AkiStatus aki_status = AKI_OK;
    if (aki_status != AKI_OK) goto cleanup;

    StackStatus st_status = STACK_INIT_DBG(&stack1, 16);
    if (st_status != STACK_OK) { aki_status = AKI_STACK_ERROR; goto cleanup; }

    st_status = STACK_INIT_DBG(&stack2, 16);
    if (st_status != STACK_OK) { aki_status = AKI_STACK_ERROR; goto cleanup; }

    aki_status = aki_prompt_path_to_obj("First object name: ", ctx, &stack1);
    if (aki_status != AKI_OK) goto cleanup;

    aki_status = aki_prompt_path_to_obj("Second object name: ", ctx, &stack2);
    if (aki_status != AKI_OK) goto cleanup;

    aki_status = print_comparison(&stack1, &stack2);

cleanup:
    stack_destroy(&stack1);
    stack_destroy(&stack2);

    return aki_status;
} 

static AkiStatus handle_visualize_action(AkiContext *ctx) {
    assert(ctx != NULL);

    char img_filename[MAX_FILENAME_LEN] = {};
    get_filename("Enter image filename: ", img_filename);

    FILE *dot_file = fopen(DOT_FILENAME, "w");
    if (dot_file == NULL)
        return AKI_CANNOT_OPEN_FILE;

    bt_dump_to_dot(ctx->root, dot_file);

    fclose(dot_file);

    compile_dot(DOT_FILENAME, img_filename);

    printf("Visualized database '%s' in '%s'.\n", ctx->db_filename, img_filename);

    return AKI_OK;
}


void aki_start_game(AkiContext *ctx) {
    AkiStatus status = 0;
    aki_greet();
    get_filename("Enter database filename: ", ctx->db_filename);
    status = aki_load_db(ctx);

    if (status != AKI_OK) {
        fprintf(stderr, "Error: %s\n", aki_error_str(status));
        return;
    }

    while (true) {
        fputs("\nSelect action:\n"
              "1. Guess object\n"
              "2. Get object definition\n"
              "3. Compare objects\n"
              "4. Visualize database\n"
              "5. Quit\n"
              "> ", stdout);

        char choice = '\0';
        if (!read_char(&choice))
            return;

        switch (choice) {
            case '1':
                status = handle_guess_action(ctx);
                if (status != AKI_OK) fprintf(stderr, "Error: %s\n", aki_error_str(status));
                break;
            case '2':
                status = handle_definition_action(ctx);
                if (status != AKI_OK) fprintf(stderr, "Error: %s\n", aki_error_str(status));
                break;
            case '3':
                status = handle_compare_action(ctx);
                if (status != AKI_OK) fprintf(stderr, "Error: %s\n", aki_error_str(status));
                break;
            case '4':
                status = handle_visualize_action(ctx);
                if (status != AKI_OK) fprintf(stderr, "Error: %s\n", aki_error_str(status));
                break;
            case '5':
                bt_destroy(ctx->root);
                return;
            default:
                printf("Unknown action. Try again\n");
        }
    }
}

