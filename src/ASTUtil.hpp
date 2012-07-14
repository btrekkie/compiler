#ifndef AST_UTIL_HPP_INCLUDED
#define AST_UTIL_HPP_INCLUDED

#include <string>
#include "grammar/ASTNode.h"

/**
 * Provides static utility methods pertaining to an AST.
 */
class ASTUtil {
private:
    /**
     * Returns the decimal representation of the specified hexadecimal
     * character.  Assumes that the character is a valid hexacedimal character.
     */
    static int hexDigitToInt(char c);
public:
    /**
     * Computes the integer or long literal value indicated by the specified
     * string.  Assumes that the string indicates such a value; that is, the
     * string matches one of the appropriate rules in grammar/tokens.l.
     * @param str the string to parse.
     * @param value a reference in which to store the results.
     * @return whether the literal value was within the proper integer or long
     *     bounds (-2147483648 to 2147483647 for integers and
     *     -9223372036854775808 to 9223372036854775807 for longs).  If the value
     *     is not in bounds, "value" is not altered.
     */
    static bool getIntLiteralValue(std::string str, long long& value);
};

#endif
