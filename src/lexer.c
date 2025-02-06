#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

#define INITIAL_BUFFER_SIZE 50

Lexer *create_lexer(FILE *stream) {
    Lexer *l = (Lexer *)malloc(sizeof(Lexer));
    l->source = stream;

    return l;
}