#!/bin/sh

lex tokens.l
yacc --verbose --debug -d grammar.y -o grammar.cpp
cc -c lex.yy.c -o lex.yy.o
cc -c ASTNode.c -Wall -o ASTNode.o
c++ -c ASTUtil.cpp -Wall -o ASTUtil.o
c++ -c BreakEvaluator.cpp -Wall -o BreakEvaluator.o
c++ -c CFG.cpp -Wall -o CFG.o
c++ -c Compiler.cpp -Wall -o Compiler.o
c++ -c CPPCompiler.cpp -Wall -o CPPCompiler.o
c++ -c Interface.cpp -Wall -o Interface.o
c++ -c JSON.cpp -Wall -o JSON.o
c++ lex.yy.o grammar.cpp ASTNode.o ASTUtil.o BreakEvaluator.o CFG.o Compiler.o \
    CPPCompiler.o Interface.o -Wall -o compiler
