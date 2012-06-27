#include <stdlib.h>
#include <string.h>
#include "ast.h"

AST* astNew() {
    AST* ast = (AST*)malloc(sizeof(AST));
    memset(ast, 0, sizeof(AST));
    return ast;
}

AST* astNewStr(int type, const char* tokenStr) {
    AST* ast = astNew();
    ast->type = type;
    ast->tokenStr = (char*)malloc(strlen(tokenStr) + 1);
    memcpy(ast->tokenStr, tokenStr, strlen(tokenStr) + 1);
    return ast;
}

AST* astNew0(int type) {
    AST* ast = astNew();
    ast->type = type;
    return ast;
}

AST* astNew1(int type, AST* child1) {
    AST* ast = astNew();
    ast->type = type;
    ast->child1 = child1;
    return ast;
}

AST* astNew2(int type, AST* child1, AST* child2) {
    AST* ast = astNew();
    ast->type = type;
    ast->child1 = child1;
    ast->child2 = child2;
    return ast;
}

AST* astNew3(int type, AST* child1, AST* child2, AST* child3) {
    AST* ast = astNew();
    ast->type = type;
    ast->child1 = child1;
    ast->child2 = child2;
    ast->child3 = child3;
    return ast;
}

AST* astNew4(int type, AST* child1, AST* child2, AST* child3, AST* child4) {
    AST* ast = astNew();
    ast->type = type;
    ast->child1 = child1;
    ast->child2 = child2;
    ast->child3 = child3;
    ast->child4 = child4;
    return ast;
}

void astFree(AST* ast) {
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
