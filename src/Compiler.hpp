#ifndef COMPILER_HPP_INCLUDED
#define COMPILER_HPP_INCLUDED

#include <iostream>
#include "grammar/ASTNode.h"

class CFGFile;

/**
 * Returns an (unoptimized) CFG representation of the specified AST.  Returns
 * NULL if there was a compiler error.
 * @param node the node.
 * @param errorOutput an ostream to which to output compiler errors.
 */
CFGFile* compileFile(ASTNode* node, std::ostream& errorOutput);

#endif
