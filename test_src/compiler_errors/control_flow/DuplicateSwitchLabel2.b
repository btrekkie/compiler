class DuplicateSwitchLabel2 {
    void test() {
        switch (1) {
            default:
                break;
            case 2:
                break;
            default:
                break;
        }
    }
}
