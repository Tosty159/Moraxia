typedef enum {
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,
    TOKEN_OPERATOR,
    TOKEN_PUNCTUATION,
    TOKEN_SEMICOLON,
    TOKEN_EOF,
    TOKEN_INVALID,
} TokenType;

typedef struct {
    TokenType type;
    char *value;
} Token;

typedef struct {
    FILE *source;
} Lexer;

Lexer *create_lexer(FILE *stream);
Token *next_token(Lexer *lexer);