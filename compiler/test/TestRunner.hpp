#ifndef TEST_RUNNER_HPP_INCLUDED
#define TEST_RUNNER_HPP_INCLUDED

#include <string>
#include <vector>

class TestCase;

/**
 * A class for executing unit tests.
 */
class TestRunner {
public:
    /**
     * Executes the specified unit tests.  Outputs the results to cout and cerr.
     */
    void runTestCases(std::vector<TestCase*> testCases);
    /**
     * Called by TestCase to indicate a successful assertion.
     */
    void assertionPassed();
    /**
     * Called by TestCase to indicate an unsuccessful assertion.  Throws an
     * exception.
     * @param message a string describing the failure.
     */
    void assertionFailed(std::string message);
};

#endif
