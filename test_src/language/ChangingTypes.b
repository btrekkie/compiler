/**
 * Tests variables whose types change in the course of a method.
 */
class ChangingTypes {
    void checkDoWhileLoopTypes(Int input) {
        var foo = 1000040;
        var i = 0;
        println(foo);
        do {
            foo += 1000000.5;
            i++;
        } while (i < input);
        println(foo);
    }
    
    void testDoWhileLoopTypes() {
        checkDoWhileLoopTypes(0);
        checkDoWhileLoopTypes(1);
        checkDoWhileLoopTypes(2);
    }
    
    void checkForLoopConditionTypes(Int input) {
        var foo = 1000040;
        println(foo);
        for (var i = 0; (foo += 1000000.0) < input; i++);
        println(foo);
    }
    
    void testForLoopConditionTypes() {
        checkForLoopConditionTypes(0);
        checkForLoopConditionTypes(3000000);
    }
    
    void checkForLoopInitTypes(Int input) {
        var foo = 1000040;
        println(foo);
        for (foo = 1000000.0; foo < input; foo += 1000000);
        println(foo);
    }
    
    void testForLoopInitTypes() {
        checkForLoopConditionTypes(0);
        checkForLoopConditionTypes(3000000);
    }
    
    void checkForLoopMultipleIterationTypes(Int input) {
        var a = 1000040;
        var b = 1000040;
        var c = 1000040;
        var d = 1000040;
        var e = 1000040;
        println(a);
        println(b);
        println(c);
        println(d);
        println(e);
        for (var i = 0; i < input; i++) {
            a += b;
            b += c;
            c += d;
            d += e;
            e += 1000000.0;
        }
        println(a);
        println(b);
        println(c);
        println(d);
        println(e);
    }
    
    void testForLoopMultipleIterationTypes() {
        checkForLoopMultipleIterationTypes(0);
        checkForLoopMultipleIterationTypes(2);
        checkForLoopMultipleIterationTypes(5);
    }
    
    void testForLoopReversionTypes() {
        var foo;
        for (var i = 1000030; i < (foo = 1000040); i++)
            foo -= i + 1.5;
        println(foo);
    }
    
    void checkForLoopTypes(Int input) {
        var foo = 1000040;
        println(foo);
        for (var i = 0; i < input; i++)
            foo += 1000000.5;
        println(foo);
    }
    
    void testForLoopTypes() {
        checkForLoopTypes(0);
        checkForLoopTypes(1);
        checkForLoopTypes(2);
    }
    
    void checkIfDemotionTypes(Bool input) {
        var foo = 1000040.0;
        println(foo);
        if (input)
            foo = 2000040;
        println(foo);
    }
    
    void testIfDemotionTypes() {
        checkIfDemotionTypes(true);
        checkIfDemotionTypes(false);
    }
    
    void checkIfElseTypes(Bool input) {
        var foo = 1000040;
        println(foo);
        if (input)
            foo += 1000000;
        else
            foo += 2000000.5;
        println(foo);
    }
    
    void testIfElseTypes() {
        checkIfElseTypes(true);
        checkIfElseTypes(false);
    }
    
    void checkIfTypes(Bool input) {
        var foo = 1000040;
        println(foo);
        if (input)
            foo += 1000000.5;
        println(foo);
    }
    
    void testIfTypes() {
        checkIfTypes(true);
        checkIfTypes(false);
    }
    
    void testOverwritten() {
        var foo = 2;
        foo += 3;
        println(foo);
        foo = false;
        println(foo && 1 < 2);
    }
    
    void checkSwitch(Int input) {
        var foo = 1000040;
        println(foo);
        switch (input) {
            case 0:
                break;
            case 1:
                foo = 2000040.0;
                break;
            default:
                foo = 3000040;
        }
        println(foo);
    }
    
    void testSwitch() {
        checkSwitch(0);
        checkSwitch(1);
        checkSwitch(2);
    }
    
    void checkUnreachableLoopType(Bool input) {
        var foo = 0;
        for (var i = 0; i < 1; i++) {
            if (input) {
                return;
                foo = 1.5;
                break;
            } else
                foo = 1000040;
        }
        println(foo);
    }
    
    void testUnreachableLoopType() {
        checkUnreachableLoopType(true);
        checkUnreachableLoopType(false);
    }
    
    void checkUnreachableType(Bool input) {
        var foo;
        if (input) {
            return;
            foo = 1.5;
        } else
            foo = 1000040;
        println(foo);
    }
    
    void testUnreachableType() {
        checkUnreachableType(true);
        checkUnreachableType(false);
    }
    
    void checkWhileLoopTypes(Int input) {
        var foo = 1000040;
        println(foo);
        var i = 0;
        while (i < input) {
            foo += 1000000.5;
            i++;
        }
        println(foo);
    }
    
    void testWhileLoopTypes() {
        checkWhileLoopTypes(0);
        checkWhileLoopTypes(1);
        checkWhileLoopTypes(2);
    }
}
