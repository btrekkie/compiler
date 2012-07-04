#ifndef PARSER_HPP_INCLUDED
#define PARSER_HPP_INCLUDED

#include <stdio.h>
#include <string>
#include "grammar/ASTNode.h"

/**
 * Parses a source file into an AST.
 */
class Parser {
public:
    /**
     * Returns the AST representation of the specified source file.
     */
    static ASTNode* parseFile(std::string filename);
};

#endif
