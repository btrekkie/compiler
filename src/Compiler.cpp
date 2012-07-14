/* We perform compilation using a top-down approach, proceeding from the root of
 * the AST to its leaves.  We perform compilation and semantic checking at the
 * same time.  Compilation is a rather stateful process, with variables
 * containing information about our current point in compilation.
 */

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include "grammar/ASTNode.h"
#include "grammar/Compiler.hpp"
#include "ASTUtil.hpp"
#include "BreakEvaluator.hpp"
#include "CFG.hpp"
#include "CompilerErrors.hpp"
#include "CPPCompiler.hpp"
#include "Interface.hpp"
#include "Universe.hpp"
#include "UniverseSet.hpp"
#include "VarResolver.hpp"

using namespace std;

/**
 * A class for converting an AST representation of a program into an
 * (unoptimized) CFG representation.
 */
/* In the below comments, unless otherwise indicated, the term "compile" refers
 * to appending a list of CFGStatements indicating the compiled representation
 * of an AST node to "statements", emitting any compiler errors we encounter in
 * the process.
 */
class Compiler {
private:
    /**
     * A map from the identifiers of the available methods to their interfaces.
     * TODO (classes) include methods from other classes
     * TODO support method overloading
     */
    map<string, MethodInterface*> methodInterfaces;
    /**
     * A vector to which to append the compiled statements.
     */
    vector<CFGStatement*> statements;
    /**
     * A map from nodes of type AST_IDENTIFIER to integers identifying those
     * variables.  See the comments for VarResolver::resolveVars for more
     * information.
     */
    map<ASTNode*, int> varIDs;
    /**
     * A map from the local variable ids in varIDs to the CFGOperands for those
     * variables.
     */
    map<int, CFGOperand*> varIDToOperand;
    /**
     * A set of the identifiers of the class's fields.
     */
    set<string> fieldIdentifiers;
    /**
     * A map from the identifiers of the class's fields to the CFGOperands for
     * those fields.
     */
    map<string, CFGOperand*> fieldVars;
    /**
     * A map from the identifiers of the arguments to the method we are
     * currently compiling to the CFGOperands for those variables.
     */
    map<string, CFGOperand*> argVars;
    /**
     * A BreakEvaluator maintaining compiler state pertaining to control flow
     * statements in the current method, if any: break statements, continue
     * statements, and return statements.
     */
    BreakEvaluator* breakEvaluator;
    /**
     * The CompilerErrors object we are using to emit compiler errors.
     */
    CompilerErrors* errors;
    /**
     * The universe for the UniverseSets indicating initialized variables.
     */
    Universe<CFGOperand*>* varUniverse;
    /**
     * A set of all of the local (non-field) variables, excluding method
     * arguments, that have necessarily been initialized at the current point in
     * compilation.  We use the field in order to emit the proper compiler
     * errors about using uninitialized variables.  (If the current point is
     * unreachable, this is the set of initialized variables in the nearest
     * parent branch that is reachable.  It is the empty set if there is no such
     * branch.)
     */
    UniverseSet<CFGOperand*>* allInitializedVars;
    /**
     * A stack of sets of local (non-field) variables, excluding method
     * arguments, that have necessarily been initialized in the branches of
     * compilation in which the current point of compilation resides, exculding
     * those initialized in a parent branch.  We use the field in order to emit
     * the proper compiler errors about using uninitialized variables.  Each
     * entry of the stack corresponds to an "if", "else", or "case" branch, or,
     * what amounts to the same thing, a "for" or "while" loop.  A NULL entry
     * indicates that the point in the stack contains unreachable code.  For
     * example, the following method indicates the state of the stack
     * immediately after executing the lines of code in a sample method.
     * 
     * void foo(Int i) {
     *     i++;              [{i}]
     *     while (i < 5) {   
     *         Int j = 0;    [{i}, {j}]
     *         Int k;        [{i}, {j}]
     *         Int l;        [{i}, {j}]
     *         if (i == 3) { 
     *             j = 1;    [{i}, {j}, {}]
     *             k = 2;    [{i}, {j}, {k}]
     *         } else {      
     *             k = 3;    [{i}, {j}, {k}]
     *             l = 4;    [{i}, {j}, {k, l}]
     *         }             
     *         println(i);   [{i}, {j, k}]
     *         return;       [{i}, NULL]
     *         if (i == 2)   
     *             k = 3;    [{i}, NULL, NULL]
     *     }                 
     *     println(i);       [{i}]
     * }
     */
    vector<UniverseSet<CFGOperand*>*> initializedVarsStack;
    /**
     * A map from the continue and break labels in "breakEvaluator" to sets of
     * all the local (non-field) variables, including method arguments, that
     * were necessarily initialized at the (reachable) break and continue
     * statements that target those labels.  We use the field in order to emit
     * the proper compiler errors about using uninitialized variables.  The map
     * does not contain any empty or NULL values, and the vectors do not contain
     * any NULL values.
     * 
     * For example, say there are two (reachable) break statements that break to
     * a label "label".  Say that at the point of the first break statement, the
     * variables "i" and "j" were initialized, and at the point of the second
     * break statement, the variables "i" and "k" were initialized.  Then
     * "incomingInitializedVars" will have a mapping from "label" to
     * [{i, j}, {i, k}].
     */
    map<CFGLabel*, vector<UniverseSet<CFGOperand*>*>*> incomingInitializedVars;
    
    void emitError(ASTNode* node, string error) {
        errors->emitError(node, error);
    }
    
    /**
     * Returns the CFGOperand for the specified node of type AST_IDENTIFIER.
     * Emits a compiler error if the variable is not declared.
     * @param node the node.
     * @param mustBeInitialized whether to emit a compiler error if the variable
     *     is not initialized.  This should be true if the variable is used in
     *     the right-hand side of an expression.
     * @return the CFGOperand.
     */
    CFGOperand* getVarOperand(ASTNode* node, bool mustBeInitialized) {
        assert(node->type == AST_IDENTIFIER || !"Not a variable");
        assert(
            varIDs.count(node) > 0 ||
            !"Missing variable id.  Probably a bug in VarResolver.");
        string identifier = node->tokenStr;
        int id = varIDs[node];
        if (id >= 0) {
            assert(
                varIDToOperand.count(id) > 0 ||
                "Attempting to get variable before creating it.  Possibly a "
                "bug in VarResolver.");
            CFGOperand* var = varIDToOperand[id];
            if (mustBeInitialized &&
                !allInitializedVars->contains(var) &&
                initializedVarsStack.back() != NULL)
                emitError(
                    node,
                    "Variable may be used before it is initialized");
            return var;
        } else if (argVars.count(identifier) > 0)
            return argVars[identifier];
        else if (fieldVars.count(identifier) > 0)
            return argVars[identifier];
        else
            return new CFGOperand(new CFGType("Object"));
    }
    
    /**
     * Creates and returns a new CFGOperand for a variable.
     * @param type the type of the variable.
     * @param node the node of type AST_IDENTIFIER in which the variable is
     *     declared.
     * @param isField whether the variable is a class field.
     * @return the operand.
     */
    CFGOperand* createVar(
        ASTNode* node,
        CFGType* type,
        bool isField = false,
        bool isArg = false) {
        assert(node->type == AST_IDENTIFIER || !"Not a variable node");
        CFGOperand* var = new CFGOperand(type, node->tokenStr, isField);
        if (isArg)
            argVars[var->getIdentifier()] = var;
        else if (isField) {
            fieldVars[var->getIdentifier()] = var;
            fieldIdentifiers.insert(var->getIdentifier());
        } else {
            assert(
                (varIDs.count(node) > 0 && varIDs[node] >= 0) ||
                !"Missing variable id.  Probably a bug in VarResolver.");
            varIDToOperand[varIDs[node]] = var;
        }
        return var;
    }
    
    /**
     * Pushes an entry onto "initializedVarsStack".  See the comments for that
     * field for more information.
     */
    void pushInitializedVarsBranch() {
        if (!initializedVarsStack.empty() &&
            initializedVarsStack.back() == NULL)
            initializedVarsStack.push_back(NULL);
        else
            initializedVarsStack.push_back(
                new UniverseSet<CFGOperand*>(varUniverse));
    }
    
    /**
     * Pops an entry from "initializedVarsStack", removing the variables the
     * discarded entry indicates from "allInitializedVars".  See the comments
     * for that field for more information.  Because this method removes values
     * from "allInitializedVars", it is only suitable for cases in which none of
     * those variables should be preserved, like "if" statements and unlike "if-
     * else" statements.
     */
    void popInitializedVarsBranch() {
        UniverseSet<CFGOperand*>* initializedVars = initializedVarsStack.back();
        initializedVarsStack.pop_back();
        if (initializedVars != NULL) {
            allInitializedVars->difference(initializedVars);
            delete initializedVars;
        }
    }
    
    /**
     * Alters "allInitializedVars" and "initializedVarsStack" to indicate that
     * the specified local variable has been initialized at the current point of
     * compilation.  Assumes that "operand" is a local variable or field in the
     * source file.  See the comments for "allInitializedVars" for more
     * information.
     */
    void markVarInitialized(CFGOperand* operand) {
        assert(
            operand->getIdentifier() != "" ||
            !"Operand must be a variable from the source code");
        if (!operand->getIsField() &&
            !allInitializedVars->contains(operand) &&
            initializedVarsStack.back() != NULL) {
            allInitializedVars->add(operand);
            initializedVarsStack.back()->add(operand);
        }
    }
    
    /**
     * Removes all of the sets from incomingInitializedVars[label] and
     * intersects them with initializedVarsStack.back().  In other words, this
     * updates "initializedVarsStack" and "allInitializedVars" to account for
     * the (reachable) break or continue statements that target the specified
     * label.  See the comments for "incomingInitializedVars" for more
     * information.
     */
    void mergeIncomingInitializedVars(CFGLabel* label) {
        if (incomingInitializedVars.count(label)) {
            vector<UniverseSet<CFGOperand*>*>* incomingVars =
                incomingInitializedVars[label];
            if (initializedVarsStack.back() == NULL) {
                initializedVarsStack.pop_back();
                initializedVarsStack.push_back(
                    new UniverseSet<CFGOperand*>(*(incomingVars->front())));
            }
            allInitializedVars->difference(initializedVarsStack.back());
            for (vector<UniverseSet<CFGOperand*>*>::const_iterator iterator =
                     incomingVars->begin();
                 iterator != incomingVars->end();
                 iterator++) {
                initializedVarsStack.back()->intersect(*iterator);
                delete *iterator;
            }
            delete incomingVars;
            allInitializedVars->unionWith(initializedVarsStack.back());
            incomingInitializedVars.erase(label);
        }
    }
    
    /**
     * Emits an error if a variable of type "destinationType" may not store a
     * value of type "sourceType".
     * @param node the node at which such an error would appear.
     * @param destinationType the destination type.
     * @param sourceType the source type.
     */
    void assertAssignmentTypeValid(
        ASTNode* node,
        CFGType* destinationType,
        CFGType* sourceType) {
        
        // TODO classes
        if (destinationType->getClassName() == "Object" &&
            destinationType->getNumDimensions() == 0)
            return;
        if (destinationType->isNumeric() &&
            sourceType->isNumeric() &&
            !sourceType->isMorePromotedThan(destinationType))
            return;
        if (destinationType->getClassName() != sourceType->getClassName() ||
            destinationType->getNumDimensions() !=
                sourceType->getNumDimensions())
            emitError(node, "Incompatible types in assignment");
    }
    
    /**
     * Appends a statement assigning "destination" to be equal to "source".
     * Emits an error if "destination" may not store a value of that type.
     * @param node the node at which such an error would appear.
     * @param destination the destination variable.
     * @param source the source value.
     */
    void appendAssignmentStatement(
        ASTNode* node,
        CFGOperand* destination,
        CFGOperand* source) {
        
        assertAssignmentTypeValid(
            node,
            destination->getType(),
            source->getType());
        statements.push_back(
            new CFGStatement(CFG_ASSIGN, destination, source));
    }
    
    /**
     * Returns the type of elements in arrays of the specified type.  If
     * "arrayType" does not indicate an array, returns some substitute type.
     */
    CFGType* attemptGetArrayElementType(CFGType* arrayType) {
        return new CFGType(
            arrayType->getClassName(),
            max(arrayType->getNumDimensions() - 1, 0));
    }
    
    /**
     * Emits a compiler error if "array" is not an array or "index" is not of
     * the array index type.
     * @param node the node at which such an error would appear.
     * @param array the candidate array.
     * @param index the candidate array index.
     * @return false iff we emitted an error.
     */
    bool assertValidArrayElement(
        ASTNode* node,
        CFGOperand* array,
        CFGOperand* index) {
        if (array->getType()->getNumDimensions() == 0) {
            emitError(node, "Operand must be an array");
            return false;
        }
        CFGType* intType = new CFGType("Int");
        if (index->getType()->isIntegerLike() &&
            !index->getType()->isMorePromotedThan(intType)) {
            delete intType;
            return true;
        } else {
            emitError(node, "Array index must be an integer");
            delete intType;
            return false;
        }
    }
    
    /**
     * Appends a statement retrieving the specified index of the specified
     * array.  If "array" is not an array or "index" is not of the array index
     * type, performs a suitable alternative.
     * @param array the array.
     * @param index the index.
     * @return a variable indicating the result of the operation.
     */
    CFGOperand* attemptAppendArrayGet(CFGOperand* array, CFGOperand* index) {
        if (array->getType()->getNumDimensions() == 0)
            return array;
        CFGType* intType = new CFGType("Int");
        if (!index->getType()->isIntegerLike() ||
            index->getType()->isMorePromotedThan(intType))
            index = new CFGOperand(0);
        delete intType;
        CFGOperand* destination = new CFGOperand(
            attemptGetArrayElementType(array->getType()));
        statements.push_back(
            new CFGStatement(CFG_ARRAY_GET, destination, array, index));
        return destination;
    }
    
    /**
     * Compiles the specified node of type AST_[PRE|POST]_[INCREMENT|DECREMENT].
     * @param node the node to compile.
     * @return a CFGOperand storing the results of the expression.
     */
    CFGOperand* compileIncrementExpression(ASTNode* node) {
        if (node->child1->type != AST_IDENTIFIER &&
            node->child1->type != AST_ARRAY_GET) {
            emitError(
                node,
                "Increment / decrement operator may only be used on variables "
                "or array elements");
            return compileExpression(node->child1);
        }
        CFGOperand* operand;
        CFGOperand* array = NULL;
        CFGOperand* index = NULL;
        if (node->child1->type == AST_IDENTIFIER)
            operand = getVarOperand(node->child1, true);
        else {
            array = compileExpression(node->child1->child1);
            index = compileExpression(node->child1->child2);
            bool isValid = assertValidArrayElement(node->child1, array, index);
            operand = attemptAppendArrayGet(array, index);
            if (!isValid)
                array = NULL;
        }
        if (!operand->getType()->isNumeric()) {
            emitError(
                node,
                "Increment / decrement operator may only be used on numbers");
            return operand;
        }
        CFGOperand* destination;
        if (node->child1->type == AST_IDENTIFIER)
            destination = operand;
        else
            destination = new CFGOperand(operand->getType());
        CFGOperand* expressionResult;
        switch (node->type) {
            case AST_POST_DECREMENT:
            case AST_POST_INCREMENT:
                expressionResult = new CFGOperand(operand->getType());
                appendAssignmentStatement(node, expressionResult, operand);
                statements.push_back(
                    new CFGStatement(
                        node->type == AST_POST_INCREMENT ? CFG_PLUS : CFG_MINUS,
                        destination,
                        operand,
                        CFGOperand::one()));
                break;
            case AST_PRE_DECREMENT:
            case AST_PRE_INCREMENT:
                statements.push_back(
                    new CFGStatement(
                        node->type == AST_PRE_INCREMENT ? CFG_PLUS : CFG_MINUS,
                        destination,
                        operand,
                        CFGOperand::one()));
                expressionResult = destination;
                break;
            default:
                assert(!"Unhandled increment type");
                return NULL;
        }
        if (array != NULL)
            statements.push_back(
                new CFGStatement(CFG_ARRAY_SET, array, index, destination));
        return expressionResult;
    }
    
    /**
     * Compiles statements causing us to jump to "trueLabel" if the expression
     * indicated by the specified node evaluates to true and to "falseLabel"
     * otherwise.
     * 
     * Of course, we could simply use "compileExpression" to achieve the same
     * effect, but sometimes "createConditionalJump" can avoid the use of
     * temporary boolean variables.  For example, "createConditionalJump" would
     * compile "if (x && y) foo(); else bar();" to the following:
     * 
     *     if (x) goto intermediateLabel; else goto barLabel;
     * intermediateLabel:
     *     if (y) goto fooLabel; else goto barLabel;
     * 
     * By contrast, if we were to use "compileExpression", we would get the
     * following:
     * 
     *     if (x) goto intermediateLabel; else goto setToFalseLabel;
     * intermediateLabel:
     *     if (y) goto setToTrueLabel; else goto setToFalseLabel;
     * setToTrueLabel:
     *     temp = true;
     *     goto checkTemp;
     * setToFalseLabel:
     *     temp = false;
     * checkTemp:
     *     if (temp) goto fooLabel; else goto barLabel;
     */
    void compileConditionalJump(
        ASTNode* node,
        CFGLabel* trueLabel,
        CFGLabel* falseLabel) {
        switch (node->type) {
            case AST_BOOLEAN_AND:
            {
                CFGLabel* intermediateLabel = new CFGLabel();
                compileConditionalJump(
                    node->child1,
                    intermediateLabel,
                    falseLabel);
                statements.push_back(
                    CFGStatement::fromLabel(intermediateLabel));
                pushInitializedVarsBranch();
                compileConditionalJump(node->child2, trueLabel, falseLabel);
                popInitializedVarsBranch();
                break;
            }
            case AST_BOOLEAN_OR:
            {
                CFGLabel* intermediateLabel = new CFGLabel();
                compileConditionalJump(
                    node->child1,
                    trueLabel,
                    intermediateLabel);
                statements.push_back(
                    CFGStatement::fromLabel(intermediateLabel));
                pushInitializedVarsBranch();
                compileConditionalJump(node->child2, trueLabel, falseLabel);
                popInitializedVarsBranch();
                break;
            }
            case AST_FALSE:
                statements.push_back(CFGStatement::jump(falseLabel));
                break;
            case AST_NOT:
                compileConditionalJump(node->child1, falseLabel, trueLabel);
                break;
            case AST_TRUE:
                statements.push_back(CFGStatement::jump(trueLabel));
                break;
            default:
            {
                CFGOperand* operand = compileExpression(node);
                if (!operand->getType()->isBool()) {
                    emitError(node, "Boolean value is required");
                    break;
                }
                CFGStatement* statement = new CFGStatement(
                    CFG_IF,
                    NULL,
                    operand);
                vector<CFGOperand*> switchValues;
                vector<CFGLabel*> switchLabels;
                switchValues.push_back(CFGOperand::fromBool(true));
                switchLabels.push_back(trueLabel);
                switchValues.push_back(NULL);
                switchLabels.push_back(falseLabel);
                statement->setSwitchValuesAndLabels(switchValues, switchLabels);
                statements.push_back(statement);
                break;
            }
        }
    }
    
    /**
     * Returns the CFG_* constant indicating the operation performed by the
     * specified AST_*_ASSIGN operation.
     */
    CFGOperation opForAssignmentType(int type) {
        switch (type) {
            case AST_AND_ASSIGN:
                return CFG_BITWISE_AND;
            case AST_DIV_ASSIGN:
                return CFG_DIV;
            case AST_LEFT_SHIFT_ASSIGN:
                return CFG_LEFT_SHIFT;
            case AST_MINUS_ASSIGN:
                return CFG_MINUS;
            case AST_MOD_ASSIGN:
                return CFG_MOD;
            case AST_MULT_ASSIGN:
                return CFG_MULT;
            case AST_OR_ASSIGN:
                return CFG_BITWISE_OR;
            case AST_PLUS_ASSIGN:
                return CFG_PLUS;
            case AST_RIGHT_SHIFT_ASSIGN:
                return CFG_RIGHT_SHIFT;
            case AST_UNSIGNED_RIGHT_SHIFT_ASSIGN:
                return CFG_UNSIGNED_RIGHT_SHIFT;
            case AST_XOR_ASSIGN:
                return CFG_XOR;
            default:
                assert(!"Unhandled assignment type");
        }
    }
    
    /**
     * Returns the CFG_* constant corresponding to the specified AST_*
     * expression operation.
     */
    CFGOperation opForExpressionType(int type) {
        switch (type) {
            case AST_BITWISE_AND:
                return CFG_BITWISE_AND;
            case AST_BITWISE_INVERT:
                return CFG_BITWISE_INVERT;
            case AST_BITWISE_OR:
                return CFG_BITWISE_OR;
            case AST_DIV:
                return CFG_DIV;
            case AST_EQUALS:
                return CFG_EQUALS;
            case AST_LEFT_SHIFT:
                return CFG_LEFT_SHIFT;
            case AST_GREATER_THAN:
                return CFG_GREATER_THAN;
            case AST_GREATER_THAN_OR_EQUAL_TO:
                return CFG_GREATER_THAN_OR_EQUAL_TO;
            case AST_LESS_THAN:
                return CFG_LESS_THAN;
            case AST_LESS_THAN_OR_EQUAL_TO:
                return CFG_LESS_THAN_OR_EQUAL_TO;
            case AST_MINUS:
                return CFG_MINUS;
            case AST_MOD:
                return CFG_MOD;
            case AST_MULT:
                return CFG_MULT;
            case AST_NEGATE:
                return CFG_NEGATE;
            case AST_NOT:
                return CFG_NOT;
            case AST_NOT_EQUALS:
                return CFG_NOT_EQUALS;
            case AST_PLUS:
                return CFG_PLUS;
            case AST_RIGHT_SHIFT:
                return CFG_RIGHT_SHIFT;
            case AST_UNSIGNED_RIGHT_SHIFT:
                return CFG_UNSIGNED_RIGHT_SHIFT;
            case AST_XOR:
                return CFG_XOR;
            default:
                assert(!"Unhandled expression type");
        }
    }
    
    /**
     * Appends a statement for a two-argument arithmetic expression.  Emits an
     * error if the operands' types are not compatible with the operation.
     * @param node the node that is evaluating the expression.
     * @param operation the CFG_* constant indicating the arithmetic expression
     *     to perform.
     * @param source1 the first argument to the expression.
     * @param source2 the second argument to the expression.
     * @return a CFGOperand storing the results of the operation.
     */
    CFGOperand* appendBinaryArithmeticExpression(
        ASTNode* node,
        CFGOperation operation,
        CFGOperand* source1,
        CFGOperand* source2) {
        if (!source1->getType()->isNumeric() ||
            !source2->getType()->isNumeric()) {
            emitError(node, "Operands to arithmetic operator must be numbers");
            return source1;
        }
        
        CFGType* destinationType;
        switch (operation) {
            case CFG_BITWISE_AND:
            case CFG_BITWISE_OR:
            case CFG_XOR:
                if (!source1->getType()->isIntegerLike() ||
                    !source2->getType()->isIntegerLike()) {
                    emitError(node, "Operand must be of an integer-like type");
                    return source1;
                }
                else if (source1->getType()->isMorePromotedThan(
                             source2->getType()))
                    destinationType = source1->getType();
                else
                    destinationType = source2->getType();
                break;
            case CFG_LEFT_SHIFT:
            case CFG_RIGHT_SHIFT:
            case CFG_UNSIGNED_RIGHT_SHIFT:
                if (!source1->getType()->isIntegerLike()) {
                    emitError(node, "Operand must be of an integer-like type");
                    return source1;
                } else if (source2->getType()->getNumDimensions() > 0 ||
                           (source2->getType()->getClassName() != "Int" &&
                            source2->getType()->getClassName() != "Byte")) {
                    emitError(
                        node,
                        "Operand to bit shift must be integer or byte");
                    return source1;
                }
                destinationType = source1->getType();
                break;
            case CFG_MOD:
                if (!source1->getType()->isIntegerLike() ||
                    !source2->getType()->isIntegerLike()) {
                    emitError(node, "Operand must be of an integer-like type");
                    return source1;
                }
                if (source1->getType()->isMorePromotedThan(source2->getType()))
                    destinationType = source1->getType();
                else
                    destinationType = source2->getType();
                break;
            default:
                if (source1->getType()->isMorePromotedThan(source2->getType()))
                    destinationType = source1->getType();
                else
                    destinationType = source2->getType();
                break;
        }
        CFGOperand* destination = new CFGOperand(destinationType);
        statements.push_back(
            new CFGStatement(operation, destination, source1, source2));
        return destination;
    }
    
    /**
     * Returns the type that is the lowest common ancestor of "type1" and
     * "type2".  To put it another way, returns the type of the expression
     * "foo() ? objectOfType1 : objectOfType2".
     */
    CFGType* getLeastCommonType(CFGType* type1, CFGType* type2) {
        if (type1->getNumDimensions() == type2->getNumDimensions() &&
            type1->getClassName() == type2->getClassName())
            return new CFGType(type1);
        else if (type1->getNumDimensions() > 0 || type2->getNumDimensions() > 0)
            return new CFGType("Object");
        else if (type1->isNumeric() && type2->isNumeric()) {
            if (type1->isMorePromotedThan(type2))
                return new CFGType(type1);
            else
                return new CFGType(type2);
        } else
            return new CFGType("Object");
    }
    
    /**
     * Returns a CFGOperand for the specified node indicating a literal value.
     */
    CFGOperand* getOperandForLiteral(ASTNode* node) {
        switch (node->type) {
            case AST_FALSE:
                return CFGOperand::fromBool(false);
            case AST_FLOAT_LITERAL:
            {
                string str = node->tokenStr;
                char lastChar = str.at(str.length() - 1);
                if (lastChar == 'f' || lastChar == 'F')
                    return new CFGOperand(
                        (float)atof(str.substr(0, str.length() - 1).c_str()));
                else
                    return new CFGOperand(strtod(node->tokenStr, NULL));
            }
            case AST_INT_LITERAL:
            {
                string str = node->tokenStr;
                char lastChar = str.at(str.length() - 1);
                bool isLong = lastChar == 'l' || lastChar == 'L';
                long long value;
                if (ASTUtil::getIntLiteralValue(str, value)) {
                    if (isLong)
                        return new CFGOperand(value);
                    else
                        return new CFGOperand((int)value);
                } else if (isLong) {
                    emitError(
                        node,
                        "Literal value is too large for Long data type");
                    return new CFGOperand(0LL);
                } else {
                    emitError(
                        node,
                        "Literal value is too large for Int data type");
                    return new CFGOperand(0);
                }
            }
            case AST_TRUE:
                return CFGOperand::fromBool(true);
            default:
                assert(!"Unhandled literal type");
        }
        return NULL;
    }
    
    /**
     * Compiles a node for performing a "mathematical" operation.
     * @param node the node to compile.
     * @return a CFGOperand storing the results of the expression.
     */
    CFGOperand* compileMathExpression(ASTNode* node) {
        switch (node->type) {
            case AST_BITWISE_AND:
            case AST_BITWISE_OR:
            case AST_DIV:
            case AST_LEFT_SHIFT:
            case AST_MINUS:
            case AST_MOD:
            case AST_MULT:
            case AST_PLUS:
            case AST_RIGHT_SHIFT:
            case AST_UNSIGNED_RIGHT_SHIFT:
            case AST_XOR:
            {
                CFGOperand* source1 = compileExpression(node->child1);
                CFGOperand* source2 = compileExpression(node->child2);
                return appendBinaryArithmeticExpression(
                    node,
                    opForExpressionType(node->type),
                    source1,
                    source2);
            }
            case AST_BITWISE_INVERT:
            {
                CFGOperand* operand = compileExpression(node->child1);
                if (!operand->getType()->isIntegerLike()) {
                    emitError(node, "Operand must be of an integer-like type");
                    return operand;
                }
                CFGOperand* destination = new CFGOperand(operand->getType());
                statements.push_back(
                    new CFGStatement(CFG_BITWISE_INVERT, destination, operand));
                return destination;
            }
            case AST_GREATER_THAN:
            case AST_GREATER_THAN_OR_EQUAL_TO:
            case AST_LESS_THAN:
            case AST_LESS_THAN_OR_EQUAL_TO:
            {
                CFGOperand* source1 = compileExpression(node->child1);
                if (!source1->getType()->isNumeric())
                    emitError(node, "Operand must be a number");
                CFGOperand* source2 = compileExpression(node->child2);
                if (!source2->getType()->isNumeric())
                    emitError(node, "Operand must be a number");
                CFGOperand* destination = new CFGOperand(CFGType::boolType());
                if (source1->getType()->isNumeric() &&
                    source2->getType()->isNumeric())
                    statements.push_back(
                        new CFGStatement(
                            opForExpressionType(node->type),
                            destination,
                            source1,
                        source2));
                else
                    return CFGOperand::fromBool(false);
                return destination;
            }
            case AST_NEGATE:
            {
                CFGOperand* operand = compileExpression(node->child1);
                if (!operand->getType()->isNumeric()) {
                    emitError(node, "Operand must be a number");
                    return operand;
                }
                CFGOperand* destination = new CFGOperand(operand->getType());
                statements.push_back(
                    new CFGStatement(CFG_NEGATE, destination, operand));
                return destination;
            }
            default:
                assert(!"Unhandled math expression");
        }
        return NULL;
    }
    
    /**
     * Compiles a node for performing a boolean-related operation.
     * @param node the node to compile.
     * @return a CFGOperand storing the results of the expression.
     */
    CFGOperand* compileBooleanExpression(ASTNode* node) {
        switch (node->type) {
            case AST_BOOLEAN_AND:
            case AST_BOOLEAN_OR:
            {
                CFGOperand* destination = new CFGOperand(CFGType::boolType());
                CFGLabel* trueLabel = new CFGLabel();
                CFGLabel* falseLabel = new CFGLabel();
                CFGLabel* endLabel = new CFGLabel();
                compileConditionalJump(node, trueLabel, falseLabel);
                statements.push_back(CFGStatement::fromLabel(trueLabel));
                statements.push_back(
                    new CFGStatement(
                        CFG_ASSIGN,
                        destination,
                        CFGOperand::fromBool(true)));
                statements.push_back(CFGStatement::jump(endLabel));
                statements.push_back(CFGStatement::fromLabel(falseLabel));
                statements.push_back(
                    new CFGStatement(
                        CFG_ASSIGN,
                        destination,
                        CFGOperand::fromBool(false)));
                statements.push_back(CFGStatement::fromLabel(endLabel));
                return destination;
            }
            case AST_EQUALS:
            case AST_NOT_EQUALS:
            {
                // TODO (classes) type checking
                CFGOperand* source1 = compileExpression(node->child1);
                CFGOperand* source2 = compileExpression(node->child2);
                CFGOperand* destination = new CFGOperand(CFGType::boolType());
                statements.push_back(
                    new CFGStatement(
                        opForExpressionType(node->type),
                        destination,
                        source1,
                        source2));
                return destination;
            }
            case AST_NOT:
            {
                CFGOperand* operand = compileExpression(node->child1);
                if (!operand->getType()->isBool()) {
                    emitError(node, "Operand must be a boolean");
                    return CFGOperand::fromBool(false);
                }
                CFGOperand* destination = new CFGOperand(CFGType::boolType());
                statements.push_back(
                    new CFGStatement(
                        opForExpressionType(node->type),
                        destination,
                        operand));
                return destination;
            }
            case AST_TERNARY:
            {
                CFGOperand* destination = new CFGOperand((CFGType*)NULL);
                CFGLabel* trueLabel = new CFGLabel();
                CFGLabel* falseLabel = new CFGLabel();
                CFGLabel* endLabel = new CFGLabel();
                compileConditionalJump(node->child1, trueLabel, falseLabel);
                statements.push_back(CFGStatement::fromLabel(trueLabel));
                CFGOperand* trueValue = compileExpression(node->child2);
                statements.push_back(
                    new CFGStatement(CFG_ASSIGN, destination, trueValue));
                statements.push_back(CFGStatement::jump(endLabel));
                statements.push_back(CFGStatement::fromLabel(falseLabel));
                CFGOperand* falseValue = compileExpression(node->child3);
                statements.push_back(
                    new CFGStatement(CFG_ASSIGN, destination, falseValue));
                statements.push_back(CFGStatement::fromLabel(endLabel));
                destination->setType(
                    getLeastCommonType(
                        trueValue->getType(),
                        falseValue->getType()));
                return destination;
            }
            default:
                assert(!"Unhanded boolean expression");
        }
        return NULL;
    }
    
    /**
     * Compiles the specified node of type AST_ASSIGNMENT_EXPRESSION.
     * @param node the node to compile.
     * @return a CFGOperand storing the results of the expression.
     */
    CFGOperand* compileAssignmentExpression(ASTNode* node) {
        assert(
            node->type == AST_ASSIGNMENT_EXPRESSION ||
                !"Not an assignment expression");
        if (node->child1->type != AST_IDENTIFIER &&
            node->child1->type != AST_ARRAY_GET) {
            emitError(
                node,
                "Invalid left-hand side; must be a variable or an array "
                "element");
            compileExpression(node->child1);
            return compileExpression(node->child2);
        }
        CFGOperand* destination;
        CFGOperand* array = NULL;
        CFGOperand* index = NULL;
        if (node->child1->type == AST_IDENTIFIER)
            destination = getVarOperand(node->child1, false);
        else {
            array = compileExpression(node->child1->child1);
            index = compileExpression(node->child1->child2);
            bool isValid = assertValidArrayElement(node, array, index);
            if (isValid && node->child2->type != AST_ASSIGN)
                destination = attemptAppendArrayGet(array, index);
            else
                destination = new CFGOperand(
                    attemptGetArrayElementType(array->getType()));
            if (!isValid)
                array = NULL;
        }
        CFGOperand* source = compileExpression(node->child3);
        if (node->child2->type != AST_ASSIGN)
            source = appendBinaryArithmeticExpression(
                node,
                opForAssignmentType(node->child2->type),
                destination,
                source);
        appendAssignmentStatement(node, destination, source);
        if (node->child1->type == AST_IDENTIFIER)
            markVarInitialized(destination);
        else if (array != NULL)
            statements.push_back(
                new CFGStatement(CFG_ARRAY_SET, array, index, destination));
        return destination;
    }
    
    /**
     * Compiles the specified expression list node (the "expressionList" rule in
     * grammar.y) for the arguments to a method call.
     * @param node the node.
     * @param argTypes a vector of the method's arguments' compile-time types
     *     (in the order in which they are declared).
     * @param args a vector in which to store the arguments' values.  The vector
     *     is parallel to "argTypes".
     */
    void compileMethodCallArgList(
        ASTNode* node,
        vector<CFGType*>& argTypes,
        vector<CFGOperand*>& args) {
        bool alreadyReachedArgLimit = args.size() == argTypes.size();
        CFGOperand* arg;
        if (node->type != AST_EXPRESSION_LIST)
            arg = compileExpression(node);
        else {
            compileMethodCallArgList(node->child1, argTypes, args);
            arg = compileExpression(node->child2);
        }
        if (args.size() == argTypes.size()) {
            if (!alreadyReachedArgLimit) // Don't emit error multiple times
                emitError(node, "Too many arguments to method call");
        } else {
            assertAssignmentTypeValid(
                node,
                argTypes.at(args.size()),
                arg->getType());
            args.push_back(arg);
        }
    }
    
    /**
     * Compiles the specified method call node.
     * @param node the node.
     * @return a CFGOperand storing the results of the expression.  Returns NULL
     *     if it is a void method.
     */
    CFGOperand* compileMethodCall(ASTNode* node) {
        // TODO (classes) method calls on objects other than "this"
        string identifier = node->child1->tokenStr;
        if (methodInterfaces.count(identifier) == 0) {
            emitError(node, "Calling an unknown method");
            return new CFGOperand(0);
        }
        
        MethodInterface* interface = methodInterfaces[identifier];
        CFGOperand* destination;
        if (interface->getReturnType() != NULL)
            destination = new CFGOperand(
                new CFGType(interface->getReturnType()));
        else
            destination = NULL;
        CFGStatement* statement = new CFGStatement(
            CFG_METHOD_CALL,
            destination,
            NULL);
        vector<CFGType*> argTypes = interface->getArgTypes();
        vector<CFGOperand*> args;
        if (node->child2 != NULL)
            compileMethodCallArgList(node->child2, argTypes, args);
        if (args.size() < argTypes.size())
            emitError(node, "Too few arguments to method call");
        else {
            statement->setMethodIdentifierAndArgs(identifier, args);
            statements.push_back(statement);
        }
        return destination;
    }
    
    /**
     * Compiles the specified expression.
     * @param node the node to compile.
     * @return a CFGOperand storing the results of the expression.
     */
    CFGOperand* compileExpression(ASTNode* node) {
        switch (node->type) {
            case AST_ARRAY:
                assert(!"TODO array literals");
                return NULL;
            case AST_ARRAY_GET:
            {
                CFGOperand* array = compileExpression(node->child1);
                CFGOperand* index = compileExpression(node->child2);
                assertValidArrayElement(node, array, index);
                attemptAppendArrayGet(array, index);
                return NULL;
            }
            case AST_ASSIGNMENT_EXPRESSION:
                return compileAssignmentExpression(node);
            case AST_BITWISE_AND:
            case AST_BITWISE_INVERT:
            case AST_BITWISE_OR:
            case AST_DIV:
            case AST_GREATER_THAN:
            case AST_GREATER_THAN_OR_EQUAL_TO:
            case AST_LEFT_SHIFT:
            case AST_LESS_THAN:
            case AST_LESS_THAN_OR_EQUAL_TO:
            case AST_MINUS:
            case AST_MOD:
            case AST_MULT:
            case AST_NEGATE:
            case AST_PLUS:
            case AST_RIGHT_SHIFT:
            case AST_UNSIGNED_RIGHT_SHIFT:
            case AST_XOR:
                return compileMathExpression(node);
            case AST_BOOLEAN_AND:
            case AST_BOOLEAN_OR:
            case AST_EQUALS:
            case AST_NOT:
            case AST_NOT_EQUALS:
            case AST_TERNARY:
                return compileBooleanExpression(node);
            case AST_CAST:
            case AST_NEW:
            case AST_TARGETED_METHOD_CALL:
                assert(!"TODO classes");
                return NULL;
            case AST_FALSE:
            case AST_FLOAT_LITERAL:
            case AST_INT_LITERAL:
            case AST_TRUE:
                return getOperandForLiteral(node);
            case AST_IDENTIFIER:
                return getVarOperand(node, true);
            case AST_METHOD_CALL:
            {
                CFGOperand* destination = compileMethodCall(node);
                if (destination == NULL) {
                    emitError(
                        node,
                        "Cannot use the return value of void method");
                    return new CFGOperand(0);
                }
                return destination;
            }
            case AST_PARENTHESES:
                return compileExpression(node->child1);
            case AST_POST_DECREMENT:
            case AST_POST_INCREMENT:
            case AST_PRE_DECREMENT:
            case AST_PRE_INCREMENT:
                return compileIncrementExpression(node);
            case AST_QUALIFIED_IDENTIFIER:
                assert(!"TODO classes");
                return NULL;
            default:
                assert(!"Unhandled expression type");
        }
        return NULL;
    }
    
    /**
     * Compiles the specified variable declaration item node (the
     * "varDeclarationItem" rule in grammar.y).
     * @param node the node.
     * @param type the type of the variable being declared, or NULL if the field
     *     is of type "auto".
     * @param isField whether we are declaring a field, as opposed to a local
     *     variable.
     */
    void compileVarDeclarationItem(ASTNode* node, CFGType* type, bool isField) {
        if (node->type == AST_ASSIGNMENT_EXPRESSION) {
            CFGOperand* source = compileExpression(node->child3);
            if (type == NULL)
                type = source->getType();
            CFGOperand* destination = createVar(node->child1, type, isField);
            appendAssignmentStatement(node, destination, source);
            markVarInitialized(destination);
        } else {
            if (type == NULL) {
                emitError(
                    node,
                    "Usage of the auto type is limited to variables that are "
                    "assigned in their declaration statement");
                type = new CFGType("Int");
            }
            createVar(node, type, isField);
        }
    }
    
    /**
     * Compiles the specified variable declaration list node (the
     * "varDeclarationList" rule in grammar.y).
     * @param node the node.
     * @param type the type of the variable being declared.
     * @param isField whether we are declaring a field, as opposed to a local
     *     variable.
     */
    void compileVarDeclarationList(ASTNode* node, CFGType* type, bool isField) {
        if (node->type != AST_VAR_DECLARATION_LIST)
            compileVarDeclarationItem(node, type, isField);
        else {
            compileVarDeclarationList(node->child1, type, isField);
            compileVarDeclarationItem(node->child2, type, isField);
        }
    }
    
    /**
     * Returns the CFGType representation of the specified type node (the "type"
     * rule in grammar.y).
     * @param node the node.
     * @param allowAuto whether the "auto" type is permissible.  If so, this
     *     method returns NULL for the "auto" type.  If not, this method emits
     *     an error for the "auto" type.
     */
    CFGType* getCFGType(ASTNode* node, bool allowAuto) {
        if (node->type == AST_AUTO) {
            if (allowAuto)
                return NULL;
            else {
                emitError(
                    node,
                    "Usage of the auto type is limited to variables that are "
                    "assigned in their declaration statement");
                return CFGType::intType();
            }
        } else if (node->type == AST_TYPE_ARRAY) {
            CFGType* childType = getCFGType(node->child1, false);
            CFGType* type = new CFGType(
                childType->getClassName(),
                childType->getNumDimensions() + 1);
            delete childType;
            return type;
        } else {
            assert(node->type == AST_TYPE || !"Not a type node");
            assert(node->child1->type != AST_FIELD || !"TODO modules");
            if (node->child1->type != AST_IDENTIFIER)
                emitError(node, "Syntax error");
            return new CFGType(node->child1->tokenStr);
        }
    }
    
    /**
     * Compiles the specified loop node (a while or for loop).
     */
    void compileLoop(ASTNode* node) {
        CFGLabel* continueLabel = new CFGLabel();
        CFGLabel* endLabel = new CFGLabel();
        breakEvaluator->pushBreakLabel(endLabel);
        breakEvaluator->pushContinueLabel(continueLabel);
        switch (node->type) {
            case AST_DO_WHILE:
            {
                CFGLabel* startLabel = new CFGLabel();
                statements.push_back(CFGStatement::fromLabel(startLabel));
                compileStatement(node->child1);
                statements.push_back(CFGStatement::fromLabel(continueLabel));
                compileConditionalJump(node->child2, startLabel, endLabel);
                break;
            }
            case AST_FOR:
            {
                CFGLabel* startLabel = new CFGLabel();
                CFGLabel* bodyLabel = new CFGLabel();
                compileStatementList(node->child1);
                statements.push_back(CFGStatement::fromLabel(startLabel));
                compileConditionalJump(node->child2, bodyLabel, endLabel);
                statements.push_back(CFGStatement::fromLabel(bodyLabel));
                pushInitializedVarsBranch();
                compileStatement(node->child4);
                statements.push_back(CFGStatement::fromLabel(continueLabel));
                compileStatementList(node->child3);
                statements.push_back(CFGStatement::jump(startLabel));
                popInitializedVarsBranch();
                break;
            }
            case AST_FOR_IN:
            {
                CFGOperand* collection = compileExpression(node->child3);
                assert(
                    collection->getType()->getNumDimensions() > 0 ||
                    !"TODO classes");
                CFGLabel* startLabel = new CFGLabel();
                CFGLabel* bodyLabel = new CFGLabel();
                CFGOperand* index = new CFGOperand(new CFGType("Int"));
                CFGOperand* length = new CFGOperand(new CFGType("Int"));
                CFGOperand* anotherIteration = new CFGOperand(
                    new CFGType("Bool"));
                statements.push_back(
                    new CFGStatement(CFG_ARRAY_LENGTH, length, collection));
                statements.push_back(CFGStatement::fromLabel(startLabel));
                statements.push_back(
                    new CFGStatement(
                        CFG_LESS_THAN,
                        anotherIteration,
                        index,
                        length));
                
                CFGStatement* statement = new CFGStatement(
                    CFG_IF,
                    NULL,
                    anotherIteration);
                vector<CFGOperand*> switchValues;
                vector<CFGLabel*> switchLabels;
                switchValues.push_back(CFGOperand::fromBool(true));
                switchLabels.push_back(bodyLabel);
                switchValues.push_back(NULL);
                switchLabels.push_back(endLabel);
                statement->setSwitchValuesAndLabels(switchValues, switchLabels);
                statements.push_back(statement);
                statements.push_back(CFGStatement::fromLabel(bodyLabel));
                
                CFGType* arrayElementType = attemptGetArrayElementType(
                    collection->getType());
                CFGType* loopVarType = getCFGType(node->child1, true);
                if (loopVarType == NULL)
                    loopVarType = arrayElementType;
                CFGOperand* loopVar = createVar(node->child2, loopVarType);
                CFGOperand* element = new CFGOperand(arrayElementType);
                statements.push_back(
                    new CFGStatement(
                        CFG_ARRAY_GET,
                        element,
                        collection,
                        index));
                appendAssignmentStatement(node->child2, loopVar, element);
                
                pushInitializedVarsBranch();
                compileStatement(node->child4);
                statements.push_back(CFGStatement::fromLabel(continueLabel));
                statements.push_back(
                    new CFGStatement(
                        CFG_PLUS,
                        index,
                        index,
                        new CFGOperand(1)));
                statements.push_back(CFGStatement::jump(startLabel));
                popInitializedVarsBranch();
                break;
            }
            case AST_WHILE:
            {
                CFGLabel* bodyLabel = new CFGLabel();
                statements.push_back(CFGStatement::fromLabel(continueLabel));
                compileConditionalJump(node->child1, bodyLabel, endLabel);
                statements.push_back(CFGStatement::fromLabel(bodyLabel));
                pushInitializedVarsBranch();
                compileStatement(node->child2);
                statements.push_back(CFGStatement::jump(continueLabel));
                popInitializedVarsBranch();
                break;
            }
            default:
                assert(!"Unhandled loop type");
                break;
        }
        statements.push_back(CFGStatement::fromLabel(endLabel));
        breakEvaluator->popBreakLabel();
        mergeIncomingInitializedVars(endLabel);
        breakEvaluator->popContinueLabel();
        mergeIncomingInitializedVars(continueLabel);
    }
    
    /**
     * Compiles the specified case list node (the "caseList" rule in grammar.y).
     * @param node the node.
     * @param nextCaseLabelNode the node for the following case label in the
     *     switch statement, if any.
     * @param switchValues a vector to which to append the values for the case
     *     statements.  See the comments for CFGStatement.switchValues for more
     *     information.
     * @param switchLabels a vector to which to append the labels associated
     *     with the case statements.  See the comments for
     *     CFGStatement.switchValues for more information.
     * @param switchValueInts a set of the values for the case labels in the
     *     enclosing switch statement that we have encountered thus far.
     * @param haveEncounteredDefault whether we have encountered a derault label
     *     in the enclosing switch statement.
     */
    void compileCaseList(
        ASTNode* node,
        ASTNode* nextCaseLabelNode,
        vector<CFGOperand*>& switchValues,
        vector<CFGLabel*>& switchLabels,
        set<int>& switchValueInts,
        bool& haveEncounteredDefault) {
        if (node->type == AST_EMPTY_CASE_LIST)
            return;
        assert(node->type == AST_CASE_LIST || !"Not a case list");
        compileCaseList(
            node->child1,
            node->child2,
            switchValues,
            switchLabels,
            switchValueInts,
            haveEncounteredDefault);
        if (node->child2->type == AST_CASE_LABEL_DEFAULT) {
            if (haveEncounteredDefault)
                emitError(node, "Duplicate default label");
            haveEncounteredDefault = true;
            switchValues.push_back(NULL);
        } else {
            CFGOperand* value = getOperandForLiteral(node->child2->child1);
            assert(
                (value->getType()->getClassName() == "Int" &&
                 value->getType()->getNumDimensions() == 0) ||
                !"Type of integer literal must be Int");
            if (switchValueInts.count(value->getIntValue()) > 0)
                emitError(node, "Duplicate case label");
            switchValueInts.insert(value->getIntValue());
            switchValues.push_back(value);
        }
        
        if (node->child3->type != AST_EMPTY_STATEMENT_LIST &&
            nextCaseLabelNode != NULL &&
            !breakEvaluator->alwaysBreaks(node->child3))
            emitError(
                nextCaseLabelNode,
                "Falling through in a switch statement is not permitted.  "
                "Perhaps you are missing a break statement.");
        CFGLabel* label = new CFGLabel();
        switchLabels.push_back(label);
        statements.push_back(CFGStatement::fromLabel(label));
        if (node->child3->type != AST_EMPTY_STATEMENT_LIST) {
            pushInitializedVarsBranch();
            compileStatementList(node->child3);
            popInitializedVarsBranch();
        }
    }
    
    /**
     * Computes the number of loops out of which the specified AST_BREAK or
     * AST_CONTINUE node directs us to break or continue.
     * @param node the node.
     * @param numLoops a reference in which to store the number of loops.
     * @return whether this method called "emitError" due to a long literal
     *     argument.  In this case, the resulting value of "numLoops" is
     *     unspecified.
     */
    bool getNumJumpLoops(ASTNode* node, int& numLoops) {
        if (node->child1 == NULL) {
            numLoops = 1;
            return true;
        } else {
            CFGOperand* numLoopsOperand = getOperandForLiteral(
                node->child1);
            if (numLoopsOperand->getType()->getClassName() == "Long") {
                emitError(
                    node,
                    "Number of loops must be an integer literal, not a long "
                    "literal");
                delete numLoopsOperand;
                return false;
            } else {
                assert(
                    numLoopsOperand->getType()->getClassName() == "Int" ||
                    !"Unexpected literal type");
                numLoops = numLoopsOperand->getIntValue();
                delete numLoopsOperand;
                return true;
            }
        }
    }
    
    /**
     * Compiles the specified control flow statement node (a break, continue, or
     * return statement).
     */
    void compileControlFlowStatement(ASTNode* node) {
        CFGLabel* label;
        switch (node->type) {
            case AST_BREAK:
            {
                int numLoops;
                if (getNumJumpLoops(node, numLoops)) {
                    label = breakEvaluator->getBreakLabel(numLoops);
                    if (label == NULL) {
                        emitError(node, "Attempting to break out of non-loop");
                        return;
                    }
                }
                break;
            }
            case AST_CONTINUE:
            {
                int numLoops;
                if (getNumJumpLoops(node, numLoops)) {
                    label = breakEvaluator->getContinueLabel(numLoops);
                    if (label == NULL) {
                        emitError(node, "Attempting to continue non-loop");
                        return;
                    }
                }
                break;
            }
            case AST_RETURN:
                if (node->child1 != NULL) {
                    CFGOperand* operand = compileExpression(node->child1);
                    if (breakEvaluator->getReturnVar() == NULL)
                        emitError(
                            node,
                            "Cannot return a value from a void method");
                    else
                        appendAssignmentStatement(
                            node,
                            breakEvaluator->getReturnVar(),
                            operand);
                } else if (breakEvaluator->getReturnVar() != NULL)
                    emitError(node, "Must return a non-void value");
                label = breakEvaluator->getReturnLabel();
                break;
            default:
                assert(!"Unhanded control flow statement type");
        }
        statements.push_back(CFGStatement::jump(label));
        if (initializedVarsStack.back() != NULL) {
            if (node->type != AST_RETURN) {
                if (incomingInitializedVars.count(label) == 0)
                    incomingInitializedVars[label] =
                        new vector<UniverseSet<CFGOperand*>*>();
                incomingInitializedVars[label]->push_back(
                    new UniverseSet<CFGOperand*>(*allInitializedVars));
            }
            popInitializedVarsBranch();
            initializedVarsStack.push_back(NULL);
        }
    }
    
    /**
     * Compiles the specified selection statement node (an if or switch
     * statement).
     */
    void compileSelectionStatement(ASTNode* node) {
        switch (node->type) {
            case AST_IF:
            {
                CFGLabel* trueLabel = new CFGLabel();
                CFGLabel* falseLabel = new CFGLabel();
                compileConditionalJump(node->child1, trueLabel, falseLabel);
                statements.push_back(CFGStatement::fromLabel(trueLabel));
                compileStatement(node->child2);
                statements.push_back(CFGStatement::fromLabel(falseLabel));
                break;
            }
            case AST_IF_ELSE:
            {
                CFGLabel* trueLabel = new CFGLabel();
                CFGLabel* falseLabel = new CFGLabel();
                CFGLabel* finishLabel = new CFGLabel();
                compileConditionalJump(node->child1, trueLabel, falseLabel);
                
                pushInitializedVarsBranch();
                statements.push_back(CFGStatement::fromLabel(trueLabel));
                compileStatement(node->child2);
                statements.push_back(CFGStatement::jump(finishLabel));
                UniverseSet<CFGOperand*>* trueInitializedVars =
                    initializedVarsStack.back();
                initializedVarsStack.pop_back();
                if (trueInitializedVars != NULL)
                    allInitializedVars->difference(trueInitializedVars);
                
                pushInitializedVarsBranch();
                statements.push_back(CFGStatement::fromLabel(falseLabel));
                compileStatement(node->child3);
                statements.push_back(CFGStatement::fromLabel(finishLabel));
                UniverseSet<CFGOperand*>* falseInitializedVars =
                    initializedVarsStack.back();
                initializedVarsStack.pop_back();
                if (falseInitializedVars != NULL)
                    allInitializedVars->difference(falseInitializedVars);
                
                if (trueInitializedVars == NULL &&
                    falseInitializedVars == NULL) {
                    popInitializedVarsBranch();
                    initializedVarsStack.push_back(NULL);
                } else {
                    // initializedVars' =
                    //     initializedVars |
                    //     (trueInitializedVars & falseInitializedVars) =
                    //     (initializedVars | trueInitializedVars |
                    //      falseInitializedVars) &
                    //     (trueInitializedVars & falseInitializedVars)
                    if (trueInitializedVars != NULL)
                        initializedVarsStack.back()->unionWith(
                            trueInitializedVars);
                    if (falseInitializedVars != NULL)
                        initializedVarsStack.back()->unionWith(
                            falseInitializedVars);
                    if (trueInitializedVars != NULL) {
                        initializedVarsStack.back()->intersect(
                            trueInitializedVars);
                        delete trueInitializedVars;
                    }
                    if (falseInitializedVars != NULL) {
                        initializedVarsStack.back()->intersect(
                            falseInitializedVars);
                        delete falseInitializedVars;
                    }
                }
                break;
            }
            case AST_SWITCH:
            {
                CFGLabel* finishLabel = new CFGLabel();
                breakEvaluator->pushBreakLabel(finishLabel);
                CFGOperand* operand = compileExpression(node->child1);
                if (!operand->getType()->isIntegerLike())
                    emitError(node, "Operand must be of an integer-like type");
                CFGStatement* statement = new CFGStatement(
                    CFG_SWITCH,
                    NULL,
                    operand);
                statements.push_back(statement);
                vector<CFGOperand*> switchValues;
                vector<CFGLabel*> switchLabels;
                set<int> switchValueInts;
                bool haveEncounteredDefault = false;
                compileCaseList(
                    node->child2,
                    NULL,
                    switchValues,
                    switchLabels,
                    switchValueInts,
                    haveEncounteredDefault);
                if (!haveEncounteredDefault) {
                    switchValues.push_back(NULL);
                    switchLabels.push_back(finishLabel);
                } else if (incomingInitializedVars.count(finishLabel))
                    // initializedVars' =
                    //     initializedVars |
                    //     (caseVars1 & caseVars2 & ... & caseVarsN) =
                    //     (initializedVars | caseVars1) &
                    //     (caseVars1 & caseVars2 & ... & caseVarsN)
                    // The intersection with caseVars occurs in the subsequent
                    // call to "mergeIncomingInitializedVars".
                    initializedVarsStack.back()->unionWith(
                        incomingInitializedVars[finishLabel]->front());
                else {
                    popInitializedVarsBranch();
                    initializedVarsStack.push_back(NULL);
                }
                statement->setSwitchValuesAndLabels(switchValues, switchLabels);
                statements.push_back(CFGStatement::fromLabel(finishLabel));
                breakEvaluator->popBreakLabel();
                mergeIncomingInitializedVars(finishLabel);
                break;
            }
            default:
                assert(!"Unhanded selection statement type");
        }
    }
    
    /**
     * Compiles the specified statement node (the "statement" rule in
     * grammar.y).
     */
    void compileStatement(ASTNode* node) {
        switch (node->type) {
            case AST_ASSIGNMENT_EXPRESSION:
                compileExpression(node);
                break;
            case AST_BLOCK:
                compileStatementList(node->child1);
                break;
            case AST_BREAK:
            case AST_CONTINUE:
            case AST_RETURN:
                compileControlFlowStatement(node);
                break;
            case AST_DO_WHILE:
            case AST_FOR:
            case AST_FOR_IN:
            case AST_WHILE:
                compileLoop(node);
                break;
            case AST_EMPTY_STATEMENT:
                break;
            case AST_IF:
            case AST_IF_ELSE:
            case AST_SWITCH:
                compileSelectionStatement(node);
                break;
            case AST_METHOD_CALL:
                compileMethodCall(node);
                break;
            case AST_POST_DECREMENT:
            case AST_POST_INCREMENT:
            case AST_PRE_DECREMENT:
            case AST_PRE_INCREMENT:
                compileIncrementExpression(node);
                break;
            case AST_VAR_DECLARATION:
                compileVarDeclarationList(
                    node->child2,
                    getCFGType(node->child1, true),
                    false);
                break;
            default:
                assert(!"Unhandled statement type");
                break;
        }
    }
    
    /**
     * Compiles the specified statement list node (the "statementList" rule in
     * grammar.y).
     */
    void compileStatementList(ASTNode* node) {
        if (node->type != AST_EMPTY_STATEMENT_LIST) {
            compileStatementList(node->child1);
            compileStatement(node->child2);
        }
    }
    
    /**
     * Creates and returns a CFGOperand for the argument indicated by the
     * specified argument item node (the "argItem" rule in grammar.y).
     */
    CFGOperand* createArgItemVar(ASTNode* node) {
        assert(node->child3 == NULL || !"TODO default arguments");
        return createVar(
            node->child2,
            getCFGType(node->child1, false),
            false,
            true);
    }
    
    /**
     * Creates CFGOperands for the arguments indicated by the specified argument
     * list node (the "argList" rule in grammar.y) and appends them to "args".
     */
    void createArgListVars(ASTNode* node, vector<CFGOperand*>& args) {
        if (node->type != AST_ARG_LIST)
            args.push_back(createArgItemVar(node));
        else {
            createArgListVars(node->child1, args);
            args.push_back(createArgItemVar(node->child2));
        }
    }
    
    /**
     * Returns the compiled CFGMethod representation of the specified node of
     * type AST_METHOD_DEFINITION.  (Assumes that all of the class's fields are
     * already available in "allVars".)
     */
    CFGMethod* compileMethodDefinition(ASTNode* node) {
        string identifier = node->child2->tokenStr;
        varUniverse = new Universe<CFGOperand*>();
        allInitializedVars = new UniverseSet<CFGOperand*>(varUniverse);
        pushInitializedVarsBranch();
        argVars.clear();
        vector<CFGOperand*> args;
        if (node->child4 != NULL)
            createArgListVars(node->child3, args);
        varIDs = VarResolver::resolveVars(node, fieldIdentifiers, errors);
        CFGOperand* returnVar;
        if (node->child1->type == AST_VOID)
            returnVar = NULL;
        else {
            CFGType* type = getCFGType(node->child1, false);
            returnVar = new CFGOperand(type);
        }
        CFGLabel* returnLabel = new CFGLabel();
        breakEvaluator = new BreakEvaluator(returnVar, returnLabel);
        statements.clear();
        
        if (node->child4 != NULL)
            compileStatementList(node->child4);
        else
            compileStatementList(node->child3);
        
        if (returnVar != NULL && initializedVarsStack.back() != NULL)
            // Initialized vars are NULL.  Thus, the end of the method (right
            // before the return label) is reachable.  Thus, we might not have a
            // return value.
            emitError(node, "Method may finish without returning a value");
        popInitializedVarsBranch();
        statements.push_back(CFGStatement::fromLabel(returnLabel));
        delete breakEvaluator;
        breakEvaluator = NULL;
        delete varUniverse;
        varUniverse = NULL;
        delete allInitializedVars;
        allInitializedVars = NULL;
        return new CFGMethod(
            node->child2->tokenStr,
            returnVar,
            args,
            statements);
    }
    
    /**
     * Appends the types of the arguments indicated by the specified argument
     * list node (the "argList" rule in grammar.y) to "argTypes".
     */
    void getArgTypes(ASTNode* node, vector<CFGType*>& argTypes) {
        // TODO default arguments
        if (node->type != AST_ARG_LIST)
            argTypes.push_back(getCFGType(node->child1, false));
        else {
            getArgTypes(node->child1, argTypes);
            argTypes.push_back(getCFGType(node->child2->child1, false));
        }
    }
    
    /**
     * Adds entries to "methodInterfaces" for the built-in methods available to
     * every class.
     * TODO do these methods need to be built-in?  Can we make them ordinary
     * methods?
     */
    void getBuiltInMethodInterfaces() {
        vector<CFGType*> argTypes;
        argTypes.push_back(new CFGType("Object"));
        methodInterfaces["print"] = new MethodInterface(
            NULL,
            argTypes,
            "print");
        argTypes.clear();
        argTypes.push_back(new CFGType("Object"));
        methodInterfaces["println"] = new MethodInterface(
            NULL,
            argTypes,
            "println");
    }
    
    /**
     * Adds entries to "methodInterfaces" for the method definitions in the
     * specified class body item list node (the "classBodyItemList" rule in
     * grammar.y).
     */
    void getMethodInterfaces(ASTNode* node) {
        if (node->type == AST_EMPTY_CLASS_BODY_ITEM_LIST)
            return;
        getMethodInterfaces(node->child1);
        if (node->child2->type != AST_METHOD_DEFINITION)
            return;
        CFGType* returnType;
        if (node->child2->child1->type != AST_VOID)
            returnType = getCFGType(node->child2->child1, false);
        else
            returnType = NULL;
        vector<CFGType*> argTypes;
        if (node->child2->child4 != NULL)
            getArgTypes(node->child2->child3, argTypes);
        string identifier = node->child2->child2->tokenStr;
        if (methodInterfaces.count(identifier) > 0)
            assert(!"TODO method overloading");
        methodInterfaces[identifier] = new MethodInterface(
            returnType,
            argTypes,
            identifier);
    }
    
    /**
     * Adds the fields declared in the specified class body item list node (the
     * "classBodyItemList" rule in grammar.y) to "allVars" and "frameVars".
     */
    void compileFieldDeclarations(ASTNode* node) {
        if (node->type == AST_EMPTY_CLASS_BODY_ITEM_LIST)
            return;
        compileFieldDeclarations(node->child1);
        if (node->child2->type == AST_VAR_DECLARATION)
            compileVarDeclarationList(
                node->child2->child2,
                getCFGType(node->child2->child1, true),
                true);
    }
    
    /**
     * Appends the compiled CFGMethod representations of the methods defined in
     * the specified class body item list node (the "classBodyItemList" rule in
     * grammar.y) to "methods".
     */
    void compileMethodDefinitions(ASTNode* node, vector<CFGMethod*>& methods) {
        if (node->type != AST_EMPTY_CLASS_BODY_ITEM_LIST) {
            compileMethodDefinitions(node->child1, methods);
            if (node->child2->type == AST_METHOD_DEFINITION)
                methods.push_back(compileMethodDefinition(node->child2));
        }
    }
    
    /**
     * Returns the compiled CFGMethod representation of the specified node of
     * type AST_CLASS_DEFINITION.
     */
    CFGClass* compileClass(ASTNode* node) {
        getBuiltInMethodInterfaces();
        getMethodInterfaces(node->child2);
        statements.clear();
        compileFieldDeclarations(node->child2);
        vector<CFGStatement*> initStatements = statements;
        vector<CFGMethod*> methods;
        compileMethodDefinitions(node->child2, methods);
        
        for (map<string, MethodInterface*>::const_iterator iterator =
                 methodInterfaces.begin();
             iterator != methodInterfaces.end();
             iterator++)
            delete iterator->second;
        methodInterfaces.clear();
        
        return new CFGClass(
            node->child1->tokenStr,
            fieldVars,
            methods,
            initStatements);
    }
public:
    Compiler() {
        breakEvaluator = NULL;
    }
    
    /**
     * Returns an (unoptimized) CFG representation of the specified AST.
     */
    CFGFile* compileFile(
        ASTNode* node,
        string filename,
        ostream& errorOutput) {
        errors = new CompilerErrors(errorOutput, filename);
        CFGFile* file = new CFGFile(compileClass(node->child1));
        bool hasEmittedError = errors->getHasEmittedError();
        delete errors;
        if (!hasEmittedError)
            return file;
        else {
            delete file;
            return NULL;
        }
    }
};

CFGFile* compileFile(ASTNode* node, string filename, ostream& errorOutput) {
    Compiler compiler;
    return compiler.compileFile(node, filename, errorOutput);
}
