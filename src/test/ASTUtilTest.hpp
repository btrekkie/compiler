#ifndef AST_UTIL_TEST_HPP_INCLUDED
#define AST_UTIL_TEST_HPP_INCLUDED

#include "TestCase.hpp"

class ASTUtilTest : public TestCase {
private:
    /**
     * Asserts that ASTUtil.getIntLiteralValue gives "expected" when passed
     * "str".
     */
    void checkIntLiteralValue(std::string str, long long expected);
    /**
     * Asserts that ASTUtile.getInttLiteralValue returns false and does not
     * alter its second argument when passed "str".
     */
    void checkTooLarge(std::string str);
public:
    std::string getName();
    void test();
};

#endif
