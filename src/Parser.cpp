extern "C" {
    #include "grammar/grammar.h"
}

#include <stdio.h>
#include "Parser.hpp"

using namespace std;

/**
 * The AST produced by the most recent call to yyparse().
 */
static ASTNode* curNode;

ASTNode* Parser::parseFile(string filename) {
    FILE* file = fopen(filename.c_str(), "r");
    void* parseData = setFileToParse(file);
    yyparse();
    fclose(file);
    freeParseData(parseData);
    return curNode;
}

void processFile(ASTNode* node) {
    curNode = node;
}
