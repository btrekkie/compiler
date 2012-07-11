#ifndef PARSER_HPP_INCLUDED
#define PARSER_HPP_INCLUDED

#include <iostream>
#include <string>
#include "grammar/ASTNode.h"

/**
 * Parses a source file into an AST.
 */
class Parser {
public:
    /**
     * Returns the AST representation of the specified source file.  Returns
     * NULL if there is a parser (or lexer) error.
     * @param filename the filename of the source file.
     * @param errorOutput an ostream to which to output parser (and lexer)
     *     errors.
     */
    static ASTNode* parseFile(std::string filename, std::ostream& errorOutput);
};

#endif
