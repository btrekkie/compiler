/**
 * Tests that none of the below methods produces a "variable used but not
 * initialized" compiler error.
 */
class InitializedVars {
    void checkAnd(Int foo) {
        var bar;
        var baz = (bar = 3) > foo && foo + bar < 10;
        println(bar);
    }
    
    void checkBreak() {
        for (var i = 0; i < 5; i++) {
            var bar;
            var j = 0;
            do {
                if (i + j == 3)
                    break 2;
                bar = 3;
            } while (j < 5);
            println(bar);
        }
    }
    
    void checkForCondition() {
        var bar;
        for (var i = 0; i < (bar = 5); i++)
            println(i);
        println(bar);
    }
    
    void checkSwitch(Int foo) {
        var bar;
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
