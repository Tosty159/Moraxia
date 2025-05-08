#include "lexeparser/lexer.h"

#include <stdio.h>
#include <stdlib.h>

Lexer *lexer_create(FILE *stream) {
    Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
    if (!lexer) {
        perror("Failed to allocate memory for lexer");
        exit(EXIT_FAILURE);
    }

    lexer->stream = stream;
    lexer->current_char = fgetc(stream);
    lexer->line = 1;
    lexer->column = 1;

    return lexer;
}

void lexer_destroy(Lexer *lexer) {
    if (lexer) {
        fclose(lexer->stream);
        free(lexer);
    }
}

void lexer_advance(Lexer *lexer) {
    if (lexer->current_char == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }

    int ch;
    if ((ch = fgetc(lexer->stream)) == EOF) {
        lexer->is_over = 1;
        lexer->current_char = '\0';
    } else {
        lexer->current_char = (char)ch;
    }
}

Token next_token(Lexer *lexer) {}