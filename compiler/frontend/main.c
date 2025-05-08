#include <stdio.h>
#include <stdlib.h>

#include "utils/io.h"
#include "lexeparser/token.h"
#include "lexeparser/lexer.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *file = open_file(argv[1], "r", ".mx");

    Lexer *lexer = lexer_create(file);

    Token token = next_token(lexer);
    while (token.type != TOKEN_EOF) {
        char *token_str = token_to_string(token);
        printf("%s\n", token_str);
        free(token_str);
        token = next_token(lexer);
    }
    char *token_str = token_to_string(token);
    printf("%s\n", token_str);
    free(token_str);

    return 0;
}