class MissingReturnValueError1 {
    Int test(Int foo) {
        switch (foo) {
            case 1:
                return 0;
            case 42:
                return 14;
        }
    }
}
