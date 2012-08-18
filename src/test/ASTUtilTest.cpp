#include "../ASTUtil.hpp"
#include "ASTUtilTest.hpp"

using namespace std;

void ASTUtilTest::checkIntLiteralValue(wstring str, long long expected) {
    long long value = expected + 1;
    assertTrue(
        ASTUtil::getIntLiteralValue(str, value),
        L"Erroneously claimed value was out of range");
    assertEqual(expected, value, L"Incorrect value");
}

void ASTUtilTest::checkTooLarge(wstring str) {
    long long value = 12345;
    assertFalse(
        ASTUtil::getIntLiteralValue(str, value),
        L"Failed to detect that value was too large");
    assertEqual(12345LL, value, L"Altered value");
}

wstring ASTUtilTest::getName() {
    return L"ASTUtilTest";
}

void ASTUtilTest::test() {
    checkIntLiteralValue(L"0", 0);
    checkIntLiteralValue(L"5", 5);
    checkIntLiteralValue(L"-8", -8);
    checkIntLiteralValue(L"12345", 12345);
    checkIntLiteralValue(L"-12345", -12345);
    checkIntLiteralValue(L"2147483647", 2147483647);
    checkIntLiteralValue(L"-2147483647", -2147483647);
    checkIntLiteralValue(L"-2147483648", -2147483648);
    checkIntLiteralValue(L"0x0", 0);
    checkIntLiteralValue(L"0x5", 5);
    checkIntLiteralValue(L"-0xb", -11);
    checkIntLiteralValue(L"0x12345", 74565);
    checkIntLiteralValue(L"0xdeadbeef", -559038737);
    
    checkIntLiteralValue(L"0L", 0);
    checkIntLiteralValue(L"5L", 5);
    checkIntLiteralValue(L"-8l", -8);
    checkIntLiteralValue(L"12345L", 12345);
    checkIntLiteralValue(L"-12345l", -12345);
    checkIntLiteralValue(L"9223372036854775807L", 9223372036854775807LL);
    checkIntLiteralValue(L"-9223372036854775807L", -9223372036854775807LL);
    checkIntLiteralValue(L"-9223372036854775808L", -9223372036854775807LL - 1);
    checkIntLiteralValue(L"0x0L", 0);
    checkIntLiteralValue(L"0x5L", 5);
    checkIntLiteralValue(L"-0xbl", -11);
    checkIntLiteralValue(L"0x12345L", 74565);
    checkIntLiteralValue(L"0xdeadbeef12345678l", -2401053092306725256LL);
    
    checkTooLarge(L"2147483648");
    checkTooLarge(L"123456789012");
    checkTooLarge(L"0x1234abcd12");
    checkTooLarge(L"-2147483649");
    checkTooLarge(L"-123456789012");
    checkTooLarge(L"-0x1234abcd12");
    checkTooLarge(L"9223372036854775808L");
    checkTooLarge(L"12345678901234567890L");
    checkTooLarge(L"0x1234abcd5678cdef12L");
    checkTooLarge(L"-9223372036854775809L");
    checkTooLarge(L"-12345678901234567890L");
    checkTooLarge(L"-0x1234abcd5678cdef12L");
}
