/* We perform compilation using a top-down approach, proceeding from the root of
 * the AST to its leaves.  We perform compilation and error checking at the same
 * time.  Compilation is a rather stateful process, with variables containing
 * information about our current point in compilation.
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
#include "StringUtil.hpp"
#include "TypeEvaluator.hpp"
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
    map<wstring, MethodInterface*> methodInterfaces;
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
     * A map from reduced type and the local variable ids in varIDs to the
     * CFGOperands for those variables.  Note that a given local variable may
     * assume different types throughout the course of a method, as in the
     * following example:
     * 
     * void foo() {
     *     var foo = 1;
     *     foo += 2.5;
     * }
     * 
     * In the first statement, the identifier "foo" refers to Int operand for
     * "foo".  In the second statement, it refers to the Double operand.
     * 
     * To account for promotion, as in the above example, every time we set a
     * variable's value, we set the value for each of the promoted types of the
     * variable.  The example method produces the following statements:
     * 
     * CFG_ASSIGN foo_int 1
     * CFG_ASSIGN foo_float 1
     * CFG_ASSIGN foo_double 1
     * CFG_PLUS foo_double foo_double 2.5
     * 
     * Note that in the example, we set the value for "foo_float", even though
     * we do not use the variable.  This is not a problem for performance,
     * because dead code elimination will eliminate the statement that sets
     * "foo_float".  The post-optimization code is precisely what we want.
     */
    map<CFGReducedType, map<int, CFGOperand*>*> varIDToOperands;
    /**
     * A map from the identifiers of the class's fields to the CFGOperands for
     * those fields.
     */
    map<wstring, CFGOperand*> fieldVars;
    /**
     * A map from the identifiers of the class's fields to their types.
     */
    map<wstring, CFGType*> fieldTypes;
    /**
     * A set of the identifiers of the class's fields.
     */
    set<wstring> fieldIdentifiers;
    /**
     * A map from the identifiers of the arguments to the method we are
     * currently compiling to the CFGOperands for those variables.
     */
    map<wstring, CFGOperand*> argVars;
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
     * The TypeEvaluator object we are using to determine the types of the
     * expressions in the method we are currently compiling.
     */
    TypeEvaluator* typeEvaluator;
    
    void emitError(ASTNode* node, wstring error) {
        errors->emitError(node, error);
    }
    
    /**
     * Returns the CFGOperand for the specified variable node of type
     * AST_IDENTIFIER, using the specified type.
     * @param node the node.
     * @param type the type of the variable value to return.  Note that each
     *     variable corresponds to multiple CFGOperands of different types.  See
     *     the comments for "varIDToOperands" for more information.
     * @return the CFGOperand.
     */
    CFGOperand* getVarOperand(ASTNode* node, CFGReducedType type) {
        assert(node->type == AST_IDENTIFIER || !L"Not a variable node");
        assert(
            varIDs.count(node) > 0 ||
            !L"Missing variable id.  Probably a bug in VarResolver.");
        wstring identifier = node->tokenStr;
        int varID = varIDs[node];
        if (varID >= 0) {
            map<int, CFGOperand*>* vars;
            if (varIDToOperands.count(type) > 0)
                vars = varIDToOperands[type];
            else {
                vars = new map<int, CFGOperand*>();
                varIDToOperands[type] = vars;
            }
            if (vars->count(varID) > 0)
                return (*vars)[varID];
            else {
                CFGOperand* var = new CFGOperand(type, node->tokenStr, false);
                (*vars)[varID] = var;
                return var;
            }
        } else if (argVars.count(identifier) > 0)
            return argVars[identifier];
        else if (fieldVars.count(identifier) > 0)
            return fieldVars[identifier];
        else
            return new CFGOperand(REDUCED_TYPE_OBJECT);
    }
    
    /**
     * Returns the CFGOperand for the specified variable node of type
     * AST_IDENTIFIER, using the type of the variable at the current point of
     * compilation.  Note that each variable corresponds to multiple CFGOperands
     * of different types.  See the comments for "varIDToOperands" for more
     * information.
     */
    CFGOperand* getVarOperand(ASTNode* node) {
        assert(node->type == AST_IDENTIFIER || !L"Not a variable node");
        return getVarOperand(node, typeEvaluator->getExpressionType(node));
    }
    
    /**
     * Appends a statement setting the promoted CFGOperand for the specified
     * variable node of type AST_IDENTIFIER.  For example, we might set the
     * Double value of a variable "foo" to be equal to its current Int value.
     * See the comments for "varIDToOperands" for more information.
     * @param node the node.
     * @param type the type of the variable from which to draw the value.
     * @param promotedType the type of the variable to set.
     */
    void setPromotedVarOperand(
        ASTNode* node,
        CFGReducedType type,
        CFGReducedType promotedType) {
        CFGOperand* source = getVarOperand(node, type);
        CFGOperand* destination = getVarOperand(node, promotedType);
        statements.push_back(new CFGStatement(CFG_ASSIGN, destination, source));
    }
    
    /**
     * Appends statements setting the promoted CFGOperands for the specified
     * variable node of type AST_IDENTIFIER.  For example, if the type of a
     * variable "foo" is Int at the current point in compilation, we will set
     * the Float and Double values of the variable to be equal to its current
     * Int value.  See the comments for "varIDToOperands" for more information.
     */
    void setPromotedVarOperands(ASTNode* node) {
        assert(node->type == AST_IDENTIFIER || !L"Not a variable node");
        assert(
            varIDs.count(node) > 0 ||
            !L"Missing variable id.  Probably a bug in VarResolver.");
        int varID = varIDs[node];
        if (varID >= 0) {
            CFGReducedType type = typeEvaluator->getExpressionType(node);
            bool promote = false;
            if (type == REDUCED_TYPE_BYTE) {
                setPromotedVarOperand(node, type, REDUCED_TYPE_INT);
                promote = true;
            }
            if (type == REDUCED_TYPE_INT || promote) {
                setPromotedVarOperand(node, type, REDUCED_TYPE_LONG);
                promote = true;
            }
            if (type == REDUCED_TYPE_LONG || promote) {
                setPromotedVarOperand(node, type, REDUCED_TYPE_FLOAT);
                promote = true;
            }
            if (type == REDUCED_TYPE_FLOAT || promote)
                setPromotedVarOperand(node, type, REDUCED_TYPE_DOUBLE);
        }
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
                L"Increment / decrement operator may only be used on variables "
                L"or array elements");
            return compileExpression(node->child1);
        }
        CFGOperand* operand;
        CFGOperand* array = NULL;
        CFGOperand* index = NULL;
        if (node->child1->type == AST_IDENTIFIER)
            operand = getVarOperand(node->child1);
        else {
            array = compileExpression(node->child1->child1);
            index = compileExpression(node->child1->child2);
            operand = new CFGOperand(
                typeEvaluator->getExpressionType(node->child1));
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
                statements.push_back(
                    new CFGStatement(CFG_ASSIGN, expressionResult, operand));
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
                assert(!L"Unhandled increment type");
                return NULL;
        }
        if (node->child1->type == AST_IDENTIFIER)
            setPromotedVarOperands(node->child1);
        else
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
                compileConditionalJump(node->child2, trueLabel, falseLabel);
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
                compileConditionalJump(node->child2, trueLabel, falseLabel);
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
                assert(!L"Unhandled assignment type");
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
                assert(!L"Unhandled expression type");
        }
    }
    
    /**
     * Returns an integer indicating the relative level of promotion of the
     * specified type.  A greater number indicates a more promoted type.
     * Assumes that it is a numeric type.  See the comments for
     * CFGType.isMorePromotedThan for more information regarding promotion.
     */
    int getPromotionLevel(CFGReducedType type) {
        switch (type) {
            case REDUCED_TYPE_BYTE:
                return 1;
            case REDUCED_TYPE_INT:
                return 2;
            case REDUCED_TYPE_LONG:
                return 3;
            case REDUCED_TYPE_FLOAT:
                return 4;
            case REDUCED_TYPE_DOUBLE:
                return 5;
            default:
                assert(!L"Unhandled numeric type");
        }
        return 0;
    }
    
    /**
     * Returns the strongest type to which both "type1" and "type2" can be
     * promoted.  In other words, this returns the type of the ternary
     * expression "condition ? type1 : type2".
     */
    CFGReducedType getLeastCommonType(
        CFGReducedType type1,
        CFGReducedType type2) {
        if (type1 == type2)
            return type1;
        else if (type1 == REDUCED_TYPE_BOOL || type1 == REDUCED_TYPE_OBJECT ||
                 type2 == REDUCED_TYPE_BOOL || type2 == REDUCED_TYPE_OBJECT)
            return REDUCED_TYPE_OBJECT;
        else if (getPromotionLevel(type1) > getPromotionLevel(type2))
            return type1;
        else
            return type2;
    }
    
    /**
     * Appends a statement for a two-argument arithmetic expression.
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
        CFGOperand* destination = new CFGOperand(
            getLeastCommonType(source1->getType(), source2->getType()));
        statements.push_back(
            new CFGStatement(operation, destination, source1, source2));
        return destination;
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
                wstring str = node->tokenStr;
                wchar_t lastChar = str.at(str.length() - 1);
                if (lastChar == L'f' || lastChar == L'F')
                    return new CFGOperand(
                        (float)atof(
                            StringUtil::asciiWstringToString(
                                str.substr(0, str.length() - 1)).c_str()));
                else
                    return new CFGOperand(
                        strtod(
                            StringUtil::asciiWstringToString(
                                node->tokenStr).c_str(),
                            NULL));
            }
            case AST_INT_LITERAL:
            {
                wstring str = node->tokenStr;
                wchar_t lastChar = str.at(str.length() - 1);
                bool isLong = lastChar == L'l' || lastChar == L'L';
                long long value;
                if (ASTUtil::getIntLiteralValue(str, value)) {
                    if (isLong)
                        return new CFGOperand(value);
                    else
                        return new CFGOperand((int)value);
                } else if (isLong) {
                    emitError(
                        node,
                        L"Literal value is too large for Long data type");
                    return new CFGOperand(0LL);
                } else {
                    emitError(
                        node,
                        L"Literal value is too large for Int data type");
                    return new CFGOperand(0);
                }
            }
            case AST_TRUE:
                return CFGOperand::fromBool(true);
            default:
                assert(!L"Unhandled literal type");
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
                CFGOperand* source2 = compileExpression(node->child2);
                CFGOperand* destination = new CFGOperand(REDUCED_TYPE_BOOL);
                statements.push_back(
                    new CFGStatement(
                        opForExpressionType(node->type),
                        destination,
                        source1,
                        source2));
                return destination;
            }
            case AST_NEGATE:
            {
                CFGOperand* operand = compileExpression(node->child1);
                CFGOperand* destination = new CFGOperand(operand->getType());
                statements.push_back(
                    new CFGStatement(CFG_NEGATE, destination, operand));
                return destination;
            }
            default:
                assert(!L"Unhandled math expression");
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
                CFGOperand* destination = new CFGOperand(REDUCED_TYPE_BOOL);
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
                CFGOperand* source1 = compileExpression(node->child1);
                CFGOperand* source2 = compileExpression(node->child2);
                CFGOperand* destination = new CFGOperand(REDUCED_TYPE_BOOL);
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
                CFGOperand* destination = new CFGOperand(REDUCED_TYPE_BOOL);
                statements.push_back(
                    new CFGStatement(
                        opForExpressionType(node->type),
                        destination,
                        operand));
                return destination;
            }
            case AST_TERNARY:
            {
                CFGLabel* trueLabel = new CFGLabel();
                CFGLabel* falseLabel = new CFGLabel();
                CFGLabel* endLabel = new CFGLabel();
                CFGOperand* destination = new CFGOperand(
                    typeEvaluator->getExpressionType(node));
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
                return destination;
            }
            default:
                assert(!L"Unhanded boolean expression");
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
                !L"Not an assignment expression");
        if (node->child1->type != AST_IDENTIFIER &&
            node->child1->type != AST_ARRAY_GET) {
            emitError(
                node,
                L"Invalid left-hand side; must be a variable or an array "
                L"element");
            compileExpression(node->child1);
            return compileExpression(node->child2);
        }
        CFGOperand* destination;
        CFGOperand* array = NULL;
        CFGOperand* index = NULL;
        if (node->child1->type == AST_IDENTIFIER)
            destination = getVarOperand(node->child1);
        else {
            array = compileExpression(node->child1->child1);
            index = compileExpression(node->child1->child2);
            destination = new CFGOperand(
                typeEvaluator->getExpressionType(node));
            if (node->child2->type != AST_ASSIGN)
                statements.push_back(
                    new CFGStatement(CFG_ARRAY_GET, destination, array, index));
        }
        CFGOperand* source = compileExpression(node->child3);
        if (node->child2->type != AST_ASSIGN)
            source = appendBinaryArithmeticExpression(
                node,
                opForAssignmentType(node->child2->type),
                destination,
                source);
        statements.push_back(new CFGStatement(CFG_ASSIGN, destination, source));
        if (node->child1->type == AST_IDENTIFIER)
            setPromotedVarOperands(node->child1);
        else
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
     * @param args a vector in which to store the arguments' values.
     */
    void compileMethodCallArgList(ASTNode* node, vector<CFGOperand*>& args) {
        if (node->type != AST_EXPRESSION_LIST)
            args.push_back(compileExpression(node));
        else {
            compileMethodCallArgList(node->child1, args);
            args.push_back(compileExpression(node->child2));
        }
    }
    
    /**
     * Compiles the specified method call node.
     * @param node the node.
     * @return a CFGOperand storing the results of the expression.  Returns NULL
     *     if it is a void method.
     */
    CFGOperand* compileMethodCall(ASTNode* node) {
        wstring identifier = node->child1->tokenStr;
        MethodInterface* interface;
        CFGOperand* destination;
        int numArgs;
        if (methodInterfaces.count(identifier) == 0) {
            emitError(node, L"Calling an unknown method");
            destination = NULL;
            numArgs = -1;
        } else {
            interface = methodInterfaces[identifier];
            if (interface->getReturnType() != NULL)
                destination = new CFGOperand(
                    typeEvaluator->getExpressionType(node));
            else
                destination = NULL;
            numArgs = (int)interface->getArgTypes().size();
        }
        vector<CFGOperand*> args;
        if (node->child2 != NULL)
            compileMethodCallArgList(node->child2, args);
        if ((int)args.size() < numArgs)
            emitError(node, L"Too few arguments to method call");
        else if ((int)args.size() > numArgs) {
            if (numArgs >= 0)
                emitError(node, L"Too many arguments to method call");
        }
        CFGStatement* statement = new CFGStatement(
            CFG_METHOD_CALL,
            destination,
            NULL);
        statement->setMethodIdentifierAndArgs(identifier, args);
        statements.push_back(statement);
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
                assert(!L"TODO array literals");
                return NULL;
            case AST_ARRAY_GET:
            {
                CFGOperand* array = compileExpression(node->child1);
                CFGOperand* index = compileExpression(node->child2);
                CFGOperand* destination = new CFGOperand(
                    typeEvaluator->getExpressionType(node));
                statements.push_back(
                    new CFGStatement(CFG_ARRAY_GET, destination, array, index));
                return destination;
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
                assert(!L"TODO classes");
                return NULL;
            case AST_FALSE:
            case AST_FLOAT_LITERAL:
            case AST_INT_LITERAL:
            case AST_TRUE:
                return getOperandForLiteral(node);
            case AST_IDENTIFIER:
                return getVarOperand(node);
            case AST_METHOD_CALL:
            {
                CFGOperand* destination = compileMethodCall(node);
                if (destination != NULL)
                    return destination;
                else {
                    emitError(
                        node,
                        L"Cannot use the return value of void method");
                    return new CFGOperand(0);
                }
            }
            case AST_POST_DECREMENT:
            case AST_POST_INCREMENT:
            case AST_PRE_DECREMENT:
            case AST_PRE_INCREMENT:
                return compileIncrementExpression(node);
            case AST_QUALIFIED_IDENTIFIER:
                assert(!L"TODO classes");
                return NULL;
            default:
                assert(!L"Unhandled expression type");
        }
        return NULL;
    }
    
    /**
     * Compiles the specified variable declaration item node (the
     * "varDeclarationItem" rule in grammar.y).
     */
    void compileVarDeclarationItem(ASTNode* node) {
        if (node->type == AST_ASSIGNMENT_EXPRESSION)
            compileAssignmentExpression(node);
    }
    
    /**
     * Compiles the specified variable declaration list node (the
     * "varDeclarationList" rule in grammar.y).
     */
    void compileVarDeclarationList(ASTNode* node) {
        if (node->type != AST_VAR_DECLARATION_LIST)
            compileVarDeclarationItem(node);
        else {
            compileVarDeclarationList(node->child1);
            compileVarDeclarationItem(node->child2);
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
                compileStatement(node->child4);
                statements.push_back(CFGStatement::fromLabel(continueLabel));
                compileStatementList(node->child3);
                statements.push_back(CFGStatement::jump(startLabel));
                break;
            }
            case AST_FOR_IN:
            case AST_FOR_IN_DECLARED:
            {
                CFGOperand* collection = compileExpression(node->child3);
                CFGLabel* startLabel = new CFGLabel();
                CFGLabel* bodyLabel = new CFGLabel();
                CFGOperand* index = new CFGOperand(REDUCED_TYPE_INT);
                CFGOperand* length = new CFGOperand(REDUCED_TYPE_INT);
                CFGOperand* anotherIteration = new CFGOperand(
                    REDUCED_TYPE_BOOL);
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
                
                CFGOperand* element = new CFGOperand(
                    typeEvaluator->getExpressionType(node->child1));
                statements.push_back(
                    new CFGStatement(
                        CFG_ARRAY_GET,
                        element,
                        collection,
                        index));
                
                compileStatement(node->child4);
                statements.push_back(CFGStatement::fromLabel(continueLabel));
                statements.push_back(
                    new CFGStatement(
                        CFG_PLUS,
                        index,
                        index,
                        new CFGOperand(1)));
                statements.push_back(CFGStatement::jump(startLabel));
                break;
            }
            case AST_WHILE:
            {
                CFGLabel* bodyLabel = new CFGLabel();
                statements.push_back(CFGStatement::fromLabel(continueLabel));
                compileConditionalJump(node->child1, bodyLabel, endLabel);
                statements.push_back(CFGStatement::fromLabel(bodyLabel));
                compileStatement(node->child2);
                statements.push_back(CFGStatement::jump(continueLabel));
                break;
            }
            default:
                assert(!L"Unhandled loop type");
                break;
        }
        statements.push_back(CFGStatement::fromLabel(endLabel));
        breakEvaluator->popBreakLabel();
        breakEvaluator->popContinueLabel();
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
        // TODO require case labels to be in the range [-128, 127] for the Byte
        // type
        if (node->type == AST_EMPTY_CASE_LIST)
            return;
        assert(node->type == AST_CASE_LIST || !L"Not a case list");
        compileCaseList(
            node->child1,
            node->child2,
            switchValues,
            switchLabels,
            switchValueInts,
            haveEncounteredDefault);
        if (node->child2->type == AST_CASE_LABEL_DEFAULT) {
            if (haveEncounteredDefault)
                emitError(node, L"Duplicate default label");
            haveEncounteredDefault = true;
            switchValues.push_back(NULL);
        } else {
            CFGOperand* value = getOperandForLiteral(node->child2->child1);
            int intValue;
            if (value->getType() != REDUCED_TYPE_INT)
                // Long
                intValue = 0;
            else {
                intValue = value->getIntValue();
                if (switchValueInts.count(value->getIntValue()) > 0)
                    emitError(node, L"Duplicate case label");
            }
            switchValueInts.insert(intValue);
            switchValues.push_back(value);
        }
        
        if (node->child3->type != AST_EMPTY_STATEMENT_LIST &&
            nextCaseLabelNode != NULL &&
            !breakEvaluator->alwaysBreaks(node->child3))
            emitError(
                nextCaseLabelNode,
                L"Falling through in a switch statement is not permitted.  "
                L"Perhaps you are missing a break statement.");
        CFGLabel* label = new CFGLabel();
        switchLabels.push_back(label);
        statements.push_back(CFGStatement::fromLabel(label));
        if (node->child3->type != AST_EMPTY_STATEMENT_LIST)
            compileStatementList(node->child3);
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
            if (numLoopsOperand->getType() == REDUCED_TYPE_LONG) {
                emitError(
                    node,
                    L"Number of loops must be an integer literal, not a long "
                    L"literal");
                delete numLoopsOperand;
                return false;
            } else {
                assert(
                    numLoopsOperand->getType() == REDUCED_TYPE_INT ||
                    !L"Unexpected literal type");
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
                        emitError(node, L"Attempting to break out of non-loop");
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
                        emitError(node, L"Attempting to continue non-loop");
                        return;
                    }
                }
                break;
            }
            case AST_RETURN:
                if (node->child1 != NULL) {
                    CFGOperand* operand = compileExpression(node->child1);
                    if (breakEvaluator->getReturnVar() == NULL) {
                        emitError(
                            node,
                            L"Cannot return a value from a void method");
                        delete operand;
                    } else
                        statements.push_back(
                            new CFGStatement(
                                CFG_ASSIGN,
                                breakEvaluator->getReturnVar(),
                                operand));
                } else if (breakEvaluator->getReturnVar() != NULL)
                    emitError(node, L"Must return a non-void value");
                label = breakEvaluator->getReturnLabel();
                break;
            default:
                assert(!L"Unhanded control flow statement type");
        }
        statements.push_back(CFGStatement::jump(label));
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
                statements.push_back(CFGStatement::fromLabel(trueLabel));
                compileStatement(node->child2);
                statements.push_back(CFGStatement::jump(finishLabel));
                statements.push_back(CFGStatement::fromLabel(falseLabel));
                compileStatement(node->child3);
                statements.push_back(CFGStatement::fromLabel(finishLabel));
                break;
            }
            case AST_SWITCH:
            {
                CFGLabel* finishLabel = new CFGLabel();
                breakEvaluator->pushBreakLabel(finishLabel);
                CFGOperand* operand = compileExpression(node->child1);
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
                }
                statement->setSwitchValuesAndLabels(switchValues, switchLabels);
                statements.push_back(CFGStatement::fromLabel(finishLabel));
                breakEvaluator->popBreakLabel();
                break;
            }
            default:
                assert(!L"Unhanded selection statement type");
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
                compileVarDeclarationList(node->child1);
                break;
            default:
                assert(!L"Unhandled statement type");
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
     * Creates a CFGOperand for the argument indicated by the specified argument
     * item node (the "argItem" rule in grammar.y).
     * @param node the node.
     * @param args a vector to which to append the CFGOperand.
     * @param argTypes a vector to which to append the argument's type.
     */
    void createArgItemVar(
        ASTNode* node,
        vector<CFGOperand*>& args,
        vector<CFGType*>& argTypes) {
        assert(node->child3 == NULL || !L"TODO default arguments");
        CFGType* type = ASTUtil::getCFGType(node->child1);
        argTypes.push_back(type);
        CFGOperand* var = new CFGOperand(
            type->getReducedType(),
            node->child2->tokenStr,
            false);
        argVars[node->child2->tokenStr] = var;
        args.push_back(var);
    }
    
    /**
     * Creates CFGOperands for the arguments indicated by the specified argument
     * list node (the "argList" rule in grammar.y).
     * @param node the node.
     * @param args a vector to which to append the CFGOperands.
     * @param argTypes a vector to which to append the arguments' types.
     */
    void createArgListVars(
        ASTNode* node,
        vector<CFGOperand*>& args,
        vector<CFGType*>& argTypes) {
        if (node->type != AST_ARG_LIST)
            createArgItemVar(node, args, argTypes);
        else {
            createArgListVars(node->child1, args, argTypes);
            createArgItemVar(node->child2, args, argTypes);
        }
    }
    
    /**
     * Returns the compiled CFGMethod representation of the specified node of
     * type AST_METHOD_DEFINITION.  (Assumes that all of the class's fields are
     * already available in "fieldVars" and the like.)
     */
    CFGMethod* compileMethodDefinition(ASTNode* node) {
        wstring identifier = node->child2->tokenStr;
        argVars.clear();
        vector<CFGOperand*> args;
        vector<CFGType*> argTypes;
        if (node->child4 != NULL)
            createArgListVars(node->child3, args, argTypes);
        varIDs = VarResolver::resolveVars(node, fieldIdentifiers, errors);
        typeEvaluator = new TypeEvaluator();
        typeEvaluator->evaluateTypes(
            node,
            fieldTypes,
            varIDs,
            methodInterfaces,
            errors);
        CFGOperand* returnVar;
        CFGType* returnType;
        if (node->child1->type == AST_VOID) {
            returnVar = NULL;
            returnType = NULL;
        } else {
            returnType = ASTUtil::getCFGType(node->child1);
            returnVar = new CFGOperand(returnType->getReducedType());
        }
        CFGLabel* returnLabel = new CFGLabel();
        breakEvaluator = new BreakEvaluator(returnVar, returnLabel);
        statements.clear();
        ASTNode* statementListNode;
        if (node->child4 != NULL)
            statementListNode = node->child4;
        else
            statementListNode = node->child3;
        
        compileStatementList(statementListNode);
        if (returnVar != NULL &&
            !breakEvaluator->alwaysBreaks(statementListNode))
            emitError(node, L"Method may finish without returning a value");
        statements.push_back(CFGStatement::fromLabel(returnLabel));
        for (map<CFGReducedType, map<int, CFGOperand*>*>::const_iterator
                 iterator = varIDToOperands.begin();
             iterator != varIDToOperands.end();
             iterator++)
            delete iterator->second;
        varIDToOperands.clear();
        delete breakEvaluator;
        breakEvaluator = NULL;
        delete typeEvaluator;
        typeEvaluator = NULL;
        return new CFGMethod(
            node->child2->tokenStr,
            returnVar,
            returnType,
            args,
            argTypes,
            statements);
    }
    
    /**
     * Appends the types of the arguments indicated by the specified argument
     * list node (the "argList" rule in grammar.y) to "argTypes".
     */
    void getArgTypes(ASTNode* node, vector<CFGType*>& argTypes) {
        // TODO default arguments
        if (node->type != AST_ARG_LIST)
            argTypes.push_back(ASTUtil::getCFGType(node->child1));
        else {
            getArgTypes(node->child1, argTypes);
            argTypes.push_back(ASTUtil::getCFGType(node->child2->child1));
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
        argTypes.push_back(new CFGType(L"Object"));
        methodInterfaces[L"print"] = new MethodInterface(
            NULL,
            argTypes,
            L"print");
        argTypes.clear();
        argTypes.push_back(new CFGType(L"Object"));
        methodInterfaces[L"println"] = new MethodInterface(
            NULL,
            argTypes,
            L"println");
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
            returnType = ASTUtil::getCFGType(node->child2->child1);
        else
            returnType = NULL;
        vector<CFGType*> argTypes;
        if (node->child2->child4 != NULL)
            getArgTypes(node->child2->child3, argTypes);
        wstring identifier = node->child2->child2->tokenStr;
        if (methodInterfaces.count(identifier) > 0)
            assert(!L"TODO method overloading");
        methodInterfaces[identifier] = new MethodInterface(
            returnType,
            argTypes,
            identifier);
    }
    
    /**
     * Adds information about the field indicated by the specified field
     * declaration item node (the "varDeclarationItem" rule in grammar.y) to
     * "fieldVars" and the like.
     * @param node the node.
     * @param type the type of the fields being declared.
     */
    void compileFieldDeclarationItem(ASTNode* node, CFGType* type) {
        wstring identifier;
        if (node->type != AST_ASSIGNMENT_EXPRESSION)
            identifier = node->tokenStr;
        else
            identifier = node->child1->tokenStr;
        CFGOperand* field = new CFGOperand(type->getReducedType());
        fieldVars[identifier] = field;
        fieldTypes[identifier] = type;
        fieldIdentifiers.insert(identifier);
        if (node->type == AST_ASSIGNMENT_EXPRESSION)
            compileAssignmentExpression(node);
    }
    
    /**
     * Adds information about the fields indicated by the specified field
     * declaration list node (the "varDeclarationList" rule in grammar.y) to
     * "fieldVars" and the like.
     * @param node the node.
     * @param type the type of the fields being declared.
     */
    void compileFieldDeclarationList(ASTNode* node, CFGType* type) {
        if (node->type != AST_VAR_DECLARATION_LIST)
            compileFieldDeclarationItem(node, type);
        else {
            compileFieldDeclarationList(node->child1, type);
            compileFieldDeclarationItem(node->child2, type);
        }
    }
    
    /**
     * Adds the fields declared in the specified class body item list node (the
     * "classBodyItemList" rule in grammar.y) to "fieldVars" and the like.
     */
    void compileFieldDeclarations(ASTNode* node) {
        if (node->type == AST_EMPTY_CLASS_BODY_ITEM_LIST)
            return;
        compileFieldDeclarations(node->child1);
        if (node->child2->type == AST_VAR_DECLARATION)
            compileFieldDeclarationList(
                node->child2->child2,
                ASTUtil::getCFGType(node->child2->child1));
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
        
        for (map<wstring, MethodInterface*>::const_iterator iterator =
                 methodInterfaces.begin();
             iterator != methodInterfaces.end();
             iterator++)
            delete iterator->second;
        methodInterfaces.clear();
        
        return new CFGClass(
            node->child1->tokenStr,
            fieldVars,
            fieldTypes,
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
        wstring filename,
        wostream& errorOutput) {
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

CFGFile* compileFile(ASTNode* node, wstring filename, wostream& errorOutput) {
    Compiler compiler;
    return compiler.compileFile(node, filename, errorOutput);
}
