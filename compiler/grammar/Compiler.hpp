#ifndef COMPILER_HPP_INCLUDED
#define COMPILER_HPP_INCLUDED

#include "ASTNode.h"

/**
 * Compiles the specified AST into C++ code, and writes the results to cout.
 */
void processFile(ASTNode* node);

#endif
