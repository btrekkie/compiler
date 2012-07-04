#ifndef COMPILER_HPP_INCLUDED
#define COMPILER_HPP_INCLUDED

#include "ASTNode.h"

/**
 * Responds to having finished parsing a source file into the specified parse
 * tree.
 */
void processFile(ASTNode* node);

#endif
