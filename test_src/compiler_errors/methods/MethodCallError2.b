class MethodCallError2 {
    void foo(Int value) {
        println(0);
    }
    
    void test() {
        foo(1, 2);
    }
}
