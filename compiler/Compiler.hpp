#ifndef COMPILER_HPP_INCLUDED
#define COMPILER_HPP_INCLUDED

#include "grammar/ASTNode.h"

class CFGFile;

/**
 * Returns an (unoptimized) CFG representation of the specified AST.
 */
CFGFile* compileFile(ASTNode* node);

#endif
