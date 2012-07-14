class UninitializedVarError5 {
    void test(Int foo) {
        var bar;
        do {
            if (i == foo)
                break;
            bar = i;
            i++;
        } while (i < 5);
        println(bar);
    }
}
