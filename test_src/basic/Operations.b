/**
 * Tests unary and binary operations.
 */
class Operations {
    void checkArithmeticOperations(Int value1, Int value2) {
        println(value1 + value2);
        println(value1 - value2);
        println(value1 * value2);
        println(value1 / value2);
        println(value1 % value2);
        println(-value1);
        println(value1 += value2);
        println(value1 -= value2);
        println(value1 *= value2);
        println(value1 /= value2);
        println(value1 %= value2);
    }
    
    /**
     * Tests arithmetic operations: addition, subtraction, multiplication,
     * division, mod, and negation.
     */
    void testArithmeticOperations() {
        checkArithmeticOperations(2, 7);
        checkArithmeticOperations(162, -37);
        checkArithmeticOperations(0, 12);
        checkArithmeticOperations(1234567890, 987654321);
        checkArithmeticOperations(1234567890, -987654321);
        checkArithmeticOperations(-1234567890, 987654321);
        checkArithmeticOperations(-1234567890, -987654321);
    }
    
    void checkBitwiseOperations(Int value1, Int value2) {
        println(value1 << value2);
        println(value1 >> value2);
        println(value1 >>> value2);
        println(value1 & value2);
        println(value1 | value2);
        println(value1 ^ value2);
        println(~value1);
        println(value1 <<= value2);
        println(value1 >>= value2);
        println(value1 >>>= value2);
        println(value1 &= value2);
        println(value1 |= value2);
        println(value1 ^= value2);
    }
    
    /**
     * Tests bitwise operations: left shift, right shift, unsigned right shift,
     * bitwise and, bitwise or, bitwise xor, and bitwise inverse.
     */
    void testBitwiseOperations() {
        checkBitwiseOperations(1234, 5);
        checkBitwiseOperations(1234567890, 3);
        checkBitwiseOperations(-1234567890, 3);
    }
}
