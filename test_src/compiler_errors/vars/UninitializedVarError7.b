class UninitializedVarError7 {
    void test(Int foo) {
        var bar;
        do {
            foo++;
            break;
            bar = 0;
        } while (foo < 5);
        println(bar);
    }
}
