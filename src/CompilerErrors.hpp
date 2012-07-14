#ifndef COMPILER_ERRORS_HPP_INCLUDED
#define COMPILER_ERRORS_HPP_INCLUDED

#include <iostream>
#include <string>
#include "grammar/ASTNode.h"

/**
 * A class for emitting user-facing compiler errors.
 */
class CompilerErrors {
private:
    /**
     * The ostream to which to output compiler errors.
     */
    std::ostream* output;
    /**
     * The filename of the source file.  This is only used in the message we
     * output when printing compiler errors.  We do not actually read the file.
     */
    std::string filename;
    /**
     * Whether we called "emitError".
     */
    bool hasEmittedError;
public:
    CompilerErrors(std::ostream& output2, std::string filename2);
    /**
     * Outputs a user-facing compiler error.
     * 
     * Compilation does not immediately terminate upon calling this method.
     * Callsites should proceed with suitable substitutes for the erroneous
     * conditions (with the understanding that we will discard the CFGFile we
     * ultimately produce), so that we can detect additional compiler errors
     * without compiling the source file again.  For example, if a
     * "compileExpression" method encounters a multiplication of non-numeric
     * values, it should call "emitError", because such types cannot be added.
     * Then, it might return a value like "new CFGOperand(0)", so that we can
     * continue compilation.
     * 
     * @param node the node at which the error appears.
     * @param error the text of the error.
     */
    void emitError(ASTNode* node, std::string error);
    bool getHasEmittedError();
};

#endif
