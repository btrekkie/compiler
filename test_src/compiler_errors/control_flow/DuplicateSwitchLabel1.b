class DuplicateSwitchLabel1 {
    void test() {
        switch (1) {
            case 1:
                break;
            case 2:
                break;
            case 1:
                break;
        }
    }
}
