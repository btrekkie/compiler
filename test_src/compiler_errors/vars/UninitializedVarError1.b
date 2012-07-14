class UninitializedVarError1 {
    void test(Int foo) {
        var bar;
        switch (foo) {
            case 1:
                bar = 1;
                break;
            case 2:
                bar = 3;
                break;
        }
        println(bar);
    }
}
