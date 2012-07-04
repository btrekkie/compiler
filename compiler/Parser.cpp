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
    void* parseData = setFileToParse(fopen(filename.c_str(), "r"));
    yyparse();
    freeParseData(parseData);
    return curNode;
}

void processFile(ASTNode* node) {
    curNode = node;
}
