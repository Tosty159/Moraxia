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

void handle_value(char **buffer, size_t *buffer_size, FILE *stream, char first, int (*is_valid)(int)) {
    (*buffer)[0] = first;
    size_t index = 1;

    int ch;
    while ((ch = fgetc(stream)) != EOF) {
        if (!is_valid(ch)) {
            ungetc(ch, stream);
            break;
        }

        if (index >= *buffer_size - 1) {
            *buffer_size += 50;
            *buffer = realloc(*buffer, *buffer_size);
            if (*buffer == NULL) {
                perror("Failed to reallocate memory");
                exit(1);
            }
        }

        (*buffer)[index++] = ch;
    }
    (*buffer)[index] = '\0';
}

void handle_whitespace(FILE *stream) {
    int ch;
    while ((ch = fgetc(stream)) != EOF) {
        if (!isspace(ch)) {
            ungetc(ch, stream);
            break;
        }
    }
}

int is_operator(char ch) {
    char ops[] = "+-*/=";

    return strchr(ops, ch) != NULL;
}

int is_punctuation(char ch) {
    char punct[] = ",()[].";

    return strchr(ops, ch) != NULL;
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

    handle_whitespace(lexer->source);

    if ((ch = fgetc(lexer->source)) == EOF) {
        value[0] = '\0';
        type = END;
        Token *token = create_token(type, value);
        return token;
    }

    if (isdigit(ch)) {
        handle_value(&value, &buffer_size, lexer->source, ch, isdigit);
        type = Literal;
    } else if (isalpha(ch)) {
        handle_value(&value, &buffer_size, lexer->source, ch, isalnum);
        type = Identifier;
    } else if (is_operator(ch)) {
        value[0] = ch;
        value[1] = '\0';
        type = Operator;
    } else if (is_punctuation(ch)) {
        value[0] = ch;
        value[1] = '\0';
        type = Punctuation;
    } else if (ch == ';') {
        value[0] = ch;
        value[1] = '\0';
        type = Semicolon;
    } else {
        fprintf(stderr, "Unexpected character %c\n", ch);
        free(value);
        exit(1);
    }

    lexer->position = ftell(lexer->source);

    Token *token = create_token(type, value);
    return token;
}