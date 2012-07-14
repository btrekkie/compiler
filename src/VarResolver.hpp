#ifndef VAR_RESOLVER_HPP_INCLUDED
#define VAR_RESOLVER_HPP_INCLUDED

#include <map>
#include <set>
#include <string>
#include "grammar/ASTNode.h"

class CompilerErrors;

/**
 * A class for assigning unique ids to variable in the source file.  See
 * "resolveVars".
 */
class VarResolver {
public:
    /**
     * Returns a map from the descendants of "node" that indicate local
     * variables to integers uniquely identifying those variables.  Basically,
     * this method is responsible for using the language's scoping rules to
     * resolve each variable identifier in the source file.  The returned map
     * includes a mapping for every node of type AST_IDENTIFIER that indicates a
     * variable (that is, for nodes of type AST_IDENTIFIER other than method
     * names and class names).  The values are non-negative integers in the case
     * of local variables and -1 in the case of fields, arguments, and
     * undeclared variables.
     * 
     * The method is responsible for emitting the relevant "multiple variables /
     * arguments with the same identifier" and "undefined variable" compiler
     * errors.  The mapping for variable declaration nodes is never -1, even in
     * the case of the former type of error.
     * 
     * @param node the node of type AST_METHOD_DEFINITION whose identifiers we
     *     are resolving.
     * @param fieldIdentifiers the identifiers of the class's fields.
     * @param errors the CompilerErrors object to use to emit compiler errors.
     */
    static std::map<ASTNode*, int> resolveVars(
        ASTNode* node,
        std::set<std::string>& fieldIdentifiers,
        CompilerErrors* errors);
};

#endif
