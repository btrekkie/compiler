class MissingReturnValueError2 {
    Int test(Int foo) {
        do {
            for (Int i = 0; i < 5; i++) {
                if (foo + i > 3)
                    break 2;
            }
            return foo;
        } while (foo > 5);
    }
}
