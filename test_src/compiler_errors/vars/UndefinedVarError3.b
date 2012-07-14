class UndefinedVarError3 {
    void test() {
        {
            var i = 0;
            println(i);
        }
        println(i);
    }
}
