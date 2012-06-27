#ifndef COMPILER_HPP_INCLUDED
#define COMPILER_HPP_INCLUDED

extern "C" {
    #include "ast.h"
}

/**
 * Compiles the specified AST into C++ code, and writes the results to cout.
 */
void processFile(AST* node);

#endif
