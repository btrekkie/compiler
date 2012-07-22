/**
 * Tests pertaining to variable scopes.
 */
class Scope {
    /**
     * Tests that there is a separate variable scope for each case block.
     */
    void checkSwitchScope(Int input) {
        switch (input) {
            case 1:
                var x = 2.5;
                x += input;
                println(x);
                break;
            case 2:
            case 3:
                var x = 2;
                x += input;
                println(x);
                break;
            default:
                println(input);
        }
    }
    
    void testSwitchScope() {
        checkSwitchScope(1);
        checkSwitchScope(2);
        checkSwitchScope(3);
        checkSwitchScope(4);
    }
}
