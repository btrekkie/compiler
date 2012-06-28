#ifndef CFG_HPP_INCLUDED
#define CFG_HPP_INCLUDED

#include <map>
#include <set>
#include <string>
#include <vector>

/**
 * A type of operation performed by CFGStatements.
 */
enum CFGOperation {
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
    CFG_MINUS,
    CFG_MOD,
    CFG_MULT,
    CFG_NEGATE, // destination = -source
    CFG_NOT, // destination = !source
    CFG_NOT_EQUALS,
    CFG_NOP, // No-op; statement has no effect
    CFG_PLUS,
    CFG_RIGHT_SHIFT,
    CFG_UNSIGNED_RIGHT_SHIFT,
    CFG_XOR,
};

/**
 * The compile-time type of a CFGOperand.
 */
class CFGType {
private:
    /**
     * Returns an integer indicating the relative level of promotion of this
     * type.  A greater number indicates a more promoted type.  Assumes
     * isNumeric() is true.  See the comments for "isMorePromotedThan" for more
     * information regardin promotion.
     */
    int getPromotionLevel();
public:
    /**
     * The type's class name.  If this is an array, "name" is the class name of
     * the array's (leaf) elements.
     */
    std::string className;
    /**
     * If this is an array, the number of dimensions in the array.  Otherwise,
     * "numDimensions" is 0.
     */
    int numDimensions;
    
    CFGType(std::string name2, int numDimensions2 = 0);
    /**
     * Returns a new CFGType for "Bool" values.
     */
    static CFGType* boolType();
    /**
     * Returns a new CFGType for "Int" values.
     */
    static CFGType* intType();
    /**
     * Returns whether this is the "Bool" type.
     */
    bool isBool();
    /**
     * Returns whether this is a numeric type; that is, whether it can be an
     * operation to addition.
     */
    bool isNumeric();
    /**
     * Returns whether this is an integer-like value: "Byte", "Int", or "Long"
     * (but not "Char").
     */
    bool isIntegerLike();
    /**
     * Returns whether "other" can be promoted to this.  Assumes that
     * isNumeric() and other->isNumeric() are true.  Returns false if this is
     * the same type as "other".
     * 
     * Promotion is a "widening" of a data type to make it compatible in an
     * arithmetic expression.  For example, in the expression 1 + 2.0, we
     * promote the first operand from an Int to a Double, so that we may add two
     * values of the same type.
     */
    bool isMorePromotedThan(CFGType* other);
};

/**
 * An expression operand in a CFGStatement.  This can be either a variable or a
 * literal value.
 */
class CFGOperand {
public:
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
     * The compile-time type of this operand.
     */
    CFGType* type;
    /**
     * The identifier of this variable.  "identifier" is "" if this is a literal
     * value, or if it does not appear in the source file, but rather is an
     * intermediate variable for an expression.
     */
    std::string identifier;
    /**
     * The boolean value of this operand, if this is a literal boolean.
     */
    bool boolValue;
    /**
     * The integer value of this operand, if this is a literal integer.
     */
    int intValue;
private:
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
    explicit CFGOperand(CFGType* type2);
    /**
     * Coinstructs a new CFGOperand for a variable.
     */
    CFGOperand(CFGType* type2, std::string identifier2, bool isField2);
    /**
     * Constructs a new CFGOperand for a literal integer value.
     */
    explicit CFGOperand(int value);
    /**
     * Returns a new CFGOperand for the literal Int value 1.
     */
    static CFGOperand* one();
    /**
     * Returns a new CFGOperand for a literal boolean value.
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
public:
    /**
     * The operation the statement performs.
     */
    CFGOperation op;
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
     * A CFGLabel identifying the statement.
     */
    CFGLabel* label;
    /**
     * A CFGLabel indicating where to jump in the event that the results of the
     * operation are true, if any.  For CFG_JUMP operations, this is the label
     * to which to jump.
     */
    CFGLabel* trueLabel;
    /**
     * A CFGLabel indicating where to jump in the event that the results of the
     * operation are false, if any.  For CFG_JUMP operations, this is the label
     * to which to jump.
     */
    CFGLabel* falseLabel;
    
    CFGStatement(
        CFGOperation op2,
        CFGOperand* destination2,
        CFGOperand* arg1b,
        CFGOperand* arg2b = NULL);
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
public:
    /**
     * The method's (unqualified) identifier.
     */
    std::string identifier;
    /**
     * A variable in which we store the method's return value.
     */
    CFGOperand* returnVar;
    /**
     * Variables indicating the methods arguments (in the order in which they
     * are declared).
     */
    std::vector<CFGOperand*> args;
    /**
     * The sequence of compiled statements indicating the method's
     * implementation.
     */
    std::vector<CFGStatement*> statements;
};

/**
 * A compiled class implementation.  For purposes of deallocation, the class is
 * the owner of the CFGMethods, CFGStatements, CFGOperands, CFGTypes, and
 * CFGLabels it stores.
 */
class CFGClass {
private:
    /**
     * A map from the (unqualified) identifiers of the class's methods to their
     * compiled implementations.  The way we store methods will need to change
     * when we add support for method overloading.
     */
    std::map<std::string, CFGMethod*> methods;
    
    /**
     * Deallocates the specified CFGStatements and their labels.
     * @param statements the statements.
     * @param operands a set to which to add the statements' operands (including
     *     null operands).
     */
    void deleteStatements(
        std::vector<CFGStatement*>& statements,
        std::set<CFGOperand*>& operands);
public:
    /**
     * The class's identifier.
     */
    std::string identifier;
    /**
     * A map from the (unqualified) identifier of the class's fields to the
     * CFGOperands for those variables.
     */
    std::map<std::string, CFGOperand*> fields;
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
    
    ~CFGClass();
    /**
     * Adds the specified compiled method to the list of this class's methods.
     */
    void addMethod(CFGMethod* method);
    /**
     * Returns a list of this class's methods, in an arbitrary order.
     */
    std::vector<CFGMethod*> getMethods();
};

/**
 * A compiled representation of a source code file.
 */
class CFGFile {
public:
    /**
     * The compiled implementation of the file's class.
     */
    CFGClass* clazz;
    
    ~CFGFile();
};

#endif
