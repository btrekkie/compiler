#include <algorithm>
#include <assert.h>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include "ast_util.hpp"
#include "break_evaluator.hpp"
#include "cfg.hpp"
#include "compiler.hpp"
#include "cpp_compiler.hpp"

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
     * A vector to which to append the compiled statements.
     */
    vector<CFGStatement*> statements;
    /**
     * A map from the identifiers of fields and local variables to their
     * corresponding CFGOperands.
     */
    map<string, CFGOperand*> allVars;
    /**
     * A stack of maps indicating the variables in the current frame / scope.
     * Each element is a map from the identifiers of fields and local variables
     * in the frame to their corresponding CFGOperands.
     */
    vector<map<string, CFGOperand*>*> frameVars;
    /**
     * A BreakEvaluator maintaining compiler state pertaining to control flow
     * statements in the current method, if any: break statements, continue
     * statements, and return statements.
     */
    BreakEvaluator* breakEvaluator;
    
    /**
     * Outputs a compiler error.
     * @param node the node at which error appears.
     * @param error the text of the error.
     */
    void emitError(ASTNode* node, string error) {
        cerr << "Error at node of type " << node->type << ": " << error << '\n';
        assert(false);
    }
    
    /**
     * Returns the CFGOperand for the specified node of type AST_IDENTIFIER.
     * Emits a compiler error if the variable is not declared.
     */
    CFGOperand* getVarOperand(ASTNode* node) {
        assert(node->type == AST_IDENTIFIER || !"Not a variable");
        if (allVars.count(node->tokenStr) == 0)
            emitError(node, "Variable not declared in this scope");
        return allVars[node->tokenStr];
    }
    
    /**
     * Creates and returns a new CFGOperand for a variable.
     * @param type the type of the variable.
     * @param node the node in which the variable is declared.  The method emits
     *     an error if there is already a variable with the same identifier.
     * @param identifier the variable's identifier.
     * @param isField whether the variable is a class field.
     * @return the operand.
     */
    CFGOperand* createVar(
        CFGType* type,
        ASTNode* node,
        string identifier,
        bool isField) {
        if (allVars.count(identifier) > 0) {
            if (isField)
                emitError(node, "Multiple fields with the same identifier");
            else
                emitError(node, "Multiple variables with the same identifier");
        }
        CFGOperand* var = new CFGOperand(type, identifier, isField);
        (*(frameVars.back()))[identifier] = var;
        allVars[identifier] = var;
        return var;
    }
    
    /**
     * Pushes a frame / scope for variables to the stack of frames.
     */
    void pushFrame() {
        frameVars.push_back(new map<string, CFGOperand*>());
    }
    
    /**
     * Pops a frame / scope for variables from the stack of frames.
     */
    void popFrame() {
        map<string, CFGOperand*>* frame = frameVars.back();
        for (map<string, CFGOperand*>::const_iterator iterator = frame->begin();
             iterator != frame->end();
             iterator++)
            allVars.erase(iterator->first);
        delete frame;
        frameVars.pop_back();
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
     * Compiles the specified node of type AST_[PRE|POST]_[INCREMENT|DECREMENT].
     * @param node the node to compile.
     * @return a CFGOperand storing the results of the expression.
     */
    CFGOperand* compileIncrementExpression(ASTNode* node) {
        assert(node->child1->type == AST_IDENTIFIER || !"TODO arrays");
        CFGOperand* operand = getVarOperand(node->child1);
        if (!operand->getType()->isNumeric())
            emitError(
                node,
                "Increment / decrement postfix / prefix operator may only be "
                "used on numbers");
        switch (node->type) {
            case AST_POST_DECREMENT:
            case AST_POST_INCREMENT:
            {
                CFGOperand* result = new CFGOperand(operand->getType());
                appendAssignmentStatement(node, result, operand);
                statements.push_back(
                    new CFGStatement(
                        node->type == AST_POST_INCREMENT ? CFG_PLUS : CFG_MINUS,
                        operand,
                        operand,
                        CFGOperand::one()));
                break;
            }
            case AST_PRE_DECREMENT:
            case AST_PRE_INCREMENT:
                statements.push_back(
                    new CFGStatement(
                        node->type == AST_PRE_INCREMENT ? CFG_PLUS : CFG_MINUS,
                        operand,
                        operand,
                        CFGOperand::one()));
                return operand;
            default:
                assert(!"Unhandled increment type");
        }
        return NULL;
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
                if (!operand->getType()->isBool())
                    emitError(node, "Boolean result is required");
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
            !source2->getType()->isNumeric())
            emitError(node, "Operands to arithmetic operator must be numbers");
        
        CFGType* destinationType;
        switch (operation) {
            case CFG_BITWISE_AND:
            case CFG_BITWISE_OR:
            case CFG_XOR:
                if (!source1->getType()->isIntegerLike() ||
                    !source2->getType()->isIntegerLike())
                    emitError(node, "Operand must be of an integer-like type");
                if (source1->getType()->isMorePromotedThan(source2->getType()))
                    destinationType = source1->getType();
                else
                    destinationType = source2->getType();
                break;
            case CFG_LEFT_SHIFT:
            case CFG_RIGHT_SHIFT:
            case CFG_UNSIGNED_RIGHT_SHIFT:
                if (!source1->getType()->isIntegerLike())
                    emitError(node, "Operand must be of an integer-like type");
                if (source2->getType()->getNumDimensions() > 0 ||
                    (source2->getType()->getClassName() != "Int" &&
                     source2->getType()->getClassName() != "Byte"))
                    emitError(
                        node,
                        "Operand to bit shift must be integer or byte");
                destinationType = source1->getType();
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
        if (type1->getNumDimensions() > 0 || type2->getNumDimensions() > 0)
            return new CFGType("Object");
        else if (type1->isBool() && type2->isBool())
            return type1;
        else if (type1->isNumeric() && type2->isNumeric()) {
            if (type1->isMorePromotedThan(type2))
                return type1;
            else
                return type2;
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
            case AST_INT_LITERAL:
                return new CFGOperand(ASTUtil::getIntLiteralValue(node));
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
            case AST_NEGATE:
            {
                CFGOperand* operand = compileExpression(node->child1);
                if (!operand->getType()->isIntegerLike())
                    emitError(node, "Operand must be of an integer-like type");
                CFGOperand* destination = new CFGOperand(operand->getType());
                statements.push_back(
                    new CFGStatement(
                        opForExpressionType(node->type),
                        destination,
                        operand));
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
                statements.push_back(
                    new CFGStatement(
                        opForExpressionType(node->type),
                        destination,
                        source1,
                        source2));
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
                if (!operand->getType()->isBool())
                    emitError(node, "Operand must be a boolean");
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
        assert(node->child1->type == AST_IDENTIFIER || !"TODO arrays");
        CFGOperand* destination = getVarOperand(node->child1);
        CFGOperand* source = compileExpression(node->child3);
        if (node->child2->type != AST_ASSIGN)
            source = appendBinaryArithmeticExpression(
                node,
                opForAssignmentType(node->child2->type),
                destination,
                source);
        appendAssignmentStatement(node, destination, source);
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
            case AST_ARRAY_GET:
            case AST_ARRAY_LENGTH:
                assert(!"TODO arrays");
                return NULL;
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
                assert(!"TODO classes");
                return NULL;
            case AST_FALSE:
            case AST_INT_LITERAL:
            case AST_TRUE:
                return getOperandForLiteral(node);
            case AST_IDENTIFIER:
                return getVarOperand(node);
            case AST_METHOD_CALL:
            case AST_NEW:
                assert(!"TODO classes");
                return NULL;
            case AST_POST_DECREMENT:
            case AST_POST_INCREMENT:
            case AST_PRE_DECREMENT:
            case AST_PRE_INCREMENT:
                return compileIncrementExpression(node);
            default:
                assert(!"Unhandled expression type");
        }
        return NULL;
    }
    
    /**
     * Compiles the specified variable declaration item node (the
     * "varDeclarationItem" rule in grammar.y).
     * @param node the node.
     * @param type the type of the variable being declared.
     * @param isField whether we are declaring a field, as opposed to a local
     *     variable.
     */
    void compileVarDeclarationItem(ASTNode* node, CFGType* type, bool isField) {
        string identifier;
        if (node->type == AST_ASSIGNMENT_EXPRESSION)
            identifier = node->child1->tokenStr;
        else
            identifier = node->tokenStr;
        CFGOperand* destination = createVar(type, node, identifier, isField);
        if (node->type == AST_ASSIGNMENT_EXPRESSION) {
            CFGOperand* source = compileExpression(node->child3);
            appendAssignmentStatement(node, destination, source);
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
     */
    CFGType* getCFGType(ASTNode* node) {
        if (node->type == AST_TYPE_ARRAY) {
            CFGType* childType = getCFGType(node->child1);
            CFGType* type = new CFGType(
                childType->getClassName(),
                childType->getNumDimensions() + 1);
            delete childType;
            return type;
        } else {
            assert(node->type == AST_TYPE || !"Not a type node");
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
                compileStatement(node->child2);
                statements.push_back(CFGStatement::fromLabel(continueLabel));
                compileConditionalJump(node->child1, startLabel, endLabel);
                break;
            }
            case AST_FOR:
            {
                CFGLabel* startLabel = new CFGLabel();
                CFGLabel* conditionLabel = new CFGLabel();
                compileStatementList(node->child1);
                statements.push_back(CFGStatement::jump(conditionLabel));
                statements.push_back(CFGStatement::fromLabel(startLabel));
                compileStatement(node->child4);
                statements.push_back(CFGStatement::fromLabel(continueLabel));
                compileStatementList(node->child3);
                statements.push_back(CFGStatement::fromLabel(conditionLabel));
                compileConditionalJump(node->child2, startLabel, endLabel);
                break;
            }
            case AST_FOR_IN:
                assert(!"TODO arrays / classes");
                break;
            case AST_WHILE:
            {
                CFGLabel* startLabel = new CFGLabel();
                statements.push_back(CFGStatement::jump(continueLabel));
                statements.push_back(CFGStatement::fromLabel(startLabel));
                compileStatement(node->child2);
                statements.push_back(CFGStatement::fromLabel(continueLabel));
                compileConditionalJump(node->child1, startLabel, endLabel);
                break;
            }
            default:
                assert(!"Unhandled loop type");
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
        statements.push_back(CFGStatement::fromLabel(label));
        compileStatementList(node->child3);
    }
    
    /**
     * Compiles the specified control flow statement node (a break, continue, or
     * return statement).
     */
    void compileControlFlowStatement(ASTNode* node) {
        switch (node->type) {
            case AST_BREAK:
            {
                int numLoops;
                if (node->child1 == NULL)
                    numLoops = 1;
                else {
                    numLoops = atoi(node->child1->tokenStr);
                    if (numLoops <= 0) {
                        emitError(node, "Must break out of at least one loop");
                        break;
                    }
                }
                CFGLabel* breakLabel = breakEvaluator->getBreakLabel(numLoops);
                if (breakLabel == NULL)
                    emitError(node, "Attempting to break out of non-loop");
                statements.push_back(CFGStatement::jump(breakLabel));
                break;
            }
            case AST_CONTINUE:
            {
                int numLoops;
                if (node->child1 == NULL)
                    numLoops = 1;
                else {
                    numLoops = atoi(node->child1->tokenStr);
                    if (numLoops <= 0) {
                        emitError(node, "Must continue at least one loop");
                        break;
                    }
                }
                CFGLabel* continueLabel = breakEvaluator->getContinueLabel(
                    numLoops);
                if (continueLabel == NULL)
                    emitError(node, "Attempting to continue non-loop");
                statements.push_back(CFGStatement::jump(continueLabel));
                break;
            }
            case AST_RETURN:
                if (node->child1 != NULL) {
                    CFGOperand* operand = compileExpression(node->child1);
                    if (breakEvaluator->getReturnVar() == NULL) {
                        emitError(
                            node,
                            "Cannot return a value from a void method");
                        break;
                    }
                    appendAssignmentStatement(
                        node,
                        breakEvaluator->getReturnVar(),
                        operand);
                } else if (breakEvaluator->getReturnVar() != NULL)
                    emitError(node, "Must return a non-void value");
                statements.push_back(
                    CFGStatement::jump(breakEvaluator->getReturnLabel()));
                break;
            default:
                assert(!"Unhanded control flow statement type");
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
                bool haveEncounteredDefault;
                compileCaseList(
                    node->child2,
                    NULL,
                    switchValues,
                    switchLabels,
                    switchValueInts,
                    haveEncounteredDefault);
                statement->setSwitchValuesAndLabels(switchValues, switchLabels);
                statements.push_back(CFGStatement::fromLabel(finishLabel));
                breakEvaluator->popBreakLabel();
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
                pushFrame();
                compileStatementList(node->child1);
                popFrame();
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
                assert(!"TODO classes");
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
                    getCFGType(node->child1),
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
            getCFGType(node->child1),
            node,
            node->child2->tokenStr,
            false);
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
            args.push_back(createArgItemVar(node));
        }
    }
    
    /**
     * Returns the compiled CFGMethod representation of the specified node of
     * type AST_METHOD_DEFINITION.  (Assumes that all of the class's fields are
     * already available in "allVars".)
     */
    CFGMethod* compileMethodDefinition(ASTNode* node) {
        string identifier = node->child2->tokenStr;
        pushFrame();
        vector<CFGOperand*> args;
        if (node->child4 != NULL)
            createArgListVars(node->child3, args);
        CFGOperand* returnVar;
        if (node->child1->type == AST_VOID)
            returnVar = NULL;
        else {
            CFGType* type = getCFGType(node->child1);
            returnVar = new CFGOperand(type);
        }
        CFGLabel* returnLabel = new CFGLabel();
        breakEvaluator = new BreakEvaluator(returnVar, returnLabel);
        statements.clear();
        if (node->child4 != NULL)
            compileStatementList(node->child4);
        else
            compileStatementList(node->child3);
        popFrame();
        statements.push_back(CFGStatement::fromLabel(returnLabel));
        delete breakEvaluator;
        breakEvaluator = NULL;
        return new CFGMethod(
            node->child2->tokenStr,
            returnVar,
            args,
            statements);
    }
    
    /**
     * Adds the fields declared in the specified class body item node (the
     * "classBodyItem" rule in grammar.y) to "allVars" and "frameVars".
     */
    void compileFieldDeclarations(ASTNode* node) {
        if (node->type == AST_EMPTY_CLASS_BODY_ITEM_LIST)
            return;
        compileFieldDeclarations(node->child1);
        if (node->child2->type == AST_VAR_DECLARATION)
            compileVarDeclarationList(
                node->child2->child2,
                getCFGType(node->child2->child1),
                true);
    }
    
    /**
     * Adds the compiled CFGMethod representations of the methods defined in the
     * specified class body item node (the "classBodyItem" rule in grammar.y) to
     * "clazz".
     */
    void compileMethodDefinitions(ASTNode* node, vector<CFGMethod*>& methods) {
        if (node->type == AST_EMPTY_CLASS_BODY_ITEM_LIST)
            return;
        compileMethodDefinitions(node->child1, methods);
        if (node->child2->type == AST_METHOD_DEFINITION)
            methods.push_back(compileMethodDefinition(node->child2));
    }
    
    /**
     * Returns the compiled CFGMethod representation of the specified node of
     * type AST_CLASS_DEFINITION.
     */
    CFGClass* compileClass(ASTNode* node) {
        statements.clear();
        pushFrame();
        compileFieldDeclarations(node->child2);
        map<string, CFGOperand*> fields = allVars;
        popFrame();
        vector<CFGStatement*> initStatements = statements;
        vector<CFGMethod*> methods;
        compileMethodDefinitions(node->child2, methods);
        return new CFGClass(
            node->child1->tokenStr,
            fields,
            methods,
            initStatements);
    }
public:
    Compiler() {
        breakEvaluator = NULL;
    }
    
    CFGFile* compileFile(ASTNode* node) {
        return new CFGFile(compileClass(node->child1));
    }
};

void processFile(ASTNode* node) {
    Compiler compiler;
    CFGFile* file = compiler.compileFile(node);
    outputCPPHeaderFile(file, cout);
    cout << '\n';
    outputCPPImplementationFile(file, cout);
    delete file;
}
