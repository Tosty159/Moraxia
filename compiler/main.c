#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <Moraxia source code>", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");

    Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
    if (!lexer) {
        perror("Failed to allocate memory for lexer");
        return 1;
    }
    lexer->source = file;
    lexer->position = 0;

    return 0;
}