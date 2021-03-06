#ifndef INTERFACE_HPP_INCLUDED
#define INTERFACE_HPP_INCLUDED

#include <map>
#include <string>
#include <vector>

enum CFGReducedType {
    REDUCED_TYPE_BOOL,
    REDUCED_TYPE_BYTE,
    REDUCED_TYPE_INT,
    REDUCED_TYPE_LONG,
    REDUCED_TYPE_FLOAT,
    REDUCED_TYPE_DOUBLE,
    REDUCED_TYPE_OBJECT
};

/**
 * The compile-time type of a CFGOperand.
 */
class CFGType {
private:
    /**
     * The type's class name.  If this is an array, "className" is the class
     * name of the array's (leaf) elements.
     */
    std::wstring className;
    /**
     * If this is an array, the number of dimensions in the array.  Otherwise,
     * "numDimensions" is 0.
     */
    int numDimensions;
    /**
     * Returns an integer indicating the relative level of promotion of this
     * type.  A greater number indicates a more promoted type.  Assumes
     * isNumeric() is true.  See the comments for "isMorePromotedThan" for more
     * information regarding promotion.
     */
    int getPromotionLevel();
public:
    CFGType(std::wstring name2, int numDimensions2 = 0);
    CFGType(CFGType* copy);
    std::wstring getClassName();
    int getNumDimensions();
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
     * Returns whether this is an integer-like value: "Byte", "Int", or "Long".
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
    /**
     * Returns the code string representation of this type, e.g. L"Int[][]".
     */
    std::wstring toString();
    /**
     * Returns the CFGType represented by the specified string, or NULL if the
     * string does not represent a CFGType.  This is the inverse of toString().
     */
    static CFGType* fromString(std::wstring str);
    /**
     * Returns the reduced type of this type.
     */
    CFGReducedType getReducedType();
};

/**
 * The externally exposed interface of a class's field.
 */
class FieldInterface {
private:
    /**
     * The field's compile-time type.
     */
    CFGType* type;
    /**
     * The field's (unqualified) identifier.
     */
    std::wstring identifier;
public:
    FieldInterface(CFGType* type2, std::wstring identifier2);
    ~FieldInterface();
    CFGType* getType();
    std::wstring getIdentifier();
};

/**
 * The externally exposed interface of a class's method.
 */
class MethodInterface {
private:
    /**
     * The compile-time type of the method's return value, or NULL if the method
     * has no return value.
     */
    CFGType* returnType;
    /**
     * A vector of the method's arguments' compile-time types (in the order in
     * which they are declared).
     */
    std::vector<CFGType*> argTypes;
    /**
     * The method's (unqualified) identifier.
     */
    std::wstring identifier;
public:
    MethodInterface(
        CFGType* returnType2,
        std::vector<CFGType*> argTypes2,
        std::wstring identifier2);
    ~MethodInterface();
    CFGType* getReturnType();
    std::vector<CFGType*> getArgTypes();
    std::wstring getIdentifier();
};

/**
 * The externally exposed interface of a class.
 */
class ClassInterface {
private:
    /**
     * A map from the (unqualified) identifier of the class's fields to their
     * FieldInterfaces.
     */
    std::map<std::wstring, FieldInterface*> fields;
    /**
     * The interfaces for the class's methods.
     */
    std::vector<MethodInterface*> methods;
    std::wstring identifier;
public:
    ClassInterface(
        std::vector<FieldInterface*> fields2,
        std::vector<MethodInterface*> methods2,
        std::wstring identifier2);
    ~ClassInterface();
    std::vector<FieldInterface*> getFields();
    std::vector<MethodInterface*> getMethods();
    std::wstring getIdentifier();
};

#endif
