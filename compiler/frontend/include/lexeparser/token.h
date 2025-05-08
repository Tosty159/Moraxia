#ifndef TOKEN_H
#define TOKEN_H

typedef enum TokenType {
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_CHAR,
    TOKEN_BOOL,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,
    TOKEN_OPERATOR,
    TOKEN_PUNCTUATION,
    TOKEN_SEMICOLON,
    TOKEN_ERROR,
    TOKEN_EOF,
} TokenType;

typedef struct Token {
    TokenType type;
    const char *value;
    unsigned long line;
    unsigned long column;
} Token;

char *token_type_to_string(TokenType type);
char *token_to_string(Token token);

#endif