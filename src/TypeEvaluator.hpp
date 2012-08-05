#ifndef TYPE_EVALUATOR_HPP_INCLUDED
#define TYPE_EVALUATOR_HPP_INCLUDED

#include <map>
#include <string>
#include "grammar/ASTNode.h"
#include "CFG.hpp"

class CFGType;
class CompilerErrors;
class TypeEvaluatorImpl;

/**
 * A class responsible for computing the type of each expression in a method's
 * body.  Note that each local variable may assume different types throughout
 * the course of a method.
 * 
 * The language requires each expression to have a particular compile-time type.
 * For example, consider the following method:
 * 
 * void foo(Bool condition) {
 *     var foo = 1;
 *     if (condition)
 *         foo += 2;
 *     else
 *         foo += 2.5;
 *     println(foo);
 * }
 * 
 * After executing the first statement, "foo" is of type Int.  The reference to
 * "foo" in the last statement must have a particular compile-time type.  So it
 * must be a Double, even if "condition" is true.  A "runtime type"
 * interpretation, which would have "foo" be an Int if "condition" is true and
 * a Double if it is false, is incorrect.
 * 
 * The type of an expression is the most restrictive type consistent with fixed
 * compile-time expression types.  (Hence, the type of "foo" on the last
 * statement is Double, not Object.)
 */
class TypeEvaluator {
private:
    /**
     * The TypeEvaluatorImpl object used to implement this class's methods.
     */
    TypeEvaluatorImpl* impl;
public:
    TypeEvaluator();
    ~TypeEvaluator();
    /**
     * Computes the types of each of the expressions in the specified node of
     * type AST_METHOD_DEFINITION.  We must call this method before any other,
     * and we may only call it once.
     * 
     * "evaluateTypes" is responsible for emitting the relevant type errors:
     * "operand / condition value is of incorrect type", "using uninitialized
     * variable" (since "uninitialized" is essentially a variable type), and
     * "incorrect types in assignment".
     * 
     * @param node the node.
     * @param fieldTypes a map from the identifiers of the enclosing class's
     *     fields to their types.
     * @param varIDs a map from nodes of type AST_IDENTIFIER to integers
     *     identifying those variables.  See the comments for
     *     VarResolver::resolveVars for more information.
     * @param methodInterfaces a map from the identifiers of the methods
     *     available in the enclosing class to their interfaces.
     * @param errors the CompilerErrors object to use to emit compiler errors.
     */
    void evaluateTypes(
        ASTNode* node,
        std::map<std::string, CFGType*>& fieldTypes,
        std::map<ASTNode*, int>& varIDs,
        std::map<std::string, MethodInterface*>& methodInterfaces,
        CompilerErrors* errors);
    /**
     * Returns the reduced type of the specified expression node.  Assumes the
     * node indicates an expression and is a descendant of the node passed to
     * "evaluateTypes".
     */
    CFGReducedType getExpressionType(ASTNode* node);
};

#endif
