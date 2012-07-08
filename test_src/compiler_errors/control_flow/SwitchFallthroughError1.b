class SwitchFallthroughError1 {
    void test() {
        Int value = 1;
        switch (1) {
            case 1:
                break;
            case 2:
                value = 2;
            case 3:
                value = 3;
                break;
        }
        println(value);
    }
}
