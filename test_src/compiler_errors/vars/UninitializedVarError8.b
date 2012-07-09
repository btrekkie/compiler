class UninitializedVarError8 {
    void test() {
        Int bar;
        for (Int i = 0; i < 5; bar = 3) {
            println(i);
            i++;
        }
        println(bar);
    }
}
