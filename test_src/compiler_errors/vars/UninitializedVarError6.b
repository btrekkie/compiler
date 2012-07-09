class UninitializedVarError6 {
    void test(Int foo) {
        Int bar;
        Bool baz = foo < 5 && (bar = 3) < foo;
        println(bar);
    }
}
