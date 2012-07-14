class UninitializedVarError2 {
    void test(Int foo) {
        var bar;
        if (foo == 0)
            bar = 1;
        else
            foo = 2;
        println(bar);
    }
}
