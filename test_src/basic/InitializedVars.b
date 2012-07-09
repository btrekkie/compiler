/**
 * Tests that none of the below methods produces a "variable used but not
 * initialized" compiler error.
 */
class InitializedVars {
    void checkAnd(Int foo) {
        Int bar;
        Bool baz = (bar = 3) > foo && foo + bar < 10;
        println(bar);
    }
    
    void checkBreak() {
        for (Int i = 0; i < 5; i++) {
            Int bar;
            Int j = 0;
            do {
                if (i + j == 3)
                    break 2;
                bar = 3;
            } while (j < 5);
            println(bar);
        }
    }
    
    void checkForCondition() {
        Int bar;
        for (Int i = 0; i < (bar = 5); i++)
            println(i);
        println(bar);
    }
    
    void checkSwitch(Int foo) {
        Int bar;
        switch (foo) {
            case 1:
                bar = 0;
                break;
            case 2:
                bar = 3;
                break;
            default:
                bar = 2;
                break;
        }
        println(bar);
    }
    
    void test() {
        println(0);
    }
}
