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
        default:
            return "unknown error";
    }
}

static AkiStatus load_db(AkiContext *ctx) {
    assert(ctx != NULL);

    LoadedFile l_file = {};
    // if we alloc every node, we don't need this. TODO: remove
    FileStatus f_status = load_file_alloc(&l_file, ctx->db_filename); 
    if (f_status != FS_OK)
        return AKI_CANNOT_LOAD_FILE;

    BiTreeStatus bt_status = bt_deserialize((char *)l_file.content, &ctx->root);
    if (bt_status != BITREE_OK)
        return AKI_TREE_DESERIALIZATION_FAILED;

    destroy_loaded_file(&l_file);

    return AKI_OK;
}

static AkiStatus save_bd(AkiContext *ctx) {
    assert(ctx != NULL);

    FILE *file = fopen(ctx->db_filename, "w");
    if (file == NULL)
        return AKI_CANNOT_OPEN_FILE;

    BiTreeStatus status = bt_serialize(ctx->root, file);
    fclose(file);

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

static AkiAnswer ask(const char *text, ...) {
    assert(text != NULL);

    va_list args = {};
    va_start(args, text);

    AkiAnswer res = AKI_INVALID;
    while (res == AKI_INVALID) {
        va_list va_list_copy = {};
        va_copy(va_list_copy, args);

        vprintf(text, va_list_copy);
        printf("? (y/n) ");

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
    assert(ctx != NULL);

    char *line = aki_getline("Uknown object!\n"
                             "What is it? ");
    
    bt_add(parent, ch_kind, line, NULL);

    printf("Added '%s'!\n", line);
}


static void present_guess(Node *parent, BtChildKind ch_kind) {
    assert(parent != NULL);

    Node *guessed_obj = bt_get_child(parent, ch_kind);
    char *guessed_obj_name = guessed_obj->value;

    AkiAnswer ans = ask(">>> It's \"%s\"", guessed_obj_name);
    if (ans == AKI_NO) {
        char *new_obj_name = aki_getline("Then what is it? ");
        char *diff_str = aki_getline("Explain what makes '%s' diffrent from '%s': ",
                                      new_obj_name, guessed_obj_name);

        bt_change(guessed_obj, diff_str);
    
        bt_add_right(guessed_obj, new_obj_name, NULL);
        bt_add_left(guessed_obj, guessed_obj_name, NULL);
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
    fputs("Welcome to Akinator " AKINATOR_VERSION "\n", stdout);
}

static void get_filename(const char *prompt, char *out_filename) {
    assert(prompt != NULL);
    assert(out_filename != NULL);

    fputs(prompt, stdout);
    scanf("%" MAX_FILENAME_LEN_STR "s", out_filename);
    getchar(); // removing last \n
}

AkiStatus aki_prompt_load_db(AkiContext *ctx) {
    assert(ctx != NULL);

    get_filename("Enter database filename: ", ctx->db_filename);

    return load_db(ctx);
}

static AkiStatus handle_guess_action(AkiContext *ctx) {
    assert(ctx != NULL);

    guess_loop(ctx); 

    return AKI_OK;
}

static bool find_leaf_cb(Node *node, BtWalkState state, void *ctx) {
    assert(ctx != NULL);

    if (state != BT_LEAVE)
        return false;

    AkiFindContext *find_ctx = (AkiFindContext *)ctx;
    // TODO: optimize
    if (node_is_leaf(node) && strcasecmp(node->value, find_ctx->target) == 0) {
        find_ctx->found_node = node;
        return true;
    }

    return false;
}

static Node *aki_find_leaf(Node *node, const char *target) {
    assert(node != NULL);
    assert(target != NULL);

    AkiFindContext find_ctx = { .target = target };
    bt_walk(node, &find_leaf_cb, &find_ctx);

    return find_ctx.found_node;
}

static void print_definition(stack_t *path_stk, const char *obj_name) {
    assert(path_stk != NULL);
    assert(obj_name != NULL);

    StackStatus st_status = 0;
    Node *prev = stack_pop(path_stk, &st_status);
    printf("--- '%s' is: ", obj_name);

    const char *prefix = "";
    bool first = true;

    while (true) {
        Node *node = stack_pop(path_stk, &st_status);
        if (st_status == STACK_CANNOT_POP_FROM_EMPTY)
            break;

        if (prev->left == node)
            prefix = "NOT ";
        else
            prefix = "";

        if (first) {
            printf("%s%s", prefix, prev->value);
            first = false;
        } else
            printf(", %s%s", prefix, prev->value);

        prev = node;
    }

    fputc('\n', stdout);
}

static AkiStatus get_path_to_node(Node *node, stack_t *stack) {
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
    AkiStatus aki_status = AKI_OK;

    obj_name = aki_getline("What's the object you searching for? ");
    Node *found_obj = aki_find_leaf(ctx->root, obj_name);

    if (found_obj == NULL) {
        fputs("No such object :(\n", stdout);

        aki_status = AKI_OK;
        goto cleanup;
    }

    StackStatus st_status = STACK_INIT_DBG(&path_stk, 16);
    if (st_status != STACK_OK) { aki_status = AKI_STACK_ERROR; goto cleanup; }

    aki_status = get_path_to_node(found_obj, &path_stk);
    if (aki_status != AKI_OK) goto cleanup;

    print_definition(&path_stk, found_obj->value);

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

    char *obj_name = aki_getline(prompt);
    Node *found_obj = aki_find_leaf(ctx->root, obj_name);
    if (found_obj == NULL) {
        free(obj_name);
        return AKI_NO_SUCH_OBJECT;
    }

    AkiStatus status = get_path_to_node(found_obj, stack);
    if (status != AKI_OK) {
        free(obj_name);
        return AKI_NO_SUCH_OBJECT;
    }

    free(obj_name);

    return AKI_OK;
}

static void print_similarity(const char *similarity, bool negated) {
    assert(similarity != NULL);

    if (negated) {
        printf("NOT %s, ", similarity);
    } else {
        printf("%s, ", similarity);
    } 
}

static void print_difference(const char *first_str, const char *second_str,
                             bool negated_first, bool negated_second) {
    assert(first_str != NULL);
    assert(second_str != NULL);

    printf("[first object] %s%s, ",
            (negated_first) ? "NOT " : "", first_str);
    printf("[second object] %s%s, ",
            (negated_second) ? "NOT " : "", second_str);
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

    Node *prev1 = NULL, *prev2 = NULL;
    AkiStatus status = pop_from_two_stacks(stack1, stack2, &prev1, &prev2);
    if (status != AKI_OK) return status;

    bool both_banner_printed = false, diff_banner_printed = false;

    while (prev1 != NULL && prev2 != NULL) {
        if (node_is_leaf(prev1) || node_is_leaf(prev2))
            break;

        Node *elem1 = NULL, *elem2 = NULL;
        status = pop_from_two_stacks(stack1, stack2, &elem1, &elem2);
        // if (status != AKI_OK) return status; // TODO: maybe comment

        bool yes_path_elem1 = prev1->right == elem1;
        bool yes_path_elem2 = prev2->right == elem2;
        
        if (prev1 == prev2 && yes_path_elem1 == yes_path_elem2) {
            if (!both_banner_printed) {
                fputs("\n--- They both are: ", stdout);
                both_banner_printed = true;
            }
            print_similarity(prev1->value, !yes_path_elem1 && !yes_path_elem2);
        } else {
            if (!diff_banner_printed) {
                fputs("\n--- They differ in: ", stdout);
                diff_banner_printed = true;
            }
            print_difference(prev1->value, prev2->value,
                             !yes_path_elem1, !yes_path_elem2);
        }

        prev1 = elem1;
        prev2 = elem2;
    }

    fputc('\n', stdout);

    return AKI_OK;
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
    status = aki_prompt_load_db(ctx);
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

