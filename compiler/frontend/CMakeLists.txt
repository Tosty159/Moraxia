cmake_minimum_required(VERSION 4.0)

project(compiler_frontend)

add_executable(compiler_frontend
    main.c
    utils/io.c
    lexeparser/token.c
    lexeparser/lexer.c
    lexeparser/ast.c
    lexeparser/parser.c
)

target_include_directories(compiler_frontend
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)