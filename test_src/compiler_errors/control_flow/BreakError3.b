class BreakError3 {
    void test() {
        for (Int i = 0; i < 5; i++) {
            for (Int j = 0; j < 5; j++) {
                for (Int k = 0; k < 5; k++) {
                    println(i + j + k);
                    break -1;
                }
            }
        }
    }
}
