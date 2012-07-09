#include "../ASTUtil.hpp"
#include "ASTUtilTest.hpp"

using namespace std;

void ASTUtilTest::checkIntLiteralValue(string str, long long expected) {
    long long value = expected + 1;
    assertTrue(
        ASTUtil::getIntLiteralValue(str, value),
        "Erroneously claimed value was out of range");
    assertEqual(expected, value, "Incorrect value");
}

void ASTUtilTest::checkTooLarge(string str) {
    long long value = 12345;
    assertFalse(
        ASTUtil::getIntLiteralValue(str, value),
        "Failed to detect that value was too large");
    assertEqual(12345LL, value, "Altered value");
}

string ASTUtilTest::getName() {
    return "ASTUtilTest";
}

void ASTUtilTest::test() {
    checkIntLiteralValue("0", 0);
    checkIntLiteralValue("5", 5);
    checkIntLiteralValue("-8", -8);
    checkIntLiteralValue("12345", 12345);
    checkIntLiteralValue("-12345", -12345);
    checkIntLiteralValue("2147483647", 2147483647);
    checkIntLiteralValue("-2147483647", -2147483647);
    checkIntLiteralValue("-2147483648", -2147483648);
    checkIntLiteralValue("0x0", 0);
    checkIntLiteralValue("0x5", 5);
    checkIntLiteralValue("-0xb", -11);
    checkIntLiteralValue("0x12345", 74565);
    checkIntLiteralValue("0xdeadbeef", -559038737);
    
    checkIntLiteralValue("0L", 0);
    checkIntLiteralValue("5L", 5);
    checkIntLiteralValue("-8l", -8);
    checkIntLiteralValue("12345L", 12345);
    checkIntLiteralValue("-12345l", -12345);
    checkIntLiteralValue("9223372036854775807L", 9223372036854775807LL);
    checkIntLiteralValue("-9223372036854775807L", -9223372036854775807LL);
    checkIntLiteralValue("-9223372036854775808L", -9223372036854775807LL - 1);
    checkIntLiteralValue("0x0L", 0);
    checkIntLiteralValue("0x5L", 5);
    checkIntLiteralValue("-0xbl", -11);
    checkIntLiteralValue("0x12345L", 74565);
    checkIntLiteralValue("0xdeadbeef12345678l", -2401053092306725256LL);
    
    checkTooLarge("2147483648");
    checkTooLarge("123456789012");
    checkTooLarge("0x1234abcd12");
    checkTooLarge("-2147483649");
    checkTooLarge("-123456789012");
    checkTooLarge("-0x1234abcd12");
    checkTooLarge("9223372036854775808L");
    checkTooLarge("12345678901234567890L");
    checkTooLarge("0x1234abcd5678cdef12L");
    checkTooLarge("-9223372036854775809L");
    checkTooLarge("-12345678901234567890L");
    checkTooLarge("-0x1234abcd5678cdef12L");
}
