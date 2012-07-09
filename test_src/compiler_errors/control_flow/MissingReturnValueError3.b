class MissingReturnValueError1 {
    Int test(Int foo) {
        for (Int i = 0; i < 5; i++)
            return i;
    }
}
