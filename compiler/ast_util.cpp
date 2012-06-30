#include <assert.h>
#include <stdlib.h>
#include "ast_util.hpp"

int ASTUtil::getIntLiteralValue(ASTNode* node) {
    assert(node->type == AST_INT_LITERAL || !"Not an integer literal node");
    return atoi(node->tokenStr);
}
