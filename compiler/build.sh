#!/bin/sh

lex tokens.l
yacc --verbose --debug -d grammar.y -o grammar.cpp
cc -c lex.yy.c -o lex.yy.o
cc -c ast.c -Wall -o ast.o
c++ lex.yy.o grammar.cpp ast.o ast_util.cpp break_evaluator.cpp cfg.cpp \
    compiler.cpp cpp_compiler.cpp -Wall -o compiler
