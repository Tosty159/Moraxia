add_executable(compiler
    main.c
    lexer.c
    parser.c
    interpreter.c
)

target_include_directories(compiler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(compiler PRIVATE -Wall -Wextra)