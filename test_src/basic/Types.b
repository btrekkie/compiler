/**
 * Contains tests pertaining to data types.
 */
class Types {
    /**
     * Tests a floating point number that is equal to an integer.  In
     * particular, this ensures that CPPCompiler writes the operand as "2.0f"
     * and not "2f".
     */
    void testFloatThatIsAnInt() {
        var x = 2.0f;
        println(x);
    }
}
