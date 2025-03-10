type lexer

val create_lexer : in_channel -> lexer
val next_token : lexer -> Token.token
