class Branching {
    Int getBitCountUsingBinarySearch(Int value) {
        Bool hasAtLeastOneBit = (value & 8) != 0 || (value & 4) != 0 ||
            (value & 2) != 0 || (value & 1) != 0;
        Bool hasAtLeastTwoBits =
            ((value & 2) != 0 && (value & 1) != 0) ||
            ((value & 4) != 0 && (value & 1) != 0) ||
            ((value & 4) != 0 && (value & 2) != 0) ||
            ((value & 8) != 0 && (value & 1) != 0) ||
            ((value & 8) != 0 && (value & 2) != 0) ||
            ((value & 8) != 0 && (value & 4) != 0);
        Bool hasAtLeastThreeBits =
            ((value & 4) != 0 && (value & 2) != 0 && (value & 1) != 0) ||
            ((value & 8) != 0 && (value & 2) != 0 && (value & 1) != 0) ||
            ((value & 8) != 0 && (value & 4) != 0 && (value & 1) != 0) ||
            ((value & 8) != 0 && (value & 4) != 0 && (value & 2) != 0);
        Bool hasFourBits = (value & 8) != 0 && (value & 4) != 0 &&
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
        for (Int i = 0; i < 32; i++)
            print(getBitCountUsingBinarySearch(i));
        println(0);
    }
    
    Int getBitCountUsingBools(Int value) {
        Bool bit3 = (value & 8) != 0;
        Bool bit2 = (value & 4) != 0;
        Bool bit1 = (value & 2) != 0;
        Bool bit0 = (value & 1) != 0;
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
        for (Int i = 0; i < 32; i++)
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
        for (Int i = 0; i < 32; i++)
            print(getBitCountUsingElseif(i));
        println(0);
    }
    
    Int getBitCountUsingTernary(Int value) {
        Bool hasAtLeastOneBit = (value & 8) != 0 || (value & 4) != 0 ||
            (value & 2) != 0 || (value & 1) != 0;
        Bool hasAtLeastTwoBits =
            ((value & 2) != 0 && (value & 1) != 0) ||
            ((value & 4) != 0 && (value & 1) != 0) ||
            ((value & 4) != 0 && (value & 2) != 0) ||
            ((value & 8) != 0 && (value & 1) != 0) ||
            ((value & 8) != 0 && (value & 2) != 0) ||
            ((value & 8) != 0 && (value & 4) != 0);
        Bool hasAtLeastThreeBits =
            ((value & 4) != 0 && (value & 2) != 0 && (value & 1) != 0) ||
            ((value & 8) != 0 && (value & 2) != 0 && (value & 1) != 0) ||
            ((value & 8) != 0 && (value & 4) != 0 && (value & 1) != 0) ||
            ((value & 8) != 0 && (value & 4) != 0 && (value & 2) != 0);
        Bool hasFourBits = (value & 8) != 0 && (value & 4) != 0 &&
            (value & 2) != 0 && (value & 1) != 0;
        return !hasAtLeastTwoBits
            ? !hasAtLeastOneBit ? 0 : 1
            : !hasAtLeastThreeBits ? 2 : !hasFourBits ? 3 : 4;
    }
    
    void testBitCountUsingTernary() {
        for (Int i = 0; i < 32; i++)
            print(getBitCountUsingTernary(i));
        println(0);
    }
}
