class UninitializedVarError8 {
    void test() {
        var bar;
        for (var i = 0; i < 5; bar = 3) {
            println(i);
            i++;
        }
        println(bar);
    }
}
