/**
 * Tests alteration of the value of a method's arguments.
 */
class ChangeArgs {
    void checkAlteration(Int value1, Double value2, Bool value3) {
        println(value1);
        println(value2);
        println(value3);
        value1 = 42;
        value2 = 14.5;
        value3 = true;
        println(value1);
        println(value2);
        println(value3);
    }
    
    void testAlteration() {
        checkAlteration(1, 2, false);
    }
}
