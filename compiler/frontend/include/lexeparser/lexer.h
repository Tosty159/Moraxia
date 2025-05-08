#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#include "lexeparser/token.h"

typedef struct Lexer {
    FILE *stream;
    char current_char;
    unsigned long line;
    unsigned long column;
    int is_over;
} Lexer;

Lexer *lexer_create(FILE *stream);
void lexer_destroy(Lexer *lexer);

void lexer_advance(Lexer *lexer);

Token next_token(Lexer *lexer);

#endif