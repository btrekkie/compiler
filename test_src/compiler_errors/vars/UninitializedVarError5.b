class UninitializedVarError5 {
    void test(Int foo) {
        Int bar;
        do {
            if (i == foo)
                break;
            bar = i;
            i++;
        } while (i < 5);
        println(bar);
    }
}
