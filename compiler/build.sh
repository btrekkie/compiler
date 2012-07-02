#!/bin/bash

cd grammar
lex tokens.l
cd ../
yacc --verbose --debug -d grammar/grammar.y -o grammar/grammar.cpp
cc -c grammar/lex.yy.c -o grammar/lex.yy.o
cc -c grammar/ASTNode.c -Wall -o grammar/ASTNode.o

export FILES="ASTUtil BreakEvaluator CFG Compiler CPPCompiler Interface "\
"InterfaceInput InterfaceOutput JSONDecoder JSONEncoder JSONValue "\
"test/TestCase test/TestRunner"

export O_FILES=''
for FILE in $FILES
do
    c++ -c $FILE.cpp -Wall -o $FILE.o
    export O_FILES="$O_FILES $FILE.o"
done
echo $O_FILES
c++ grammar/lex.yy.o grammar/grammar.cpp grammar/ASTNode.o $O_FILES \
    -Wall -o compiler
