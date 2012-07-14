class DuplicateVarError2 {
    void test() {
        for (var i = 0; i < 5; i++) {
            for (var i = 0; i < 5; i++)
                println(i);
        }
    }
}
