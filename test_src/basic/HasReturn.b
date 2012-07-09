/**
 * Tests that none of the below methods produces a "method may not return a
 * value" compiler error.
 */
class HasReturn {
    Int checkIf(Int foo) {
        if (foo == 1)
            return 2;
        else
            return 3;
    }
    
    Int checkSwitch(Int foo) {
        switch (foo) {
            case 1:
                return 0;
            case 2:
                return 3;
            default:
                return 2;
        }
    }
    
    void test() {
        println(0);
    }
}
