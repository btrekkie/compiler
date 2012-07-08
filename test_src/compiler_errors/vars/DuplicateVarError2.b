class DuplicateVarError2 {
    void test() {
        for (Int i = 0; i < 5; i++) {
            for (Int i = 0; i < 5; i++)
                println(i);
        }
    }
}
