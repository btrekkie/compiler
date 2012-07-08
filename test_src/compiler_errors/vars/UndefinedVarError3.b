class UndefinedVarError3 {
    void test() {
        {
            Int i = 0;
            println(i);
        }
        println(i);
    }
}
