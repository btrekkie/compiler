class MissingReturnValueError1 {
    Int test(Int foo) {
        for (var i = 0; i < 5; i++)
            return i;
    }
}
