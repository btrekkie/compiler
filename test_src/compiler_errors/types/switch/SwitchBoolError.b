class SwitchBoolError {
    void test() {
        switch (false) {
            case 1:
                println(2);
                break;
            case 3:
                println(4);
                break;
        }
    }
}
