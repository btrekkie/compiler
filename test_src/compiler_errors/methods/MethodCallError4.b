class MethodCallError4 {
    void foo(Int value) {
        println(0);
    }
    
    void test() {
        println(foo(1));
    }
}
