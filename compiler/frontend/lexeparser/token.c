#include <stdio.h>
#include <stdlib.h>

#include "lexeparser/token.h"

char *token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_INT: return "TOKEN_INT";
        case TOKEN_FLOAT: return "TOKEN_FLOAT";
        case TOKEN_STRING: return "TOKEN_STRING";
        case TOKEN_IDENTIFIER: return "TOKEN_IDENTIFIER";
        case TOKEN_KEYWORD: return "TOKEN_KEYWORD";
        case TOKEN_OPERATOR: return "TOKEN_OPERATOR";
        case TOKEN_PUNCTUATION: return "TOKEN_PUNCTUATION";
        case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
        case TOKEN_ERROR: return "TOKEN_ERROR";
        case TOKEN_EOF: return "TOKEN_EOF";
    }
}

char *token_to_string(Token token) {
    char *type_str = token_type_to_string(token.type);
    char *value_str = token.value ? token.value : "NULL";

    int len = snprintf(NULL, 0, "Token(type: %s, value: %s, line: %lu, column: %lu)", type_str, value_str, token.line, token.column);
    
    if (len < 0) {
        perror("Failed to format token string");
        exit(EXIT_FAILURE);
    }

    char *result = (char *)malloc(len + 1);
    if (!result) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    snprintf(result, len + 1, "Token(type: %s, value: %s, line: %lu, column: %lu)", type_str, value_str, token.line, token.column);

    return result;
}