class UninitializedVarError6 {
    void test(Int foo) {
        var bar;
        var baz = foo < 5 && (bar = 3) < foo;
        println(bar);
    }
}
