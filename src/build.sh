#!/bin/bash

if [ $# -eq 0 ]
then
    echo "Missing compilation target"
    exit -1
elif [ $# -ne 1 ]
then
    echo "build.sh only takes one argument: the compilation target"
    exit -1
elif [ $1 != "compiler" -a $1 != "test" ]
then
    echo "Invalid compilation target"
    exit -1
fi

cd grammar
lex tokens.l
cd ../
yacc --verbose --debug -d grammar/grammar.y -o grammar/grammar.cpp
cc -c grammar/lex.yy.c -o grammar/lex.yy.o
cc -c grammar/ASTNode.c -Wall -o grammar/ASTNode.o

export FILES="ASTUtil BinaryCompiler BreakEvaluator CFG Compiler "\
"CompilerErrors CPPCompiler FileManager Interface InterfaceInput "\
"InterfaceOutput JSONDecoder JSONEncoder JSONValue Parser Process "\
"VarResolver grammar/grammar"

# Target-specific logic
if [ $1 = "compiler" ]
then
    export MAIN_FILE="CompilerMain.cpp"
    export EXECUTABLE_FILE="compiler"
elif [ $1 = "test" ]
then
    export FILES="$FILES test/ASTUtilTest test/BinaryCompilerTest "\
"test/InterfaceIOTest test/JSONTest test/TestCase test/TestRunner "\
"test/UniverseSetTest"
    export MAIN_FILE="TestCompiler.cpp"
    export EXECUTABLE_FILE="test_compiler"
fi

# Compile source files
export OBJ_FILES="grammar/lex.yy.o grammar/ASTNode.o"
export EXIT_CODE=0
for FILE in $FILES
do
    export OBJ_FILES="$OBJ_FILES $FILE.o"
    c++ -c $FILE.cpp -Wall -o $FILE.o
    export CUR_EXIT_CODE=$?
    if [ $CUR_EXIT_CODE -ne 0 ]
    then
        export EXIT_CODE=$CUR_EXIT_CODE
    fi
done

# Compile executable file
c++ $OBJ_FILES $MAIN_FILE -Wall -o $EXECUTABLE_FILE
export CUR_EXIT_CODE=$?
if [ $CUR_EXIT_CODE -ne 0 ]
then
    export EXIT_CODE=$CUR_EXIT_CODE
fi
if [ $EXIT_CODE -ne 0 ]
then
    exit $EXIT_CODE
fi

# Run unit tests
if [ $1 = "test" ]
then
    ./test_compiler
fi
