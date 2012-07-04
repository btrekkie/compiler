#ifndef BINARY_COMPILER_TEST_HPP_INCLUDED
#define BINARY_COMPILER_TEST_HPP_INCLUDED

#include "TestCase.hpp"

class BinaryCompilerTest : public TestCase {
private:
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
