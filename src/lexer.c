#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

#define INITIAL_BUFFER_SIZE 50

Lexer *create_lexer(FILE *stream) {
    Lexer *l = (Lexer *)malloc(sizeof(Lexer));
    l->source = stream;

    return l;
}

int handle_number(char **buffer, size_t *buffer_size, FILE *stream, char first) {
    (*buffer)[0] = first;
    size_t idx = 0;

    int ch;
    int is_int = 1;
    while ((ch = fgetx(stream)) != EOF) {
        if (!(isdigit(ch) || ch == '.')) {
            break;
        }

        if (idx >= *buffer_size - 1) {
            *buffer_size += 50;
            char *temp = realloc(*buffer, *buffer_size);
            if (!temp) {
                perror("Failed to reallocate memory for buffer");
                exit(1);
            }
            *buffer = temp;
        }
        (*buffer)[idx++] = ch;


        if (ch == '.') {
            if (is_int) {
                is_int = 0;
            } else {
                fprintf(stderr, "Unexpected decimal point: %s", *buffer);
                exit(1);
            }
        }
    }

    return is_int;
}

void handle_alphabetic(char **buffer, size_t *buffer_size, FILE *stream, char first) {
    (*buffer)[0] = first;
    size_t idx = 0;

    int ch;
    while ((ch = fgetc(stream)) != EOF) {
        if (!(isalnum(ch) || ch == '_')) {
            break;
        }

        if (idx >= *buffer_size - 1) {
            *buffer_size += 50;
            char *temp = realloc(*buffer, *buffer_size);
            if (!temp) {
                perror("Failed to reallocate memory for buffer");
                exit(1);
            }
            *buffer = temp;
        }
        (*buffer)[idx++] = ch;
    }
}

int is_keyword(char *s) {
    char *keywords[8] = {"int", "float", "str", "String", "bool", "File", "static", "mut"};

    for (int i = 0; i < 8; i++) {
        if (strcmp(keywords[i], s) == 0) {
            return 1;
        }
    }
    return 0;
}

Token *next_token(Lexer *lexer) {
    Token *token = (Token *)malloc(sizeof(Token));
    if (!token) {
        perror("Failed to allocate memory for token");
        exit(1);
    }

    size_t buffer_size = INITIAL_BUFFER_SIZE;
    char *value = malloc(buffer_size);
    if (!value) {
        perror("Failed to allocate memory for value");
        exit(1);
    }

    int ch;
    while ((ch = fgetc(lexer->source)) != EOF) {
        if (isspace(ch)) {
            continue;
        }

        if (isdigit(ch)) {
            if (handle_number(&value, &buffer_size, lexer->source, ch)) {
                token->type = TOKEN_INTEGER;
            } else {
                token->type = TOKEN_FLOAT;
            }
            token->value = strdup(value);
            break;
        }

        if (isalpha(ch) || ch == '_') {
            handle_alphabetic(&value, &buffer_size, lexer->source, ch);
            if (is_keyword(value)) {
                token->type = TOKEN_KEYWORD;
            } else {
                token->type = TOKEN_IDENTIFIER;
            }
            token->value = strdup(value);
            break;
        }
    }
}