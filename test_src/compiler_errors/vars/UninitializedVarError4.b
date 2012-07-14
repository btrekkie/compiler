class UninitializedVarError4 {
    void test(Int foo) {
        var bar;
        var i = 0;
        while (i < foo) {
            i++;
            bar = i;
        }
        println(bar);
    }
}
