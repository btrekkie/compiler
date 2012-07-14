class SwitchFallthroughError2 {
    void test() {
        var value = 1;
        switch (1) {
            case 1:
                break;
            case 2:
                switch (value + 1) {
                    case 1:
                        return;
                    case 2:
                        return;
                    // No default label; fallthrough is possible
                }
            case 3:
                value = 3;
                break;
        }
        println(value);
    }
}
