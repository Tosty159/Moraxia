#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/io.h"

FILE *open_file(const char *filename, const char *mode, const char *extension) {
    // Check if the filename has the correct extension
    const char *dot = strrchr(filename, '.');
    if (!dot || strcmp(dot, extension) != 0) {
        fprintf(stderr, "Error: File '%s' does not have the correct extension '%s'.\n", filename, extension);
        exit(EXIT_FAILURE);
    }

    // Open the file
    FILE *file = fopen(filename, mode);
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'.\n", filename);
        exit(EXIT_FAILURE);
    }

    return file;
}