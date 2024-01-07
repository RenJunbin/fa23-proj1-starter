#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

int main() {
    FILE *fp = fopen("/home/styaeng/project/fa23-proj1-starter/tests/01-simple-in.snk", "r");
    // char c = fgetc(fp);
    load_board(fp);
    return 0;
}