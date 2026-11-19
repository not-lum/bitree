#include "akinator_utils.h"
#include "../include/bitree.h"
#include "../include/bitree_dump.h"
#include "file_utils.h"

#include <assert.h>
#include <string.h>
#include <stdbool.h>

// #define MAX_FILENAME_LEN 128
// char buff[] = "(\"root\"(\"left\"(\"left of left\"#(\"right of left of left\"##))#)(\"abracadabra\"#(\"bibobabobus\"#(\"fffff\"#(\"aaaaa aa\"##)))))";
int main() {
    AkiContext ctx = {};
    aki_start_game(&ctx);
}