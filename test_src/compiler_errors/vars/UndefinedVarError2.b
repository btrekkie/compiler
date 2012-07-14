class UndefinedVarError2 {
    void test() {
        for (var i = 0; i < 5; i++)
            println(i);
        println(i);
    }
}
