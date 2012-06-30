#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode* astNew() {
    ASTNode* ast = (ASTNode*)malloc(sizeof(ASTNode));
    memset(ast, 0, sizeof(ASTNode));
    return ast;
}

ASTNode* astNewStr(int type, const char* tokenStr) {
    ASTNode* ast = astNew();
    ast->type = type;
    ast->tokenStr = (char*)malloc(strlen(tokenStr) + 1);
    memcpy(ast->tokenStr, tokenStr, strlen(tokenStr) + 1);
    return ast;
}

ASTNode* astNew0(int type) {
    ASTNode* ast = astNew();
    ast->type = type;
    return ast;
}

ASTNode* astNew1(int type, ASTNode* child1) {
    ASTNode* ast = astNew();
    ast->type = type;
    ast->child1 = child1;
    return ast;
}

ASTNode* astNew2(int type, ASTNode* child1, ASTNode* child2) {
    ASTNode* ast = astNew();
    ast->type = type;
    ast->child1 = child1;
    ast->child2 = child2;
    return ast;
}

ASTNode* astNew3(int type, ASTNode* child1, ASTNode* child2, ASTNode* child3) {
    ASTNode* ast = astNew();
    ast->type = type;
    ast->child1 = child1;
    ast->child2 = child2;
    ast->child3 = child3;
    return ast;
}

ASTNode* astNew4(
    int type,
    ASTNode* child1,
    ASTNode* child2,
    ASTNode* child3,
    ASTNode* child4) {
    ASTNode* ast = astNew();
    ast->type = type;
    ast->child1 = child1;
    ast->child2 = child2;
    ast->child3 = child3;
    ast->child4 = child4;
    return ast;
}

void astFree(ASTNode* ast) {
    if (ast->tokenStr != NULL)
        free(ast->tokenStr);
    else if (ast->child1 != NULL) {
        astFree(ast->child1);
        if (ast->child2 != NULL) {
            astFree(ast->child2);
            if (ast->child3 != NULL) {
                astFree(ast->child3);
                if (ast->child4 != NULL)
                    astFree(ast->child4);
            }
        }
    }
    free(ast);
}
