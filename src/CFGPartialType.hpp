#ifndef CFG_PARTIAL_TYPE_HPP_INCLUDED
#define CFG_PARTIAL_TYPE_HPP_INCLUDED

class CFGType;

/**
 * The type of an expression in the source file.
 * 
 * While at present, we have the complete type of each expression, with future
 * language changes, our information may be partial.  For example, in the case
 * of a null value, we do not know the exact type of the null value, but we know
 * that it is not a primitive value.  In the case of a literal array, like
 * "[1.0f, 2]", we do not immediately know the type of the array.  The example
 * code might indicate an array of type Float, Double, or Object.
 */
class CFGPartialType {
private:
    /**
     * The type of the expression.
     */
    CFGType* type;
public:
    CFGPartialType(CFGType* type2);
    ~CFGPartialType();
    CFGType* getType();
};

#endif
