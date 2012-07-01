#!/bin/sh

lex tokens.l
yacc --verbose --debug -d grammar.y -o grammar.cpp
cc -c lex.yy.c -o lex.yy.o
cc -c ast.c -Wall -o ast.o
c++ -c ast_util.cpp -Wall -o ast_util.o
c++ -c break_evaluator.cpp -Wall -o break_evaluator.o
c++ -c cfg.cpp -Wall -o cfg.o
c++ -c compiler.cpp -Wall -o compiler.o
c++ -c cpp_compiler.cpp -Wall -o cpp_compiler.o
c++ -c interface.cpp -Wall -o interface.o
c++ lex.yy.o grammar.cpp ast.o ast_util.o break_evaluator.o cfg.o  compiler.o \
    cpp_compiler.o interface.o -Wall -o compiler
