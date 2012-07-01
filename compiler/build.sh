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
c++ -c InterfaceInput.cpp -Wall -o InterfaceInput.o
c++ -c InterfaceOutput.cpp -Wall -o InterfaceOutput.o
c++ -c JSONDecoder.cpp -Wall -o JSONDecoder.o
c++ -c JSONEncoder.cpp -Wall -o JSONEncoder.o
c++ -c test/TestCase.cpp -Wall -o test/TestCase.o
c++ -c test/TestRunner.cpp -Wall -o test/TestRunner.o
c++ lex.yy.o grammar.cpp ASTNode.o ASTUtil.o BreakEvaluator.o CFG.o Compiler.o \
    CPPCompiler.o Interface.o InterfaceInput.o InterfaceOutput.o JSONDecoder.o \
    JSONEncoder.o -Wall -o compiler
