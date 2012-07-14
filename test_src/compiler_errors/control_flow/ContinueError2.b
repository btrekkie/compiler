class ContinueError2 {
    void test() {
        for (var i = 0; i < 5; i++) {
            for (var j = 0; j < 5; j++) {
                for (var k = 0; k < 5; k++) {
                    println(i + j + k);
                    break 0;
                }
            }
        }
    }
}
