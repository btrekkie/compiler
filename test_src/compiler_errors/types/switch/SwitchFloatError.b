class SwitchFloatError {
    void test() {
        switch (0.0) {
            case 1:
                println(2);
                break;
            case 3:
                println(4);
                break;
        }
    }
}
