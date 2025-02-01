#include <stdio.h>

typedef enum {
    Literal,
    Identifier,
    Keyword,
    Operator,
    Semicolon,
    END,
} TokenType;

typedef struct {
    TokenType type;
    char* value;
} Token;

typedef struct {
    FILE *source;
    long position;
} Lexer;

Token next_token(Lexer *lexer);