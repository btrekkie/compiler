class UndefinedVarError4 {
    void test(Int input) {
        switch (input) {
            case 1:
                var x = 1;
                println(x);
                break;
            case 2:
                println(x);
                break;
        }
    }
}
