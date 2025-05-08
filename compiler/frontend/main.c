#include <stdio.h>
#include <stdlib.h>

#include "utils/io.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *file = open_file(argv[1], "r", ".mx");

    return 0;
}