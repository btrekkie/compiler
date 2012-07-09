class UninitializedVarError4 {
    void test(Int foo) {
        Int bar;
        Int i = 0;
        while (i < foo) {
            i++;
            bar = i;
        }
        println(bar);
    }
}
