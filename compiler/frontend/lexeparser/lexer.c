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

int is_keyword(const char *str) {
    char *keywords[7] = {
        "let", "fun", "if", "else", "while", "for", "return",
    };

    for (int i = 0; i < 6; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int could_contain(char *set[], size_t size, char *elem) {
    for (size_t i = 0; i < size; i++) {
        if (strstr(set[i], elem) == 0) {
            return 1;
        }
    }
    return 0;
}

char *is_operator(Lexer *lexer) {
    char *all_ops[38] = {
        "=", ":=", "+=", "-=", "*=", "/=", "%=", "//=", "**=",
        ":", "->", "+", "-", "*", "/", "%", "//", "**", "&&",
        "||", "^^", "!", "==", "!=", "<", "<=", ">=", ">", "~",
        "(", ")", "[", "]", "{", "}", ".", "..", "...",
    };

    char *buff = (char *)malloc(4);
    char *last_op = (char *)malloc(4);
    if (!buff || !last_op) {
        perror("Failed to allocate memory for operator");
        exit(EXIT_FAILURE);
    }

    // Make sure it is the start of an operator
    buff[0] = lexer->current_char;
    if (!could_contain(all_ops, 38, buff)) {
        free(buff);
        free(last_op);
        return NULL;
    }
    lexer_advance(lexer);

    int count = 1;
    while (!lexer->is_over) {
        buff[count++] = lexer->current_char;
        if (!could_contain(all_ops, 38, buff)) {
            break;
        }

        strcpy(last_op, buff);
        lexer_advance(lexer);
    }
    last_op[count] = '\0';

    return last_op;
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

        if (isalpha(ch) || ch == '_') {
            size_t size = INITIAL_BUFFER_SIZE;
            char *name = (char *)malloc(size);
            if (!name) {
                perror("Failed to allocate memory for identifier");
                exit(EXIT_FAILURE);
            }

            name[0] = ch;
            lexer_advance(lexer);

            size_t count = 1;
            while (isalnum(lexer->current_char) || lexer->current_char == '_') {
                if (count >= size - 1) {
                    size *= 2;
                    char *temp = (char *)realloc(name, size);
                    if (!temp) {
                        perror("Failed to reallocate memory for identifier");
                        free(name);
                        exit(EXIT_FAILURE);
                    }
                    name = temp;
                }

                name[count++] = lexer->current_char;
                lexer_advance(lexer);
            }
            name[count] = '\0';

            TokenType type = TOKEN_IDENTIFIER;
            if (is_keyword(name)) {
                type = TOKEN_KEYWORD;
            } else if (strcmp(name, "true") == 0 || strcmp(name, "false") == 0) {
                type = TOKEN_BOOL;
            }
            
            return (Token){type, name, lexer->line, lexer->column};
        }

        if (ch == '\'') {
            lexer_advance(lexer); // Skip the opening quote

            char *char_literal = (char *)malloc(3);
            if (!char_literal) {
                perror("Failed to allocate memory for char literal");
                exit(EXIT_FAILURE);
            }

            int count = 0;
            if (lexer->current_char == '\\') {
                char_literal[count++] = '\\';
                lexer_advance(lexer);
            }
            char_literal[count++] = lexer->current_char;
            lexer_advance(lexer);

            if (lexer->current_char != '\'') {
                fprintf(stderr, "Error: Invalid character literal: '%s', line: %lu, pos: %lu\n",
                    char_literal, lexer->line, lexer->column);
                free(char_literal);
                exit(EXIT_FAILURE);
            }

            char_literal[count] = '\0';
            lexer_advance(lexer); // Skip the closing quote

            return (Token){TOKEN_CHAR, char_literal, lexer->line, lexer->column};
        }

        if (ch == '"') {
            lexer_advance(lexer); // Skip the opening quote

            size_t size = INITIAL_BUFFER_SIZE;
            char *str = (char *)malloc(size);
            if (!str) {
                perror("Failed to allocate memory for string");
                exit(EXIT_FAILURE);
            }

            size_t count = 0;
            while (lexer->current_char != '"') {
                if (lexer->is_over) {
                    fprintf(stderr, "Error: String literal not terminated: '\"%s', line: %lu, pos: %lu\n",
                        str, lexer->line, lexer->column);
                    free(str);
                    exit(EXIT_FAILURE);
                }

                if (count >= size - 1) {
                    size *= 2;
                    char *temp = (char *)realloc(str, size);
                    if (!temp) {
                        perror("Failed to reallocate memory for string");
                        free(str);
                        exit(EXIT_FAILURE);
                    }
                    str = temp;
                }

                str[count++] = lexer->current_char;
                lexer_advance(lexer);
            }
            str[count] = '\0';
            lexer_advance(lexer); // Skip the closing quote

            return (Token){TOKEN_STRING, str, lexer->line, lexer->column};
        }

        char *op = is_operator(lexer);
        if (op) {
            return (Token){TOKEN_OPERATOR, op, lexer->line, lexer->column};
        }
    }
}