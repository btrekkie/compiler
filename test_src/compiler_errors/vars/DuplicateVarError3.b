class DuplicateVarError3 {
    void test(Int input) {
        var x = 0;
        switch (input) {
            case 1:
                var x = 2;
                println(x);
                break;
        }
    }
}
