#ifndef BINARY_COMPILER_TEST_HPP_INCLUDED
#define BINARY_COMPILER_TEST_HPP_INCLUDED

#include <map>
#include <string>

#include "TestCase.hpp"

/**
 * Unit test for BinaryCompiler.  The BinaryCompilerTest class compiles and runs
 * the source files in the "test_src" directory, making sure their results match
 * the results indicated in the "expected output files".  The expected output
 * file for a given class is in the same directory as the class's source file.
 * The name of the file is the name of the class, with a .txt suffix appended to
 * it.  Here is what an expected output file looks like:
 * 
 * testFoo:
 * 5
 * 13
 * 8
 * 
 * testBar:
 * 16
 * 4
 * 32
 * 
 * An expected output file consists of a sequence of expected outputs for each
 * "test method", that is, for each method whose identifier begins with "test".
 * The output for a given method is indicated with the method's identifier,
 * followed by a colon, followed by a newline, followed by the output.  The
 * expected output must end with exactly one newline character.  Any additional
 * newline characters in the expected output file are ignored.  In addition, no
 * line of the expected output may end in a colon.
 */
class BinaryCompilerTest : public TestCase {
private:
    /**
     * Returns the expected output indicated in the specified "expected output
     * file".  The returned map is a map from method identifiers to expected
     * output strings.
     */
    std::map<std::string, std::string> readExpectedOutput(
        std::string expectedOutputFilename);
    /**
     * Tests the specified file or the files in the specified directory, which
     * is specified relative to "testsrc".
     */
    void checkSourceFile(std::string file);
public:
    std::string getName();
    void test();
};

#endif
