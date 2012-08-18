extern "C" {
    #include "grammar/ASTNode.h"
    #include "grammar/grammar.h"
    
    void yyerror(const char* str);
    int yylex();
    
    extern int yylineno;
}

#include <set>
#include <stdio.h>
#include "Parser.hpp"
#include "StringUtil.hpp"

using namespace std;

/**
 * The filename passed to the most recent call to "parseFile".
 */
static wstring curFilename;
/**
 * The wostream to which to output parser (and lexer) errors produced by the
 * most recent call to yyparse().
 */
static wostream* curErrorOutput;
/**
 * A set of all of the ASTNodes allocated since the beginning of the last call
 * to "parseFile".
 */
static set<ASTNode*> allNodes;
/**
 * The AST produced by the most recent call to yyparse().
 */
static ASTNode* curNode;
/**
 * Whether the most recent call to yyparse() produced a parser (or lexer) error.
 */
static bool encounteredError;

void yyerror(const char* str) {
    *curErrorOutput << L"Syntax error in " << curFilename << L" at line " <<
        yylineno << L'\n';
    encounteredError = true;
}

ASTNode* Parser::parseFile(wstring filename, wostream& errorOutput) {
    FILE* file = fopen(StringUtil::asciiWstringToString(filename).c_str(), "r");
    void* parseData = setFileToParse(file);
    curFilename = filename;
    curErrorOutput = &errorOutput;
    curNode = NULL;
    encounteredError = false;
    yyparse();

    fclose(file);
    freeParseData(parseData);
    if (encounteredError) {
        curNode = NULL;
        
        // Deallocate the nodes in "allNodes".
        set<ASTNode*> nonRootNodes;
        for (set<ASTNode*>::const_iterator iterator = allNodes.begin();
             iterator != allNodes.end();
             iterator++) {
            ASTNode* node = *iterator;
            nonRootNodes.insert(node->child1);
            nonRootNodes.insert(node->child2);
            nonRootNodes.insert(node->child3);
            nonRootNodes.insert(node->child4);
        }
        for (set<ASTNode*>::const_iterator iterator = allNodes.begin();
             iterator != allNodes.end();
             iterator++) {
            if (nonRootNodes.count(*iterator) == 0)
                astFree(*iterator);
        }
    }
    allNodes.clear();
    return curNode;
}

void processFile(ASTNode* node) {
    curNode = node;
}

void handleASTNodeCreated(ASTNode* node) {
    allNodes.insert(node);
}
