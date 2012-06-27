#!/bin/sh

lex tokens.l
yacc --verbose --debug -d grammar.y -o grammar.cpp
cc -c lex.yy.c -o lex.yy.o
cc -c ast.c -Wall -o ast.o
c++ lex.yy.o grammar.cpp ast.o cfg.cpp compiler.cpp cpp_compiler.cpp -Wall \
    -o compiler
