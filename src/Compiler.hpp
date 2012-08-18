#ifndef COMPILER_HPP_INCLUDED
#define COMPILER_HPP_INCLUDED

#include <iostream>
#include <string>
#include "grammar/ASTNode.h"

class CFGFile;

/**
 * Returns an (unoptimized) CFG representation of the specified AST.  Returns
 * NULL if there was a compiler error.
 * @param node the node.
 * @param filename the filename of the source file.  This is only used when
 *     printing compiler errors.  We do not actually read the file.
 * @param errorOutput an ostream to which to output compiler errors.
 */
CFGFile* compileFile(
    ASTNode* node,
    std::wstring filename,
    std::wostream& errorOutput);

#endif
