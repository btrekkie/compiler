#ifndef AST_UTIL_HPP_INCLUDED
#define AST_UTIL_HPP_INCLUDED

#include "grammar/ASTNode.h"

/**
 * Provides static utility methods pertaining to an AST.
 */
class ASTUtil {
public:
    /**
     * Returns the integer literal value indicated by the specified node of type
     * AST_INT_LITERAL.
     */
    static int getIntLiteralValue(ASTNode* node);
};

#endif
