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

int is_symbol(char ch) {
    char symbols[23] = "+-*/\%^>=<!~&|,.:()[]{}";

    return strchr(symbols, ch) != NULL;
}

TokenType handle_symbol(char **buffer, size_t *buffer_size, FILE *stream, char first) {
    TokenType type;

    (*buffer)[0] = first;
    size_t idx = 1;

    switch (first) {
        case '+': case '-': case '*': case '/':
        case '=': case '>': case '!':
            // Operators that can be followed by a '='
            type = TOKEN_OPERATOR;

            int c;
            if ((c = fgetc(stream)) != EOF) {
                if (c == '=') {
                    (*buffer)[idx++] = c;
                } else {
                    ungetc(c, stream);
                }
            }
            break;
        case '&': case '|':
            // Operators that come in two, e.g. '&&'
            type = TOKEN_OPERATOR;

            int c;
            if ((c = fgetc(stream)) != EOF) {
                if (c == first) {
                    (*buffer)[idx++] = c;
                } else {
                    type = TOKEN_INVALID;

                    char error_message[25];
                    snprintf(error_message, sizeof(error_message), "Invalid operator: %c", first);
                    (*buffer) = strdup(error_message);
                }
            }
            break;
        case '^':
            // Can come in one or two, e.g. '^'(exponent) or '^^'(XOR)
            type = TOKEN_OPERATOR;

            int c;
            if ((c = fgetc(stream)) != EOF) {
                if (c == first) {
                    (*buffer)[idx++] = c;
                } else {
                    ungetc(c, stream);
                }
            }
            break;
        case '~':
            // Operators that only come alone
            type = TOKEN_OPERATOR;
            break;
        case ',': case '.': case ':': case '(': case ')':
        case '[': case '{': case '}':
            // Punctuation
            type = TOKEN_PUNCTUATION;
            break;
        case ']':
            // Punctuation that can have more than one character
            type = TOKEN_PUNCTUATION;

            int c;
            if ((c = fgetc(stream)) != EOF) {
                if (c == '>') {
                    (*buffer)[idx++] = c;
                } else {
                    ungetc(c, stream);
                }
            }
            break;
        case '<':
            // Can be a punctuation or an operator
            int c;
            if ((c = fgetc(stream)) != EOF) {
                if (c == '=') {
                    // Is an operator
                    (*buffer)[idx++] = c;

                    type = TOKEN_OPERATOR;
                } else if (c == '[') {
                    // Is punctuation
                    (*buffer)[idx++] = c;

                    type = TOKEN_PUNCTUATION;
                } else {
                    ungetc(c, stream);
                }
            }
            break;
        default:
            // This won't be reached if the check for 'is_symbol()' is ran first
            type = TOKEN_INVALID;
            break;
    }

    (*buffer)[idx] = '\0';
    return type;
}

void handle_space(FILE *stream) {
    int ch;
    while ((ch = fgetc(stream)) != EOF) {
        if (!isspace(ch)) {
            ungetc(ch, stream);
            break;
        }
    }
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

    handle_space(lexer->source);

    int ch;
    if ((ch = fgetc(lexer->source)) == EOF) {
        token->type = TOKEN_EOF;

        value[0] = '\0';
        token->value = strdup(value);

        free(value);
        return token;
    }

    if (isdigit(ch)) {
        if (handle_number(&value, &buffer_size, lexer->source, ch)) {
            token->type = TOKEN_INTEGER;
        } else {
            token->type = TOKEN_FLOAT;
        }
        token->value = strdup(value);
    }

    if (isalpha(ch) || ch == '_') {
        handle_alphabetic(&value, &buffer_size, lexer->source, ch);
        if (is_keyword(value)) {
            token->type = TOKEN_KEYWORD;
        } else {
            token->type = TOKEN_IDENTIFIER;
        }
        token->value = strdup(value);
    }

    if (is_symbol(ch)) {
        token->type = handle_symbol(&value, &buffer_size, lexer->source, ch);
        token->value = strdup(value);
    }

    token->type = TOKEN_INVALID;

    char error_message[25];
    snprintf(error_message, sizeof(error_message), "Invalid character: %c", ch);
    token->value = strdup(error_message);

    free(value);
    return token;
}