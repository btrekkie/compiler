#include <stdlib.h>
#include <string.h>
#include "ASTNode.h"

extern int yylineno;

/**
 * Returns a new ASTNode, with the "lineNumber" field set appropriately and all
 * the other fields set to NULL and 0.
 */
ASTNode* astNew() {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    memset(node, 0, sizeof(ASTNode));
    node->lineNumber = yylineno;
    handleASTNodeCreated(node);
    return node;
}

ASTNode* astNewStr(int type, const char* tokenStr) {
    ASTNode* node = astNew();
    node->type = type;
    node->tokenStr = (char*)malloc(strlen(tokenStr) + 1);
    memcpy(node->tokenStr, tokenStr, strlen(tokenStr) + 1);
    return node;
}

ASTNode* astNew0(int type) {
    ASTNode* node = astNew();
    node->type = type;
    return node;
}

ASTNode* astNew1(int type, ASTNode* child1) {
    ASTNode* node = astNew();
    node->type = type;
    node->child1 = child1;
    return node;
}

ASTNode* astNew2(int type, ASTNode* child1, ASTNode* child2) {
    ASTNode* node = astNew();
    node->type = type;
    node->child1 = child1;
    node->child2 = child2;
    return node;
}

ASTNode* astNew3(int type, ASTNode* child1, ASTNode* child2, ASTNode* child3) {
    ASTNode* node = astNew();
    node->type = type;
    node->child1 = child1;
    node->child2 = child2;
    node->child3 = child3;
    return node;
}

ASTNode* astNew4(
    int type,
    ASTNode* child1,
    ASTNode* child2,
    ASTNode* child3,
    ASTNode* child4) {
    ASTNode* node = astNew();
    node->type = type;
    node->child1 = child1;
    node->child2 = child2;
    node->child3 = child3;
    node->child4 = child4;
    return node;
}

void astFree(ASTNode* node) {
    if (node->tokenStr != NULL)
        free(node->tokenStr);
    else if (node->child1 != NULL) {
        astFree(node->child1);
        if (node->child2 != NULL) {
            astFree(node->child2);
            if (node->child3 != NULL) {
                astFree(node->child3);
                if (node->child4 != NULL)
                    astFree(node->child4);
            }
        }
    }
    free(node);
}
