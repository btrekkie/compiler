class UninitializedVarError7 {
    void test(Int foo) {
        Int bar;
        do {
            foo++;
            break;
            bar = 0;
        } while (foo < 5);
        println(bar);
    }
}
