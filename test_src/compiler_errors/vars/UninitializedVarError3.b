class UninitializedVarError3 {
    void test(Int foo) {
        var bar;
        for (var i = 0; i < 5; i++) {
            if (i == foo)
                break;
            bar = i;
        }
        println(bar);
    }
}
