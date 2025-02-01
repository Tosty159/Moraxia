#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

int main(int argc, char *argv[]) {
    char path[] = "../src/main.mx";

    FILE *file = fopen(path, "r");

    Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
    if (!lexer) {
        perror("Failed to allocate memory for lexer");
        exit(1);
    }
    lexer->source = file;
    lexer->position = 0;
}