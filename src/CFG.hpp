#ifndef CFG_HPP_INCLUDED
#define CFG_HPP_INCLUDED

#include <map>
#include <set>
#include <string>
#include <vector>
#include "Interface.hpp"

/**
 * A type of operation performed by CFGStatements.
 */
enum CFGOperation {
    CFG_ARRAY_GET, // destination = source1[source2] (with bounds checks)
    CFG_ARRAY_LENGTH, // destination = source1.length()
    CFG_ARRAY_SET, // destination[source1] = source2 (with bounds checks)
    CFG_ASSIGN, // destination = source1
    CFG_BITWISE_AND,
    CFG_BITWISE_INVERT,
    CFG_BITWISE_OR,
    CFG_DIV,
    CFG_EQUALS, // destination = (source1 == source2)
    CFG_GREATER_THAN,
    CFG_GREATER_THAN_OR_EQUAL_TO,
    CFG_IF, // if (destination) goto trueLabel; else goto falseLabel;
    CFG_JUMP, // goto trueLabel
    CFG_LEFT_SHIFT,
    CFG_LESS_THAN,
    CFG_LESS_THAN_OR_EQUAL_TO,
    CFG_METHOD_CALL,
    CFG_MINUS,
    CFG_MOD,
    CFG_MULT,
    CFG_NEGATE, // destination = -source1
    CFG_NOT, // destination = !source1
    CFG_NOT_EQUALS,
    CFG_NOP, // No-op; statement has no effect
    CFG_PLUS,
    CFG_RIGHT_SHIFT,
    CFG_SWITCH,
    CFG_UNSIGNED_RIGHT_SHIFT,
    CFG_XOR,
};

/**
 * An expression operand in a CFGStatement.  This can be either a variable or a
 * literal value.
 */
class CFGOperand {
private:
    /**
     * Whether this is a variable, as opposed to a literal value.
     */
    bool isVar;
    /**
     * Whether this is a field, as opposed to a local variable (or argument) or
     * a literal value.
     */
    bool isField;
    /**
     * The reduced compile-time type of this operand.
     */
    CFGReducedType type;
    /**
     * The identifier of this variable.  "identifier" is L"" if this is a
     * literal value, or if it does not appear in the source file, but rather is
     * an intermediate variable for an expression.
     */
    std::wstring identifier;
    /**
     * The boolean value of this operand, if this is a literal boolean.
     */
    bool boolValue;
    /**
     * The integer value of this operand, if this is a literal integer.
     */
    int intValue;
    /**
     * The long value of this operand, if this is a literal long.
     */
    long long longValue;
    /**
     * The float value of this operand, if this is a literal float.
     */
    float floatValue;
    /**
     * The double value of this operand, if this is a literal double.
     */
    double doubleValue;
    
    /**
     * Constructs a new CFGOperand for a literal boolean value.
     */
    explicit CFGOperand(bool value);
public:
    /**
     * Constructs a new CFGOperand for a temporary variable: a variable that
     * does not appear in the source file, but rather is an intermediate
     * variable for an expression.
     */
    explicit CFGOperand(CFGReducedType type2);
    /**
     * Coinstructs a new CFGOperand for a variable.
     */
    CFGOperand(CFGReducedType type2, std::wstring identifier2, bool isField2);
    /**
     * Constructs a new CFGOperand for a literal integer value.
     */
    explicit CFGOperand(int value);
    /**
     * Constructs a new CFGOperand for a literal long value.
     */
    explicit CFGOperand(long long value);
    /**
     * Constructs a new CFGOperand for a literal float value.
     */
    explicit CFGOperand(float value);
    /**
     * Constructs a new CFGOperand for a literal double value.
     */
    explicit CFGOperand(double value);
    bool getIsVar();
    bool getIsField();
    CFGReducedType getType();
    std::wstring getIdentifier();
    bool getBoolValue();
    int getIntValue();
    long long getLongValue();
    float getFloatValue();
    double getDoubleValue();
    /**
     * Returns a new CFGOperand for the literal Int value 1.
     */
    static CFGOperand* one();
    /**
     * Returns a new CFGOperand for a literal Bool value.
     */
    static CFGOperand* fromBool(bool value);
};

/**
 * A label identifying a CFGStatement.
 */
class CFGLabel {
    
};

/**
 * A single "compiled" statement in a CFG (control flow graph) representation of
 * a source file.  At present, a program is not actually represented as a CFG,
 * but it probably will be in the future; the naming scheme is forward-looking.
 */
class CFGStatement {
private:
    /**
     * The operation the statement performs.
     */
    CFGOperation operation;
    /**
     * The variable in which to store the results of the operation, if any.
     */
    CFGOperand* destination;
    /**
     * The first operand to the operation, if any.
     */
    CFGOperand* arg1;
    /**
     * The second operand to the operation, if any.
     */
    CFGOperand* arg2;
    /**
     * The identifier of the method being called, if any.
     */
    std::wstring methodIdentifier;
    /**
     * The operands to the method being called, if any.
     */
    std::vector<CFGOperand*>* methodArgs;
    /**
     * A CFGLabel identifying the statement.
     */
    CFGLabel* label;
    /**
     * A vector of the literal CFGOperand values indicating the conditions under
     * which to jump to the labels in "switchLabels".  See the comments for
     * "switchLabels" for more information.
     */
    std::vector<CFGOperand*>* switchValues;
    /**
     * A vector of the CFGLabels to which to jump as a consequence of this
     * operation.  If the operation is not a jumping operation, this and
     * "switchValues" are NULL.  If it is CFG_JUMP, this consists of a single
     * element indicating the jump target, and "switchValues" consists of a
     * single NULL element.  If it is CFG_IF, this consists of two elements: the
     * label to which to jump if "arg1" evaluates to true, and the label to
     * which to jump if it evaluates to false, respecitvely.  "switchValues"
     * consists of two elements: a CFGOperand for true and NULL respectively.
     * 
     * If it is CFG_SWITCH, this consists of the labels to jump in each of the
     * cases.  If "arg1" is equal to switchValues[i], then we jump to
     * switchLabels[i].  A NULL element in "switchValues" indicates a default
     * label to which to jump if "arg1" does not match any of the other values.
     * All non-NULL elements of "switchValues" are literals of type Int.
     * 
     * Here is a C++ approximation of the CFG_SWITCH operation:
     * 
     * switch (arg1) {
     *     case switchValues[0]:
     *         goto switchLabels[0];
     *     case switchValues[1]:
     *         goto switchLabels[1];
     *     ...
     *     case switchValues[n]:
     *         goto switchLabels[n];
     * }
     */
    std::vector<CFGLabel*>* switchLabels;
public:
    CFGStatement(
        CFGOperation operation2,
        CFGOperand* destination2,
        CFGOperand* arg1b,
        CFGOperand* arg2b = NULL);
    ~CFGStatement();
    CFGOperation getOperation();
    CFGOperand* getDestination();
    CFGOperand* getArg1();
    CFGOperand* getArg2();
    std::wstring getMethodIdentifier();
    std::vector<CFGOperand*> getMethodArgs();
    CFGLabel* getLabel();
    /**
     * Sets "methodIdentifier" and "methodArgs".  This method may only be called
     * once.
     */
    void setMethodIdentifierAndArgs(
        std::wstring methodIdentifier2,
        std::vector<CFGOperand*> methodArgs2);
    /**
     * Returns the size of "switchLabels".  See the comments for that field for
     * more information.
     */
    int getNumSwitchLabels();
    /**
     * Returns switchValues.at(index).  See the comments for "switchValues" for
     * more information.
     */
    CFGOperand* getSwitchValue(int index);
    /**
     * Returns switchLabels.at(index).  See the comments for "switchLabels" for
     * more information.
     */
    CFGLabel* getSwitchLabel(int index);
    /**
     * Sets "switchValues" and "switchLabels".  This may only be called once.
     */
    void setSwitchValuesAndLabels(
        std::vector<CFGOperand*> switchValues2,
        std::vector<CFGLabel*> switchLabels2);
    /**
     * Returns a new CFGStatement of type CFG_NOP, associated with the specified
     * label.
     */
    static CFGStatement* fromLabel(CFGLabel* label2);
    /**
     * Returns a new CFGStatement for (unconditionally) jumping to the specified
     * label.
     */
    static CFGStatement* jump(CFGLabel* label2);
};

/**
 * A compiled method implementation.
 */
class CFGMethod {
private:
    /**
     * The method's (unqualified) identifier.
     */
    std::wstring identifier;
    /**
     * A variable in which we store the method's return value, or NULL if the
     * method does not have a return value.
     */
    CFGOperand* returnVar;
    /**
     * The type of the method's return value, or NULL if the method does not
     * have a return value.
     */
    CFGType* returnType;
    /**
     * Variables indicating the method's arguments (in the order in which they
     * are declared).
     */
    std::vector<CFGOperand*> args;
    /**
     * The types of the method's arguments (in the order in which they are
     * declared).
     */
    std::vector<CFGType*> argTypes;
    /**
     * The sequence of compiled statements indicating the method's
     * implementation.
     */
    std::vector<CFGStatement*> statements;
public:
    CFGMethod(
        std::wstring identifier2,
        CFGOperand* returnVar2,
        CFGType* returnType2,
        std::vector<CFGOperand*> args2,
        std::vector<CFGType*> argTypes2,
        std::vector<CFGStatement*> statements2);
    ~CFGMethod();
    std::wstring getIdentifier();
    CFGOperand* getReturnVar();
    std::vector<CFGOperand*> getArgs();
    std::vector<CFGStatement*> getStatements();
    /**
     * Returns the method's externally exposed interface.
     */
    MethodInterface* getInterface();
};

/**
 * A compiled class implementation.  For purposes of deallocation, the class is
 * the owner of the CFGMethods, CFGStatements, CFGOperands, and CFGLabels it
 * stores.
 */
class CFGClass {
private:
    /**
     * The class's identifier.
     */
    std::wstring identifier;
    /**
     * A map from the (unqualified) identifiers of the class's fields to the
     * CFGOperands for those variables.
     */
    std::map<std::wstring, CFGOperand*> fields;
    /**
     * A map from the (unqualified) identifiers of the class's fields to the
     * types of those variables.
     */
    std::map<std::wstring, CFGType*> fieldTypes;
    /**
     * A map from the (unqualified) identifiers of the class's methods to their
     * compiled implementations.  The way we store methods will need to change
     * when we add support for method overloading.
     */
    std::map<std::wstring, CFGMethod*> methods;
    /**
     * The sequence of compiled statements indicating the class's initialization
     * statements.  The initialization statements differ from the constructors
     * indicated in the source code.  They are executed immediately before the
     * body of such constructors.  For example, if a field is declared with the
     * statement "Int foo = 42;", one of the initialization statements will set
     * the value of "foo" to 42.  The statements do not include any
     * initialization that applies to every class, such as setting the object's
     * hidden field indicating its class.
     */
    std::vector<CFGStatement*> initStatements;
    
    /**
     * Adds the specified compiled method to the list of this class's methods.
     */
    void addMethod(CFGMethod* method);
    /**
     * Deallocates the specified CFGStatements and their labels.
     * @param statements the statements.
     * @param operands a set to which to add the statements' operands (including
     *     null operands).
     */
    void deleteStatements(
        std::vector<CFGStatement*> statements,
        std::set<CFGOperand*>& operands);
public:
    CFGClass(
        std::wstring identifier2,
        std::map<std::wstring, CFGOperand*> fields2,
        std::map<std::wstring, CFGType*> fieldTypes2,
        std::vector<CFGMethod*> methods2,
        std::vector<CFGStatement*> initStatements2);
    ~CFGClass();
    std::wstring getIdentifier();
    std::map<std::wstring, CFGOperand*> getFields();
    /**
     * Returns a list of this class's methods, in an arbitrary order.
     */
    std::vector<CFGMethod*> getMethods();
    std::vector<CFGStatement*> getInitStatements();
    /**
     * Returns the class's externally exposed interface.
     */
    ClassInterface* getInterface();
};

/**
 * A compiled representation of a source code file.
 */
class CFGFile {
private:
    /**
     * The compiled implementation of the file's class.
     */
    CFGClass* clazz;
public:
    CFGFile(CFGClass* clazz2);
    ~CFGFile();
    CFGClass* getClass();
};

#endif
