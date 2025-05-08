#include "lexeparser/lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define INITIAL_BUFFER_SIZE 64

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
    if (lexer->is_over) {
        return;
    }

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

char lexer_peek(Lexer *lexer) {
    if (lexer->is_over) {
        return '\0';
    }

    int ch = fgetc(lexer->stream);
    if (ch == EOF) {
        lexer->is_over = 1;
        return '\0';
    }
    ungetc(ch, lexer->stream);

    return (char)ch;
}

void skip_whitespace(Lexer *lexer) {
    while (!lexer->is_over) {
        if (!isspace(lexer->current_char)) {
            break;
        }
        lexer_advance(lexer);
    }
}

void skip_comments(Lexer *lexer) {
    while (!lexer->is_over) {
        if (lexer->current_char != '/') {
            break;
        }

        char next_char = lexer_peek(lexer);
        if (next_char == '/') { // Case: '//'
            while (!lexer->is_over) {
                if (lexer->current_char == '\n') {
                    lexer_advance(lexer);
                    break;
                }
                lexer_advance(lexer);
            }
        } else if (next_char == '*') { // Case: '/* ... */'
            lexer_advance(lexer); // Consume '/'
            lexer_advance(lexer); // Consume '*': To avoid cases like '/*/ ... */'
            while (!lexer->is_over) {
                if (lexer->current_char == '*' && lexer_peek(lexer) == '/') {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    break;
                }
                lexer_advance(lexer);
            }
        } else {
            break;
        }
    }
}

Token next_token(Lexer *lexer) {
    skip_whitespace(lexer);
    skip_comments(lexer);

    if (lexer->is_over) {
        return (Token){TOKEN_EOF, NULL, lexer->line, lexer->column};
    }

    while (lexer->current_char != '\0') {
        char ch = lexer->current_char;

        if (isdigit(ch)) {
            size_t size = INITIAL_BUFFER_SIZE;
            char *result_num = (char *)malloc(size);
            if (!result_num) {
                perror("Failed to allocate memory for number");
                exit(EXIT_FAILURE);
            }

            result_num[0] = ch;
            lexer_advance(lexer);

            int is_int = 1; // Assume it's an integer first

            size_t count = 1;
            while (!lexer->is_over) {
                if (!(isdigit(lexer->current_char) || lexer->current_char == '.')) {
                    break;
                }

                if (count >= size - 1) {
                    size *= 2;
                    char *temp = (char *)realloc(result_num, size);
                    if (!temp) {
                        perror("Failed to reallocate memory for number");
                        free(result_num);
                        exit(EXIT_FAILURE);
                    }
                    result_num = temp;
                    free(temp);
                }
                result_num[count++] = lexer->current_char;

                if (lexer->current_char == '.') {
                    if (!is_int) {
                        fprintf(stderr, "Error: Multiple decimal points in number: '%s.', line: %lu, pos: %lu\n",
                            result_num, lexer->line, lexer->column);
                        free(result_num);
                        exit(EXIT_FAILURE);
                    }
                    is_int = 0; // It's a float
                }

                lexer_advance(lexer);
            }
            result_num[count] = '\0';

            return (Token){TOKEN_INT, result_num, lexer->line, lexer->column};
        }
    }
}