/**
 * Tests if statements, if-else statements, complex conditions, and switch
 * statements.
 * /* Testing nested comment */
 */
class Branching {
    Int getBitCountUsingBinarySearch(Int value) {
        // Inline comment
        var hasAtLeastOneBit = (value & 8) != 0 || (value & 4) != 0 ||
            (value & 2) != 0 || (value & 1) != 0; // Another inline comment
        var hasAtLeastTwoBits =
            ((value & 2) != 0 && (value & 1) != 0) ||
            ((value & 4) != 0 && (value & 1) != 0) ||
            ((value & 4) != 0 && (value & 2) != 0) ||
            ((value & 8) != 0 && (value & 1) != 0) ||
            ((value & 8) != 0 && (value & 2) != 0) ||
            ((value & 8) != 0 && (value & 4) != 0);
        var hasAtLeastThreeBits =
            ((value & 4) != 0 && (value & 2) != 0 && (value & 1) != 0) ||
            ((value & 8) != 0 && (value & 2) != 0 && (value & 1) != 0) ||
            ((value & 8) != 0 && (value & 4) != 0 && (value & 1) != 0) ||
            ((value & 8) != 0 && (value & 4) != 0 && (value & 2) != 0);
        var hasFourBits = (value & 8) != 0 && (value & 4) != 0 &&
            (value & 2) != 0 && (value & 1) != 0;
        if (!hasAtLeastTwoBits) {
            if (!hasAtLeastOneBit)
                return 0;
            else
                return 1;
        } else if (!hasAtLeastThreeBits)
            return 2;
        else if (!hasFourBits)
            return 3;
        else
            return 4;
    }
    
    void testBitCountUsingBinarySearch() {
        for (var i = 0; i < 32; i++)
            print(getBitCountUsingBinarySearch(i));
        println(0);
    }
    
    Int getBitCountUsingBools(Int value) {
        var bit3 = (value & 8) != 0;
        var bit2 = (value & 4) != 0;
        var bit1 = (value & 2) != 0;
        var bit0 = (value & 1) != 0;
        if (!bit3 && !bit2 && !bit1 && !bit0)
            return 0;
        else if ((!bit3 && !bit2 && !bit1 && bit0) ||
                 (!bit3 && !bit2 && bit1 && !bit0) ||
                 (!bit3 && bit2 && !bit1 && !bit0) ||
                 (bit3 && !bit2 && !bit1 && !bit0))
            return 1;
        else if ((!bit3 && !bit2 && bit1 && bit0) ||
                 (!bit3 && bit2 && !bit1 && bit0) ||
                 (!bit3 && bit2 && bit1 && !bit0) ||
                 (bit3 && !bit2 && !bit1 && bit0) ||
                 (bit3 && !bit2 && bit1 && !bit0) ||
                 (bit3 && bit2 && !bit1 && !bit0))
            return 2;
        else if ((!bit3 && bit2 && bit1 && bit0) ||
                 (bit3 && !bit2 && bit1 && bit0) ||
                 (bit3 && bit2 && !bit1 && bit0) ||
                 (bit3 && bit2 && bit1 && !bit0))
            return 3;
        else
            return 4;
    }
    
    void testBitCountUsingBools() {
        for (var i = 0; i < 32; i++)
            print(getBitCountUsingBools(i));
        println(0);
    }
    
    Int getBitCountUsingElseif(Int value) {
        if ((value & 8) == 0 && (value & 4) == 0 && (value & 2) == 0 &&
            (value & 1) == 0)
            return 0;
        else if (((value & 8) == 0 && (value & 4) == 0 && (value & 2) == 0 &&
                  (value & 1) != 0) ||
                 ((value & 8) == 0 && (value & 4) == 0 && (value & 2) != 0 &&
                  (value & 1) == 0) ||
                 ((value & 8) == 0 && (value & 4) != 0 && (value & 2) == 0 &&
                  (value & 1) == 0) ||
                 ((value & 8) != 0 && (value & 4) == 0 && (value & 2) == 0 &&
                  (value & 1) == 0))
            return 1;
        else if (((value & 8) == 0 && (value & 4) == 0 && (value & 2) != 0 &&
                  (value & 1) != 0) ||
                 ((value & 8) == 0 && (value & 4) != 0 && (value & 2) == 0 &&
                  (value & 1) != 0) ||
                 ((value & 8) == 0 && (value & 4) != 0 && (value & 2) != 0 &&
                  (value & 1) == 0) ||
                 ((value & 8) != 0 && (value & 4) == 0 && (value & 2) == 0 &&
                  (value & 1) != 0) ||
                 ((value & 8) != 0 && (value & 4) == 0 && (value & 2) != 0 &&
                  (value & 1) == 0) ||
                 ((value & 8) != 0 && (value & 4) != 0 && (value & 2) == 0 &&
                  (value & 1) == 0))
            return 2;
        else if (((value & 8) == 0 && (value & 4) != 0 && (value & 2) != 0 &&
                  (value & 1) != 0) ||
                 ((value & 8) != 0 && (value & 4) == 0 && (value & 2) != 0 &&
                  (value & 1) != 0) ||
                 ((value & 8) != 0 && (value & 4) != 0 && (value & 2) == 0 &&
                  (value & 1) != 0) ||
                 ((value & 8) != 0 && (value & 4) != 0 && (value & 2) != 0 &&
                  (value & 1) == 0))
            return 3;
        else
            return 4;
    }
    
    void testBitCountUsingElseif() {
        for (var i = 0; i < 32; i++)
            print(getBitCountUsingElseif(i));
        println(0);
    }
    
    Int getBitCountUsingSwitch1(Int value) {
        var numOddZeroes = (((value & 8) == 0) ? 1 : 0) +
            (((value & 2) == 0) ? 1 : 0);
        var numEvenZeroes = (((value & 4) == 0) ? 1 : 0) +
            (((value & 1) == 0) ? 1 : 0);
        switch (numOddZeroes + numEvenZeroes) {
            case 0:
                break;
            case 1:
                return 3;
            case 2:
                return 2;
            case 3:
                return 1;
            case 4:
                return 0;
        }
        return 4;
    }
    
    void testBitCountUsingSwitch1() {
        for (var i = 0; i < 32; i++)
            print(getBitCountUsingSwitch1(i));
        println(0);
    }
    
    Int getBitCountUsingSwitch2(Int value) {
        var numOddZeroes = (((value & 8) == 0) ? 1 : 0) +
            (((value & 2) == 0) ? 1 : 0);
        var numEvenZeroes = (((value & 4) == 0) ? 1 : 0) +
            (((value & 1) == 0) ? 1 : 0);
        switch (numOddZeroes + numEvenZeroes) {
            case 0:
                return 4;
            case 1:
                return 3;
            case 3:
                return 1;
            case 4:
                return 0;
            default:
                return 2;
        }
    }
    
    void testBitCountUsingSwitch2() {
        for (var i = 0; i < 32; i++)
            print(getBitCountUsingSwitch2(i));
        println(0);
    }
    
    Int getBitCountUsingSwitch3(Int value) {
        var numOddZeroes = (((value & 8) == 0) ? 1 : 0) +
            (((value & 2) == 0) ? 1 : 0);
        var numEvenZeroes = (((value & 4) == 0) ? 1 : 0) +
            (((value & 1) == 0) ? 1 : 0);
        var bitCount;
        switch (numOddZeroes + numEvenZeroes) {
            case 0:
                return 4;
            case 1:
                bitCount = 3;
                break;
            case 2:
                return 2;
            case 3:
                bitCount = 1;
                break;
            default:
                return 0;
        }
        return bitCount;
    }
    
    void testBitCountUsingSwitch3() {
        for (var i = 0; i < 32; i++)
            print(getBitCountUsingSwitch3(i));
        println(0);
    }
    
    Int getBitCountUsingSwitch4(Int value) {
        var numOddZeroes = (((value & 8) == 0) ? 1 : 0) +
            (((value & 2) == 0) ? 1 : 0);
        var numEvenZeroes = (((value & 4) == 0) ? 1 : 0) +
            (((value & 1) == 0) ? 1 : 0);
        var bitCount = 0;
        switch (numOddZeroes + numEvenZeroes) {
            case 0:
                bitCount = 4;
                break;
            case 1:
                return 3;
            case 2:
                bitCount = 2;
                break;
            case 3:
                return 1;
            case 4:
        }
        return bitCount;
    }
    
    void testBitCountUsingSwitch4() {
        for (var i = 0; i < 32; i++)
            print(getBitCountUsingSwitch4(i));
        println(0);
    }
    
    Int getBitCountUsingSwitch5(Int value) {
        var numOddZeroes = (((value & 8) == 0) ? 1 : 0) +
            (((value & 2) == 0) ? 1 : 0);
        var numEvenZeroes = (((value & 4) == 0) ? 1 : 0) +
            (((value & 1) == 0) ? 1 : 0);
        var bitCount = -1;
        switch (numOddZeroes + numEvenZeroes) {
            case 0:
                bitCount = 4;
                break;
            case 1:
                return 3;
            case 2:
                bitCount = 2;
                break;
            case 3:
                return 1;
            case 4:
                bitCount = 0;
        }
        return bitCount;
    }
    
    void testBitCountUsingSwitch5() {
        for (var i = 0; i < 32; i++)
            print(getBitCountUsingSwitch5(i));
        println(0);
    }
    
    Int getBitCountUsingSwitch6(Int value) {
        var numOddZeroes = (((value & 8) == 0) ? 1 : 0) +
            (((value & 2) == 0) ? 1 : 0);
        var numEvenZeroes = (((value & 4) == 0) ? 1 : 0) +
            (((value & 1) == 0) ? 1 : 0);
        switch (numOddZeroes + numEvenZeroes) {
            case 0:
            case 1:
                if (numOddZeroes + numEvenZeroes == 0)
                    return 4;
                else
                    return 3;
            case 3:
                return 1;
            case 4:
                return 0;
            default:
                return 2;
        }
    }
    
    void testBitCountUsingSwitch6() {
        for (var i = 0; i < 32; i++)
            print(getBitCountUsingSwitch6(i));
        println(0);
    }
    
    Int getBitCountUsingTernary(Int value) {
        var hasAtLeastOneBit = (value & 8) != 0 || (value & 4) != 0 ||
            (value & 2) != 0 || (value & 1) != 0;
        var hasAtLeastTwoBits =
            ((value & 2) != 0 && (value & 1) != 0) ||
            ((value & 4) != 0 && (value & 1) != 0) ||
            ((value & 4) != 0 && (value & 2) != 0) ||
            ((value & 8) != 0 && (value & 1) != 0) ||
            ((value & 8) != 0 && (value & 2) != 0) ||
            ((value & 8) != 0 && (value & 4) != 0);
        var hasAtLeastThreeBits =
            ((value & 4) != 0 && (value & 2) != 0 && (value & 1) != 0) ||
            ((value & 8) != 0 && (value & 2) != 0 && (value & 1) != 0) ||
            ((value & 8) != 0 && (value & 4) != 0 && (value & 1) != 0) ||
            ((value & 8) != 0 && (value & 4) != 0 && (value & 2) != 0);
        var hasFourBits = (value & 8) != 0 && (value & 4) != 0 &&
            (value & 2) != 0 && (value & 1) != 0;
        return !hasAtLeastTwoBits
            ? !hasAtLeastOneBit ? 0 : 1
            : !hasAtLeastThreeBits ? 2 : !hasFourBits ? 3 : 4;
    }
    
    void testBitCountUsingTernary() {
        for (var i = 0; i < 32; i++)
            print(getBitCountUsingTernary(i));
        println(0);
    }
}
