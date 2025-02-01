#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_BUFFER_SIZE 64

Token *create_token(TokenType type, char* value) {
    Token *token = (Token *)malloc(sizeof(Token));
    if (!token) {
        perror("Failed to allocate memory for token");
        exit(1);
    }

    token->type = type;
    token->value = strdup(value);
    if (!token->value) {
        perror("Failed to allocate memory for token value");
        free(token);
        exit(1);
    }

    return token;
}

void handle_number(char **buffer, size_t *buffer_size, FILE *stream, char first) {
    (*buffer)[0] = first;
    size_t index = 1;

    int ch;
    while ((ch = fgetc(stream)) != EOF) {
        if (!isdigit(ch)) {
            break;
        }

        if (index >= *buffer_size - 1) {
            *buffer_size += 50;
            *buffer = realloc(*buffer, *buffer_size);
            if (!*buffer) {
                perror("Failed to reallocate memory");
                exit(1);
            }
        }

        (*buffer)[index++] = ch;
    }
    (*buffer)[index] = '\0';
}

void handle_alphabetic(char **buffer, size_t *buffer_size, FILE *stream, char first) {
    (*buffer)[0] = first;

    int ch = 0;
    int index = 1;
    while ((ch = fgetc(stream)) != EOF) {
        if (!isalnum(ch)) {
            break;
        }

        if (index >= *buffer_size - 1) {
            *buffer_size += 50;
            *buffer = realloc(*buffer, *buffer_size);
            if (!*buffer) {
                perror("Failed to reallocate memory");
                exit(1);
            }
        }

        (*buffer)[index++] = ch;
    }
    (*buffer)[index] = '\0';
}

Token *next_token(Lexer *lexer) {
    size_t buffer_size = INITIAL_BUFFER_SIZE;
    char *value = malloc(buffer_size);
    if (!value) {
        perror("Failed to allocate memory");
        exit(1);
    }

    TokenType type;
    int ch;

    while ((ch = fgetc(lexer->source)) != EOF) {
        if (isspace(ch)) {
            continue;
        }
        
        if (isdigit(ch)) {
            handle_number(&value, &buffer_size, lexer->source, ch);
            type = Literal;
            break;
        }

        if (isalpha(ch)) {
            handle_alphabetic(&value, &buffer_size, lexer->source, ch);
            type = Identifier;
            break;
        }

        if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=') {
            value[0] = ch;
            value[1] = '\0';
            type = Operator;
            break;
        }

        fprintf(stderr, "Unexpected character %c", ch);
        free(value);
        exit(1);
    }

    lexer->position = ftell(lexer->source);

    Token *token = create_token(type, value);

    free(value);

    return token;
}